#include "xmlparser.hpp"
#include "xmlconstants.hpp"

#include <atomic>

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLException.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/framework/XMLGrammarPoolImpl.hpp>
#include <xercesc/validators/common/Grammar.hpp>

XERCES_CPP_NAMESPACE_USE

using namespace std;


// ---------------------------------------------------------------------------
//  Internal helpers
// ---------------------------------------------------------------------------

//- RAII wrapper: char* transcoded from XMLCh* (auto-release on destruction)
class _CStr
{
public:
    explicit _CStr(const XMLCh* str) : _p(XMLString::transcode(str)) {}
    ~_CStr() { if (_p) XMLString::release(&_p); }
    string str() const { return _p ? string(_p) : string(); }
    const char* c_str() const { return _p; }
private:
    char* _p;
};

//- RAII wrapper: XMLCh* transcoded from char* (auto-release on destruction)
class _XStr
{
public:
    explicit _XStr(const char* str) : _p(XMLString::transcode(str)) {}
    ~_XStr() { if (_p) XMLString::release(&_p); }
    operator const XMLCh*() const { return _p; }
private:
    XMLCh* _p;
};


// ---------------------------------------------------------------------------
//  Simple SAX error handler used during DOM parsing
// ---------------------------------------------------------------------------

class _NLAPErrorHandler : public HandlerBase
{
public:
    _NLAPErrorHandler() : _hasError(false) {}

    bool hasError() const { return _hasError; }

    void error(const SAXParseException&)      override { _hasError = true; }
    void fatalError(const SAXParseException&) override { _hasError = true; }
    void warning(const SAXParseException&)    override {}
    void resetErrors()                        override { _hasError = false; }

private:
    bool _hasError;
};


// ---------------------------------------------------------------------------
//  Entity resolver that blocks external entity loading (XXE prevention).
//  Only the known NLAP DTD SYSTEM identifier is allowed to pass through
//  (the pre-loaded grammar pool handles it without disk I/O).  All other
//  external entity references — which could be injected by a malicious
//  client — are resolved to an empty document.
// ---------------------------------------------------------------------------

class _BlockingEntityResolver : public HandlerBase
{
public:
    InputSource* resolveEntity(const XMLCh* /*publicId*/, const XMLCh* systemId) override
    {
        if (systemId != nullptr) {
            char* sysStr = XMLString::transcode(systemId);
            bool isNLAPDtd = (string(sysStr).find("nlap.dtd") != string::npos);
            XMLString::release(&sysStr);
            if (isNLAPDtd) {
                //- let Xerces handle it; the grammar pool will serve the
                //- pre-loaded DTD without loading from the file system
                return nullptr;
            }
        }
        //- block all other external entities (XXE prevention)
        static const char emptyDoc[] = "";
        return new MemBufInputSource(
            reinterpret_cast<const XMLByte*>(emptyDoc), 0, "blocked-entity"
        );
    }
};


// ---------------------------------------------------------------------------
//  Xerces reference counting for Initialize / Terminate
//  Thread-safe via std::atomic.
// ---------------------------------------------------------------------------

static atomic<int> _xercesRefCount{0};


// ---------------------------------------------------------------------------
//  DOM tree helper functions
// ---------------------------------------------------------------------------

//- Return the trimmed text content of a DOM element (concatenates TEXT and
//- CDATA child nodes; does NOT recurse into element children).
static string _getElementText(DOMElement* elem)
{
    string result;
    DOMNodeList* children = elem->getChildNodes();
    for (XMLSize_t i = 0; i < children->getLength(); ++i) {
        DOMNode* child = children->item(i);
        DOMNode::NodeType t = child->getNodeType();
        if (t == DOMNode::TEXT_NODE || t == DOMNode::CDATA_SECTION_NODE) {
            _CStr val(child->getNodeValue());
            result += val.str();
        }
    }
    //- trim leading and trailing whitespace
    size_t start = result.find_first_not_of(" \t\r\n");
    if (start == string::npos) { return string(); }
    size_t end = result.find_last_not_of(" \t\r\n");
    return result.substr(start, end - start + 1);
}

