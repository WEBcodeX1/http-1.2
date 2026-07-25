#include "xmlparser.hpp"

#include "static-dtd.hpp"
#include "xmlconstants.hpp"

#include <algorithm>
#include <atomic>
#include <coroutine>
#include <cstring>
#include <memory>
#include <utility>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/framework/XMLGrammarPoolImpl.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/validators/common/Grammar.hpp>

XERCES_CPP_NAMESPACE_USE

namespace
{

static std::atomic_uint32_t XercesRefCount{0};

template<typename T>
class Generator
{
public:
    struct promise_type {
        T CurrentValue{};

        Generator get_return_object()
        {
            return Generator(std::coroutine_handle<promise_type>::from_promise(*this));
        }

        std::suspend_always initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        std::suspend_always yield_value(T Value) noexcept
        {
            CurrentValue = std::move(Value);
            return {};
        }
        void return_void() {}
        void unhandled_exception() { std::terminate(); }
    };

    class iterator
    {
    public:
        explicit iterator(std::coroutine_handle<promise_type> Handle) : _Handle(Handle) {}

        iterator& operator++()
        {
            _Handle.resume();
            if (_Handle.done()) {
                _Handle = {};
            }
            return *this;
        }

        const T& operator*() const
        {
            return _Handle.promise().CurrentValue;
        }

        bool operator==(std::default_sentinel_t) const
        {
            return !_Handle;
        }

    private:
        std::coroutine_handle<promise_type> _Handle;
    };

    explicit Generator(std::coroutine_handle<promise_type> Handle) : _Handle(Handle) {}
    Generator(Generator&& Other) noexcept : _Handle(Other._Handle) { Other._Handle = {}; }

    ~Generator()
    {
        if (_Handle) {
            _Handle.destroy();
        }
    }

    iterator begin()
    {
        if (_Handle) {
            _Handle.resume();
            if (_Handle.done()) {
                return iterator({});
            }
        }
        return iterator(_Handle);
    }

    std::default_sentinel_t end() const { return {}; }

private:
    std::coroutine_handle<promise_type> _Handle;
};

std::string transcodeXMLCh(const XMLCh* Input)
{
    if (Input == nullptr) {
        return std::string();
    }

    char* Buffer = XMLString::transcode(Input);
    if (Buffer == nullptr) {
        return std::string();
    }

    std::string Value(Buffer);
    XMLString::release(&Buffer);
    return Value;
}

std::string trimCopy(const std::string& Value)
{
    const auto Begin = Value.find_first_not_of(" \t\n\r");
    if (Begin == std::string::npos) {
        return std::string();
    }

    const auto End = Value.find_last_not_of(" \t\n\r");
    return Value.substr(Begin, End - Begin + 1);
}

std::string getElementText(DOMElement* Element)
{
    std::string Value;
    DOMNodeList* Children = Element->getChildNodes();
    for (XMLSize_t Index = 0; Index < Children->getLength(); ++Index) {
        DOMNode* Child = Children->item(Index);
        const auto Type = Child->getNodeType();
        if (Type == DOMNode::TEXT_NODE || Type == DOMNode::CDATA_SECTION_NODE) {
            Value += transcodeXMLCh(Child->getNodeValue());
        }
    }

    return trimCopy(Value);
}

Generator<DOMElement*> iterateElementChildren(DOMElement* Parent)
{
    DOMNodeList* Children = Parent->getChildNodes();
    for (XMLSize_t Index = 0; Index < Children->getLength(); ++Index) {
        DOMNode* Child = Children->item(Index);
        if (Child->getNodeType() == DOMNode::ELEMENT_NODE) {
            co_yield static_cast<DOMElement*>(Child);
        }
    }
}

struct MessageSlice
{
    std::string_view Slice;
    std::size_t StartOffset;
};

Generator<MessageSlice> splitMessages(std::string_view Input, bool& FramingError)
{
    std::size_t Cursor = 0;
    std::size_t LastEnd = std::string_view::npos;

    while (true) {
        const std::size_t Start = Input.find(NLAP_XML_START_MARKER, Cursor);
        if (Start == std::string_view::npos) {
            break;
        }

        if (LastEnd != std::string_view::npos && Start != LastEnd) {
            FramingError = true;
            co_return;
        }

        const std::size_t End = Input.find(NLAP_XML_END_MARKER, Start);
        if (End == std::string_view::npos) {
            break;
        }

        const std::size_t MessageEnd = End + NLAP_XML_END_MARKER.size();
        co_yield MessageSlice{Input.substr(Start, MessageEnd - Start), Start};

        Cursor = MessageEnd;
        LastEnd = MessageEnd;
    }
}

std::string ensureParseableXML(std::string_view Message)
{
    std::string Parseable(Message);

    if (Parseable.find("<?xml") == std::string::npos) {
        Parseable.insert(0, NLAP_XML_DECLARATION);
    }

    if (Parseable.find("<!DOCTYPE") == std::string::npos) {
        const std::size_t DeclEnd = Parseable.find("?>");
        const std::string Doctype = "<!DOCTYPE NLAP SYSTEM \"" + std::string(NLAP_DTD_SYSTEM_PATH) + "\">";
        if (DeclEnd != std::string::npos) {
            Parseable.insert(DeclEnd + 2, Doctype);
        } else {
            Parseable.insert(0, Doctype);
        }
    }

    return Parseable;
}

class ParseErrorHandler : public HandlerBase
{
public:
    ParseErrorHandler() = default;

