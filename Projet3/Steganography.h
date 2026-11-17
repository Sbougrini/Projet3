#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "BmpImage.h"

constexpr char STEG_SIGNATURE[] = "STEG";
constexpr std::size_t SIGNATURE_SIZE = 4;
constexpr std::size_t LENGTH_SIZE = 4;
constexpr std::size_t BITS_PER_BYTE = 8;
constexpr std::size_t METADATA_SIZE = 9;

bool hideMessage(BmpImage &image, const std::string &message, bool encrypted,
                 std::string &errorMessage);

bool extractMessage(const std::vector<unsigned char> &imageData, std::string &message,
                    bool &isEncrypted, std::string &error);