//- Return all child text content recursively (for Payload ANY elements).
static string _getAllText(DOMNode* node)
{
    string result;
    DOMNodeList* children = node->getChildNodes();
    for (XMLSize_t i = 0; i < children->getLength(); ++i) {
        DOMNode* child = children->item(i);
        DOMNode::NodeType t = child->getNodeType();
        if (t == DOMNode::TEXT_NODE || t == DOMNode::CDATA_SECTION_NODE) {
            _CStr val(child->getNodeValue());
            result += val.str();
        } else if (t == DOMNode::ELEMENT_NODE) {
            result += _getAllText(child);
        }
    }
    return result;
}

//- Parse <Header> element child nodes into the NLAPHeader_t map.
static void _parseHeader(DOMElement* headerElem, NLAPHeader_t& headerMap)
{
    DOMNodeList* children = headerElem->getChildNodes();
    for (XMLSize_t i = 0; i < children->getLength(); ++i) {
        DOMNode* child = children->item(i);
        if (child->getNodeType() != DOMNode::ELEMENT_NODE) { continue; }

        DOMElement* field = static_cast<DOMElement*>(child);
        _CStr name(field->getTagName());
        string value = _getElementText(field);

        if (!value.empty()) {
            headerMap.emplace(name.str(), value);
        }
    }
}

//- Parse <Security> element into Encryption / Signature fields.
static void _parseSecurity(DOMElement* secElem, RequestProperties_t& props)
{
    DOMNodeList* children = secElem->getChildNodes();
    for (XMLSize_t i = 0; i < children->getLength(); ++i) {
        DOMNode* child = children->item(i);
        if (child->getNodeType() != DOMNode::ELEMENT_NODE) { continue; }

        DOMElement* field = static_cast<DOMElement*>(child);
        _CStr name(field->getTagName());
        string value = _getElementText(field);
        string nameStr = name.str();

        if (nameStr == "Encryption")  { props.Encryption = value; }
        else if (nameStr == "Signature") { props.Signature  = value; }
    }
}

//- Parse <Status> element into StatusCode / StatusDescription / StatusException.
static void _parseStatus(DOMElement* statusElem, RequestProperties_t& props)
{
    DOMNodeList* children = statusElem->getChildNodes();
    for (XMLSize_t i = 0; i < children->getLength(); ++i) {
        DOMNode* child = children->item(i);
        if (child->getNodeType() != DOMNode::ELEMENT_NODE) { continue; }

        DOMElement* field = static_cast<DOMElement*>(child);
        _CStr name(field->getTagName());
        string value = _getElementText(field);
        string nameStr = name.str();

        if      (nameStr == "Code")        { props.StatusCode        = value; }
        else if (nameStr == "Description") { props.StatusDescription = value; }
        else if (nameStr == "Exception")   { props.StatusException   = value; }
    }
}


// ---------------------------------------------------------------------------
//  Build a parseable XML string: ensure <?xml declaration is present and
//  inject a DOCTYPE declaration so Xerces can locate the DTD for validation.
// ---------------------------------------------------------------------------

static string _buildParseableXML(const string& xmlMsg)
{
    string result = xmlMsg;

    const string declTag  = "<?xml";
    const string declEnd  = "?>";
    const string doctype  = "<!DOCTYPE NLAP SYSTEM \"" + NLAP_DTD_SYSTEM_PATH + "\">";

    size_t declStartPos = result.find(declTag);

    if (declStartPos == string::npos) {
        //- no XML declaration at all: prepend both
        result = NLAP_XML_DECLARATION + doctype + result;
    } else {
        //- XML declaration is present: inject DOCTYPE right after it
        size_t declEndPos = result.find(declEnd, declStartPos);
        if (declEndPos != string::npos) {
            result.insert(declEndPos + declEnd.length(), doctype);
        }
    }

    return result;
}


// ---------------------------------------------------------------------------
//  StringHelper (shared split utility – mirrors httpparser.hpp)
// ---------------------------------------------------------------------------

class StringHelper {
public:
    static void split(string& StringRef, const string& Delimiter, vector<string>& ResultRef)
    {
        string SplitElement;
        auto pos = StringRef.find(Delimiter);

        while (pos != string::npos) {
            SplitElement = StringRef.substr(0, pos);
            ResultRef.push_back(SplitElement);
            StringRef.erase(0, pos + Delimiter.length());
            pos = StringRef.find(Delimiter);
        }
    }
};


