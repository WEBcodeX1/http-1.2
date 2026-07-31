#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

struct XMLNode
{
    std::unordered_map<std::string, XMLNode> Children;
    const char* Address = nullptr;
    std::size_t Length = 0;

    XMLNode& operator[](const std::string& Key)
    {
        return Children[Key];
    }

    const XMLNode& at(const std::string& Key) const
    {
        return Children.at(Key);
    }
};

using ResultTree_t = std::unordered_map<std::string, XMLNode>;

struct ParseResult_t
{
    std::vector<ResultTree_t> Results;
    std::uint16_t ErrorCode = 0;
};

class XMLParser
{
public:
    explicit XMLParser(std::size_t ParseBufferSize = 4096);
    ~XMLParser();

    void setParseBufferSize(std::size_t ParseBufferSize);
    std::size_t getParseBufferSize() const;

    ParseResult_t parse(char* InputBuffer) const;

private:
    std::size_t _ParseBufferSize;
    std::shared_ptr<void> _GrammarPool;
};
