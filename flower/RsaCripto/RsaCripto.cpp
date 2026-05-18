#include "RsaCripto.h"
#include <iostream>
#include <string>
#include <openssl/err.h>
Cryptographic::Cryptographic() {
  m_publicKey = RSA_new();
  m_privateKey = RSA_new();
}
Cryptographic::Cryptographic(string fileName, bool isPrivate) {
  m_publicKey = RSA_new();
  m_privateKey = RSA_new();
  if (isPrivate) {
    initPrivateKey(fileName);
  } else {
    initPublicKey(fileName);
  }
};
void Cryptographic::generateRsakey(int bits, string pub, string pri) {
  BIGNUM *e = BN_new();
  BN_set_word(e, RSA_F4);
  RSA *r = RSA_generate_key(bits, BN_get_word(e), NULL, NULL);

  BIO *pubIO = BIO_new_file(pub.data(), "w");
  PEM_write_bio_RSAPublicKey(pubIO, r);
  BIO_flush(pubIO);
  BIO_free(pubIO);

  BIO *priBio = BIO_new_file(pri.data(), "w");
  PEM_write_bio_RSAPrivateKey(priBio, r, NULL, NULL, 0, NULL, NULL);
  BIO_flush(priBio);
  BIO_free(priBio);

  m_privateKey = RSAPrivateKey_dup(r);
  m_publicKey = RSAPublicKey_dup(r);
  BN_free(e);
  RSA_free(r);
};
bool Cryptographic::initPublicKey(string pubfile) {
  BIO *pubBio = BIO_new_file(pubfile.data(), "r");
  if (PEM_read_bio_RSAPublicKey(pubBio, &m_publicKey, NULL, NULL) == NULL) {
    ERR_print_errors_fp(stdout);
    BIO_free(pubBio);
    return false;
  }
  BIO_free(pubBio);
  return true;
};
bool Cryptographic::initPrivateKey(string prifile) {
  BIO *priBio = BIO_new_file(prifile.data(), "r");
  if (PEM_read_bio_RSAPrivateKey(priBio, &m_privateKey, NULL, NULL) == NULL) {
    ERR_print_errors_fp(stdout);
    BIO_free(priBio);
    return false;
  }
  BIO_free(priBio);
  return true;
}

Cryptographic::~Cryptographic() {
  if (m_publicKey) RSA_free(m_publicKey);
  if (m_privateKey) RSA_free(m_privateKey);
}

string Cryptographic::rsaPubKeyEncrypt(string data) {
  int len = RSA_size(m_publicKey);
  unsigned char *out = new unsigned char[len];
  int ret = RSA_public_encrypt(data.size(), (unsigned char *)data.data(),
                               out, m_publicKey, RSA_PKCS1_PADDING);
  if (ret < 0) {
    delete[] out;
    return "";
  }
  string result((char *)out, ret);
  delete[] out;
  return result;
}

string Cryptographic::rsaPriKeyDecrypt(string encData) {
  int len = RSA_size(m_privateKey);
  unsigned char *out = new unsigned char[len];
  int ret = RSA_private_decrypt(encData.size(), (unsigned char *)encData.data(),
                                out, m_privateKey, RSA_PKCS1_PADDING);
  if (ret < 0) {
    delete[] out;
    return "";
  }
  string result((char *)out, ret);
  delete[] out;
  return result;
}

string Cryptographic::rsaSign(string data, SignLevel level) {
  unsigned int len;
  unsigned char *out = new unsigned char[RSA_size(m_privateKey)];
  unsigned char digest[SHA512_DIGEST_LENGTH];
  unsigned int digestLen;
  const EVP_MD *md = EVP_get_digestbynid(level);
  EVP_Digest(data.data(), data.size(), digest, &digestLen, md, NULL);
  int ret = RSA_sign(level, digest, digestLen, out, &len, m_privateKey);
  if (ret != 1) {
    delete[] out;
    return "";
  }
  string result((char *)out, len);
  delete[] out;
  return result;
}

bool Cryptographic::rsaVerify(string data, string signData, SignLevel level) {
  unsigned char digest[SHA512_DIGEST_LENGTH];
  unsigned int digestLen;
  const EVP_MD *md = EVP_get_digestbynid(level);
  EVP_Digest(data.data(), data.size(), digest, &digestLen, md, NULL);
  int ret = RSA_verify(level, digest, digestLen,
                       (unsigned char *)signData.data(), signData.size(),
                       m_publicKey);
  return ret == 1;
}