// ---------------------------------------------------------------------------
//  XMLParser implementation
// ---------------------------------------------------------------------------

XMLParser::XMLParser(const uint16_t BufferSize) :
    _RequestParseError(0),
    _ReqAddIndex(0),
    _ReqNextIndex(0),
    _XMLRequestBuffer(""),
    _grammarPool(nullptr)
{
    _XMLRequestBuffer.reserve(BufferSize);
    _XMLRequestBufferMax = BufferSize;

    if (_xercesRefCount.fetch_add(1) == 0) {
        XMLPlatformUtils::Initialize();
    }

    //- pre-load the NLAP DTD into a locked grammar pool so that individual
    //- parse calls can use the cached grammar without loading external files
    auto* pool = new XMLGrammarPoolImpl(XMLPlatformUtils::fgMemoryManager);
    _grammarPool = pool;

    try {
        XercesDOMParser dtdLoader(nullptr, XMLPlatformUtils::fgMemoryManager, pool);
        dtdLoader.loadGrammar(NLAP_DTD_SYSTEM_PATH.c_str(), Grammar::DTDGrammarType, true);
    }
    catch (...) {
        //- DTD loading failed (file not found or malformed DTD).
        //- The pool remains empty; validation will fail for every incoming message,
        //- which is the correct safe behaviour: no DTD → no accepted messages.
    }

    pool->lockPool();
}

XMLParser::~XMLParser()
{
    delete static_cast<XMLGrammarPoolImpl*>(_grammarPool);
    _grammarPool = nullptr;

    if (_xercesRefCount.fetch_sub(1) == 1) {
        XMLPlatformUtils::Terminate();
    }
}

void XMLParser::appendBuffer(const char* BufferRef, const uint16_t RecvBytes)
{
    if (_XMLRequestBuffer.length() + RecvBytes > _XMLRequestBufferMax) {
        _RequestParseError = XML_ERROR_PARSE_BUFFER_EXCEEDED;
        return;
    }

    _XMLRequestBuffer.append(BufferRef, RecvBytes);

    //- reset split vector and process only when at least one complete message arrived
    const size_t EndMarkerFound = _XMLRequestBuffer.find(NLAP_XML_END_MARKER);

    if (EndMarkerFound != string::npos) {
        _SplittedRequests.clear();
        _processRequests();
    }
}

RequestsMap_t XMLParser::getRequests()
{
    return _Requests;
}

RequestPropertiesPtr_t XMLParser::getNextRequest()
{
    auto it = _Requests.find(_ReqNextIndex);
    if (it != _Requests.end()) {
        _ReqNextIndex += 1;
        return make_shared<RequestProperties_t>(it->second);
    }
    return nullptr;
}

void XMLParser::removeRequest(uint16_t Index)
{
    if (_Requests.find(Index) != _Requests.end()) {
        _Requests.erase(Index);
    }
}


inline void XMLParser::_processRequests()
{
    //- split stream buffer into individual NLAP messages on the end marker
    StringHelper::split(_XMLRequestBuffer, NLAP_XML_END_MARKER, _SplittedRequests);

    //- iterate over split messages
    for (size_t i = 0; i < _SplittedRequests.size(); ++i) {
        if (_processRequestProperties(i) == false) {
            _RequestParseError = XML_ERROR_BAD_REQUEST;
        }
    }
}

inline bool XMLParser::_processRequestProperties(const size_t Index)
{
    //- get the raw split piece at this index
    auto& Request = _SplittedRequests.at(Index);

    //- skip empty pieces (e.g. trailing split result)
    if (Request.empty()) { return false; }

    //- reassemble a complete, valid XML message:
    //-   1. append </NLAP> (was consumed as the delimiter during split)
    //-   2. prepend <?xml declaration if not already present
    string XMLMessage = Request;
    XMLMessage += NLAP_XML_END_MARKER;

    if (XMLMessage.find("<?xml") == string::npos) {
        XMLMessage = NLAP_XML_DECLARATION + XMLMessage;
    }

    //- reset properties
    _RequestProperties = RequestProperties_t{};
    _RequestProperties.XMLRawMessage = XMLMessage;

    //- parse XML and populate remaining fields
    if (_parseXML(XMLMessage, _RequestProperties) == false) { return false; }

    //- add to requests map
    _Requests.emplace(_ReqAddIndex, _RequestProperties);
    _ReqAddIndex += 1;

    return true;
}

