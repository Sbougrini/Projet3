#pragma once

#include "BmpImage.h"

#include <cstdint>
#include <string>

const char STEG_SIGNATURE[] = "STEG";
const int SIGNATURE_SIZE = 4;
const int LENGTH_SIZE = 4;
const int BITS_PER_BYTE = 8;
const int METADATA_SIZE = 9;

bool hideMessage
(
 BmpImage &image,
 const std::string &message,
 bool encrypted,
 std::string &errorMessage
);
