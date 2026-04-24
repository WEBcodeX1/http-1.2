#pragma once

#include <string>
#include <stdint.h>

constexpr uint16_t HTTP_VERSION_UNKNOWN = 0;
constexpr uint16_t HTTP_VERSION_1_1 = 1;

constexpr uint16_t HTTP_METHOD_OTHER = 0;
constexpr uint16_t HTTP_METHOD_GET = 1;
constexpr uint16_t HTTP_METHOD_POST = 2;

constexpr uint16_t HTTP_POST_MAX_CONTENT_LENGTH = 4096;

constexpr uint16_t URL_PARAM_NOT_FOUND = 10;
constexpr uint16_t URL_PARAM_PARSE_ERROR = 20;

static const std::string HTTP_1_1_END_MARKER = "\r\n\r\n";
static const std::string HTTP_HEADER_CONTENT_LENGTH = "Content-Length";
