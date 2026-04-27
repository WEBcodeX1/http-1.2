#pragma once

#include <string>
#include <vector>
#include <stdint.h>

//- constant expressions
constexpr uint16_t HTTP_VERSION_UNKNOWN = 0;
constexpr uint16_t HTTP_VERSION_1_1 = 1;

constexpr uint16_t HTTP_METHOD_OTHER = 0;
constexpr uint16_t HTTP_METHOD_GET = 1;
constexpr uint16_t HTTP_METHOD_POST = 2;

constexpr uint16_t HTTP_POST_MAX_CONTENT_LENGTH = 4096;

constexpr uint16_t URL_PARAM_NOT_FOUND_ERROR = 10;

//- constant expressions (error)
constexpr uint16_t HTTP_ERROR_PARSE_BUFFER_EXCEEDED = 10;
constexpr uint16_t HTTP_ERROR_BAD_REQUEST = 400;

//- contant strings
static const std::string HTTP_1_1_END_MARKER("\r\n\r\n");
static const std::string HTTP_HEADER_CONTENT_LENGTH("Content-Length");

//- contant multidimensional
static const std::vector<std::string> HTTPHeaderTypes
{
    "Host",
    "Transfer-Encoding",
    "If-None-Match",
    "Content-Type",
    HTTP_HEADER_CONTENT_LENGTH
};
