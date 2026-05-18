#include "interface.h"
#include "../Client/json.hpp"
#include "../ShareMemory/SecKeyShm.h"
#include "../SecKeyNodeInfo/SeckeyNodeInfo.h"
#include <fstream>

using json = nlohmann::json;

Interface::Interface(std::string key) {
  m_aes = std::make_unique<AesCrypto>(std::move(key));
}

Interface Interface::fromJsonFile(std::string jsonFile) {
  std::ifstream ifs(jsonFile);
  if (!ifs.is_open()) return Interface(std::string());

  json root = json::parse(ifs);
  std::string sid = root.value("ServerID", "");
  std::string cid = root.value("ClientID", "");
  std::string shmPath = root.value("ShmKey", "");
  int maxNode = root.value("ShmMaxNode", 4);

  SecKeyShm shm(shmPath, maxNode);
  NodeSecKeyInfo node = shm.shmRead(cid, sid);
  return Interface(std::string(node.seckey));
}

Interface::~Interface() = default;

std::string Interface::encryptData(const std::string& data) {
  if (!m_aes) return "";
  return m_aes->aesCBCEncrypto(data);
}

std::string Interface::decryptData(const std::string& data) {
  if (!m_aes) return "";
  return m_aes->aesCBCDecrypt(data);
}
