#pragma once

#include <string>

class Xor
{
public:
    Xor() = delete;
    static std::string encryptDecrypt(const std::string &text, const std::string &key);
};
