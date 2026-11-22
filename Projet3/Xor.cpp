#include "Xor.h"

#include <cassert>

std::string Xor::encryptDecrypt(const std::string &text, const std::string &key)
{
    assert(!key.empty());
    std::string result = text;
    for (std::size_t i = 0; i < text.size(); ++i)
    {
        result[i] = static_cast<char>(text[i] ^ key[i % key.size()]);
    }
    return result;
}