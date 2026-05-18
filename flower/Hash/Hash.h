#pragma once
#include <string>

class Hash {
public:
    static std::string md5(const std::string &data);
    static std::string sha1(const std::string &data);
    static std::string sha256(const std::string &data);
};
