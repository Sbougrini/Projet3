#pragma once

#include <cstddef>
#include <string>
#include <vector>

constexpr std::size_t METADATA_SIZE = 9;
const std::string SIGNATURE = "STEG";

bool extractMessage(const std::vector<unsigned char> &imageData, std::string &message,
                    bool &isEncrypted, std::string &error);