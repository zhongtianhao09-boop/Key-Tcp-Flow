#include "RespondFactory.h"

RespondFactory::RespondFactory(RespondInfo *info) : m_info(info) {}

RespondFactory::~RespondFactory() {}

Codec *RespondFactory::createCodec() {
  return new RespondCodec(m_info);
}

RespondInfo *RespondFactory::createRespondInfo(bool status, int seckeyID,
                                               string clientID, string serverID,
                                               string data) {
  RespondInfo *info = new RespondInfo;
  info->status = status;
  info->seckeyID = seckeyID;
  info->clientID = clientID;
  info->serverID = serverID;
  info->data = data;
  return info;
}

RespondCodec *RespondFactory::createCodec(RespondInfo *info) {
  return new RespondCodec(info);
}

RespondCodec *RespondFactory::createCodec(string encstr) {
  return new RespondCodec(encstr);
}
