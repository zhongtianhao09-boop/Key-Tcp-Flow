#pragma once
#include <openssl/aes.h>
#include <string>

class AesCrypto {
public:
  AesCrypto(std::string key);
  ~AesCrypto();

  std::string aesCBCEncrypto(std::string text);
  std::string aesCBCDecrypt(std::string enStr);

private:
  std::string aesCrypto(std::string data, int enc);
  void generateIvec(unsigned char *ivec);

  AES_KEY m_encKey;
  AES_KEY m_decKey;
  bool m_ready = false;
};
