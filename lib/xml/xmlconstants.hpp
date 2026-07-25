#pragma once

#include <string>
#include <string_view>
#include <cstdint>

//- NLAP XML start marker: configurable tag used to identify message boundaries
static const std::string NLAP_XML_START_MARKER("<nlap>");

//- NLAP XML end marker: used to split stream data into individual NLAP messages
static const std::string NLAP_XML_END_MARKER("</nlap>");

//- XML declaration prepended to messages that do not already contain it
static const std::string NLAP_XML_DECLARATION("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");

//- string_view variants for zero-copy operations
static constexpr std::string_view NLAP_XML_START_MARKER_SV("<nlap>");
static constexpr std::string_view NLAP_XML_END_MARKER_SV("</nlap>");
static constexpr std::string_view NLAP_XML_DECLARATION_SV("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");

//- Path to the NLAP DTD used for validation (override at compile time via -DNLAP_DTD_PATH=...)
#ifndef NLAP_DTD_PATH
#define NLAP_DTD_PATH "specs/xml/nlap.dtd"
#endif

static const std::string NLAP_DTD_SYSTEM_PATH(NLAP_DTD_PATH);

//- constant expressions (error)
constexpr uint16_t XML_ERROR_PARSE_BUFFER_EXCEEDED = 10;
constexpr uint16_t XML_ERROR_BAD_REQUEST           = 400;
constexpr uint16_t XML_ERROR_INVALID_FRAMING       = 401;