    void error(const SAXParseException& Exception) override
    {
        _HasAnyError = true;
        _LastMessage = transcodeXMLCh(Exception.getMessage());
    }

    void fatalError(const SAXParseException& Exception) override
    {
        _HasAnyError = true;
        _LastMessage = transcodeXMLCh(Exception.getMessage());
    }

    void warning(const SAXParseException&) override {}

    void resetErrors() override
    {
        _HasAnyError = false;
        _LastMessage.clear();
    }

    bool hasAnyError() const { return _HasAnyError; }
    const std::string& lastMessage() const { return _LastMessage; }

private:
    bool _HasAnyError = false;
    std::string _LastMessage;
};

void populateTree(DOMElement* Element, XMLNode& Node, std::string_view RawMessage, std::size_t& SearchOffset)
{
    bool HasElementChildren = false;

    for (DOMElement* Child : iterateElementChildren(Element)) {
        HasElementChildren = true;
        const std::string ChildName = transcodeXMLCh(Child->getTagName());
        XMLNode& ChildNode = Node[ChildName];
        populateTree(Child, ChildNode, RawMessage, SearchOffset);
    }

    if (HasElementChildren) {
        return;
    }

    const std::string Value = getElementText(Element);
    if (Value.empty()) {
        return;
    }

    std::size_t Found = RawMessage.find(Value, SearchOffset);
    if (Found == std::string_view::npos) {
        Found = RawMessage.find(Value);
    }
    if (Found == std::string_view::npos) {
        return;
    }

    Node.Address = RawMessage.data() + Found;
    Node.Length = Value.size();
    SearchOffset = Found + Value.size();
}

uint16_t parseMessage(
    const std::shared_ptr<void>& GrammarPool,
    std::string_view RawMessage,
    ResultTree_t& OutputTree)
{
    const std::string Parseable = ensureParseableXML(RawMessage);

    ParseErrorHandler SyntaxHandler;
    XercesDOMParser SyntaxParser;
    SyntaxParser.setErrorHandler(&SyntaxHandler);
    SyntaxParser.setValidationScheme(XercesDOMParser::Val_Never);
    SyntaxParser.setDoNamespaces(false);
    SyntaxParser.setDoSchema(false);

    MemBufInputSource SyntaxSource(
        reinterpret_cast<const XMLByte*>(Parseable.data()),
        Parseable.size(),
        "nlap-syntax",
        false
    );
    SyntaxParser.parse(SyntaxSource);

    if (SyntaxHandler.hasAnyError()) {
        return XML_ERROR_INVALID_SYNTAX;
    }

    ParseErrorHandler DTDHandler;
    auto* CastedPool = static_cast<XMLGrammarPool*>(GrammarPool.get());
    XercesDOMParser DTDParser(nullptr, XMLPlatformUtils::fgMemoryManager, CastedPool);

    DTDParser.setErrorHandler(&DTDHandler);
    DTDParser.setValidationScheme(XercesDOMParser::Val_Always);
    DTDParser.setDoNamespaces(false);
    DTDParser.setDoSchema(false);
    DTDParser.setLoadExternalDTD(false);
    DTDParser.useCachedGrammarInParse(true);
    DTDParser.setCreateEntityReferenceNodes(false);
    DTDParser.setValidationConstraintFatal(true);

    MemBufInputSource DTDSource(
        reinterpret_cast<const XMLByte*>(Parseable.data()),
        Parseable.size(),
        "nlap-dtd-validation",
        false
    );
    DTDParser.parse(DTDSource);

    if (DTDHandler.hasAnyError()) {
        return XML_ERROR_INVALID_CONTENT_DTD;
    }

    DOMDocument* Document = DTDParser.getDocument();
    if (Document == nullptr) {
        return XML_ERROR_INVALID_SYNTAX;
    }

    DOMElement* Root = Document->getDocumentElement();
    if (Root == nullptr) {
        return XML_ERROR_INVALID_SYNTAX;
    }

    const std::string RootName = transcodeXMLCh(Root->getTagName());
    XMLNode& RootNode = OutputTree[RootName];

    std::size_t SearchOffset = 0;
    for (DOMElement* Child : iterateElementChildren(Root)) {
        const std::string ChildName = transcodeXMLCh(Child->getTagName());
        XMLNode& ChildNode = RootNode[ChildName];
        populateTree(Child, ChildNode, RawMessage, SearchOffset);
    }

    return 0;
}

} // namespace