inline bool XMLParser::_parseXML(const string& XMLMessage, RequestProperties_t& Props)
{
    //- build a version of the XML that includes a DOCTYPE so Xerces can
    //- identify the grammar to use from the pre-loaded grammar pool
    string parseableXML = _buildParseableXML(XMLMessage);

    auto* pool = static_cast<XMLGrammarPool*>(_grammarPool);
    XercesDOMParser parser(nullptr, XMLPlatformUtils::fgMemoryManager, pool);
    _NLAPErrorHandler errHandler;

    parser.setErrorHandler(&errHandler);
    parser.setValidationScheme(XercesDOMParser::Val_Always);
    parser.setDoNamespaces(false);
    parser.setDoSchema(false);

    //- use the pre-loaded DTD grammar; disable loading of any external
    //- entities from user-supplied XML to prevent XXE attacks
    parser.setLoadExternalDTD(false);
    parser.useCachedGrammarInParse(true);
    parser.setCreateEntityReferenceNodes(false);
    parser.setValidationConstraintFatal(true);

    //- block all external entity resolution at the SAX layer (XXE defence)
    _BlockingEntityResolver blockingResolver;
    parser.setEntityResolver(&blockingResolver);

    try {
        MemBufInputSource xmlInput(
            reinterpret_cast<const XMLByte*>(parseableXML.c_str()),
            static_cast<XMLSize_t>(parseableXML.size()),
            "nlap-xml-input"
        );
        parser.parse(xmlInput);
    }
    catch (const XMLException&)      { return false; }
    catch (const DOMException&)      { return false; }
    catch (...)                      { return false; }

    if (errHandler.hasError()) { return false; }

    DOMDocument* doc = parser.getDocument();
    if (!doc) { return false; }

    //- root element: <NLAP>
    DOMElement* root = doc->getDocumentElement();
    if (!root) { return false; }

    //- first element child of <NLAP> is either <Request> or <Response>
    DOMNodeList* rootChildren = root->getChildNodes();
    DOMElement*  reqresElem   = nullptr;

    for (XMLSize_t i = 0; i < rootChildren->getLength(); ++i) {
        DOMNode* node = rootChildren->item(i);
        if (node->getNodeType() == DOMNode::ELEMENT_NODE) {
            reqresElem = static_cast<DOMElement*>(node);
            break;
        }
    }

    if (!reqresElem) { return false; }

    {
        _CStr tagName(reqresElem->getTagName());
        Props.RequestType = tagName.str();
    }

    //- only accept <Request> or <Response>
    if (Props.RequestType != "Request" && Props.RequestType != "Response") {
        return false;
    }

    //- iterate over child elements of <Request> / <Response>
    DOMNodeList* children = reqresElem->getChildNodes();
    for (XMLSize_t i = 0; i < children->getLength(); ++i) {
        DOMNode* child = children->item(i);
        if (child->getNodeType() != DOMNode::ELEMENT_NODE) { continue; }

        DOMElement* elem = static_cast<DOMElement*>(child);
        _CStr nameCS(elem->getTagName());
        string name = nameCS.str();

        if      (name == "UUID")     { Props.UUID     = _getElementText(elem); }
        else if (name == "Protocol") { Props.Protocol = _getElementText(elem); }
        else if (name == "Version")  { Props.Version  = _getElementText(elem); }
        else if (name == "Subtype")  { Props.Subtype  = _getElementText(elem); }
        else if (name == "Header")   { _parseHeader(elem, Props.Header);       }
        else if (name == "Security") { _parseSecurity(elem, Props);            }
        else if (name == "Payload")  { Props.Payload  = _getAllText(child);    }
        else if (name == "Status")   { _parseStatus(elem, Props);             }
    }

    return true;
}
