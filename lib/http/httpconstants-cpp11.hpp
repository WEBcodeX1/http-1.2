#pragma once

//- Legacy C++11 compatible constants header.
//- Functionally identical to httpconstants.hpp; usable with -std=c++11 or later.

#include <string>
#include <vector>
#include <stdint.h>

//- constant expressions
constexpr uint16_t HTTP_VERSION_UNKNOWN = 0;
constexpr uint16_t HTTP_VERSION_1_1 = 1;

constexpr uint16_t HTTP_METHOD_OTHER = 0;
constexpr uint16_t HTTP_METHOD_GET = 1;
constexpr uint16_t HTTP_METHOD_POST = 2;

constexpr uint16_t HTTP_POST_MAX_CONTENT_LENGTH = 2048;

constexpr uint16_t URL_PARAM_NOT_FOUND_ERROR = 10;

//- constant expressions (error)
constexpr uint16_t HTTP_ERROR_PARSE_BUFFER_EXCEEDED = 10; //- currently not implemented
constexpr uint16_t HTTP_ERROR_BAD_REQUEST = 400;

//- constant strings
static const std::string HTTP_1_1_END_MARKER("\r\n\r\n");
static const std::string HTTP_HEADER_CONTENT_LENGTH("Content-Length");

//- constant multidimensional
static const std::vector<std::string> HTTPHeaderAllowed //- currently not implemented
{
    "Host",
    "Transfer-Encoding",
    "If-None-Match",
    "Content-Type",
    HTTP_HEADER_CONTENT_LENGTH
};
