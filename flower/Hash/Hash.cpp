#include "Hash.h"
#include <openssl/evp.h>
#include <iomanip>
#include <sstream>

static std::string digest(const std::string &data, const EVP_MD *md) {
    unsigned char result[EVP_MAX_MD_SIZE];
    unsigned int len = 0;
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, md, NULL);
    EVP_DigestUpdate(ctx, data.data(), data.size());
    EVP_DigestFinal_ex(ctx, result, &len);
    EVP_MD_CTX_free(ctx);
    std::stringstream ss;
    for (unsigned int i = 0; i < len; i++)
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)result[i];
    return ss.str();
}

std::string Hash::md5(const std::string &data)    { return digest(data, EVP_md5()); }
std::string Hash::sha1(const std::string &data)   { return digest(data, EVP_sha1()); }
std::string Hash::sha256(const std::string &data) { return digest(data, EVP_sha256()); }
