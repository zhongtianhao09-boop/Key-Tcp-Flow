#pragma once
#include "AesCrypto.h"
#include <memory>
#include <string>

class Interface {
public:
  explicit Interface(std::string key);
  Interface(Interface&&) = default;
  Interface& operator=(Interface&&) = default;
  ~Interface();

  static Interface fromJsonFile(std::string jsonFile);

  std::string encryptData(const std::string& data);
  std::string decryptData(const std::string& data);

private:
  std::unique_ptr<AesCrypto> m_aes;
};
