#pragma once

#include <cstdint>
#include <string_view>

//- NLAP XML start marker: configurable tag used to identify message boundaries
static constexpr std::string_view NLAP_XML_START_MARKER("<NLAP>");

//- NLAP XML end marker: used to split stream data into individual NLAP messages
static constexpr std::string_view NLAP_XML_END_MARKER("</NLAP>");

//- XML declaration prepended to messages that do not already contain it
static constexpr std::string_view NLAP_XML_DECLARATION("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");

//- Path to the NLAP DTD used for validation (override at compile time via -DNLAP_DTD_PATH=...)
#ifndef NLAP_DTD_PATH
#define NLAP_DTD_PATH "specs/xml/nlap.dtd"
#endif

static const std::string_view NLAP_DTD_SYSTEM_PATH(NLAP_DTD_PATH);

//- constant expressions (error)
constexpr uint16_t XML_ERROR_INVALID_CONTENT_DTD = 10;
constexpr uint16_t XML_ERROR_INVALID_SYNTAX = 20;
constexpr uint16_t XML_ERROR_INVALID_FRAMING = 30;