XMLParser::XMLParser(std::size_t ParseBufferSize)
    : _ParseBufferSize(ParseBufferSize)
{
    if (XercesRefCount.fetch_add(1) == 0) {
        XMLPlatformUtils::Initialize();
    }

    auto* GrammarPool = new XMLGrammarPoolImpl(XMLPlatformUtils::fgMemoryManager);
    _GrammarPool = std::shared_ptr<void>(
        GrammarPool,
        [](void* Pointer) { delete static_cast<XMLGrammarPoolImpl*>(Pointer); }
    );

    XercesDOMParser Loader(nullptr, XMLPlatformUtils::fgMemoryManager, GrammarPool);
    const std::string DTDSourceId(NLAP_DTD_SYSTEM_PATH);
    MemBufInputSource DTDSource(
        reinterpret_cast<const XMLByte*>(NLAP_DTD_HEX.data()),
        NLAP_DTD_HEX.size(),
        DTDSourceId.c_str(),
        false
    );

    Loader.loadGrammar(DTDSource, Grammar::DTDGrammarType, true);
    GrammarPool->lockPool();
}

XMLParser::~XMLParser()
{
    _GrammarPool.reset();
    if (XercesRefCount.fetch_sub(1) == 1) {
        XMLPlatformUtils::Terminate();
    }
}

void XMLParser::setParseBufferSize(std::size_t ParseBufferSize)
{
    _ParseBufferSize = ParseBufferSize;
}

std::size_t XMLParser::getParseBufferSize() const
{
    return _ParseBufferSize;
}

ParseResult_t XMLParser::parse(char* InputBuffer) const
{
    ParseResult_t Result;

    if (InputBuffer == nullptr) {
        Result.ErrorCode = XML_ERROR_INVALID_SYNTAX;
        return Result;
    }

    std::string_view InputBufferSV(InputBuffer);

    if (InputBufferSV.size() > _ParseBufferSize) {
        Result.ErrorCode = XML_ERROR_INVALID_FRAMING;
        return Result;
    }

    bool FramingError = false;
    for (const MessageSlice& Slice : splitMessages(InputBufferSV, FramingError)) {
        ResultTree_t Tree;
        const uint16_t MessageError = parseMessage(_GrammarPool, Slice.Slice, Tree);

        if (MessageError != 0) {
            Result.ErrorCode = MessageError;
            Result.Results.clear();
            return Result;
        }

        Result.Results.push_back(std::move(Tree));
    }

    if (FramingError) {
        Result.ErrorCode = XML_ERROR_INVALID_FRAMING;
        Result.Results.clear();
        return Result;
    }

    return Result;
}
