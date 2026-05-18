#pragma once
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <string>

using namespace std;

enum SignLevel {
  Leve11 = NID_md5,
  Leve12 = NID_sha1,
  Leve13 = NID_sha224,
  Leve14 = NID_sha256,
  Leve15 = NID_sha384,
  Leve16 = NID_sha512
};
class Cryptographic {
public:
  Cryptographic();
  Cryptographic(string filename, bool isPrivate = true);
  ~Cryptographic();
  // 生成密钥对
  void generateRsakey(int bits, string pub = "public.pem",
                      string pri = "private.pem");
  // 公钥加密
  string rsaPubKeyEncrypt(string data);
  // 私钥解密
  string rsaPriKeyDecrypt(string encData);
  // 使用RSA签名
  string rsaSign(string gdata, SignLevel level = Leve13);
  // 使用RSA验证签名
  bool rsaVerify(string data, string signData, SignLevel level = Leve13);

private:
  // 得到公钥
  bool initPublicKey(string pubfile);
  // 得到私钥
  bool initPrivateKey(string prifile);

private:
  RSA *m_publicKey;
  RSA *m_privateKey;
};