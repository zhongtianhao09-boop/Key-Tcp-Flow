#include "AesCrypto.h"
#include <cstring>
#include <openssl/rand.h>
#include <vector>

AesCrypto::AesCrypto(std::string key) {
  if (key.size() == 16 || key.size() == 24 || key.size() == 32) {
    const unsigned char *aesKey = (const unsigned char *)key.data();
    int bits = key.size() * 8;
    AES_set_encrypt_key(aesKey, bits, &m_encKey);
    AES_set_decrypt_key(aesKey, bits, &m_decKey);
    m_ready = true;
  }
}

AesCrypto::~AesCrypto() {
  std::memset(&m_encKey, 0, sizeof(m_encKey));
  std::memset(&m_decKey, 0, sizeof(m_decKey));
}

void AesCrypto::generateIvec(unsigned char *ivec) {
  for (int i = 0; i < AES_BLOCK_SIZE; i++)
    ivec[i] = (unsigned char)(i + 1);
}

std::string AesCrypto::aesCrypto(std::string data, int enc) {
  if (!m_ready || data.empty()) return "";

  unsigned char ivec[AES_BLOCK_SIZE];
  generateIvec(ivec);

  if (enc == AES_ENCRYPT) {
    size_t padLen = AES_BLOCK_SIZE - (data.size() % AES_BLOCK_SIZE);
    size_t paddedSize = data.size() + padLen;
    std::vector<unsigned char> in(paddedSize);
    std::memcpy(in.data(), data.data(), data.size());
    std::memset(in.data() + data.size(), (int)padLen, padLen);

    std::vector<unsigned char> out(paddedSize);
    AES_cbc_encrypt(in.data(), out.data(), paddedSize, &m_encKey, ivec, AES_ENCRYPT);

    return std::string((char *)out.data(), paddedSize);
  } else {
    size_t len = data.size();
    std::vector<unsigned char> out(len);
    AES_cbc_encrypt((const unsigned char *)data.data(), out.data(), len,
                    &m_decKey, ivec, AES_DECRYPT);

    unsigned char padLen = out[len - 1];
    if (padLen == 0 || padLen > AES_BLOCK_SIZE) return "";

    for (unsigned char i = 1; i <= padLen; i++) {
      if (out[len - i] != padLen) return "";
    }
    return std::string((char *)out.data(), len - padLen);
  }
}

std::string AesCrypto::aesCBCEncrypto(std::string text) {
  return aesCrypto(text, AES_ENCRYPT);
}

std::string AesCrypto::aesCBCDecrypt(std::string enStr) {
  return aesCrypto(enStr, AES_DECRYPT);
}
