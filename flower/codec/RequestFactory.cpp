#include "RequestFactory.h"

RequestFactory::RequestFactory(RequestInfo *info) : m_info(info) {}

RequestFactory::~RequestFactory() {}

Codec *RequestFactory::createCodec() {
  return new RequestCodec(m_info);
}

RequestInfo *RequestFactory::createRequestInfo(int cmd, string clientID,
                                               string serverID, string sign,
                                               string data) {
  RequestInfo *info = new RequestInfo;
  info->cmd = cmd;
  info->clientID = clientID;
  info->serverID = serverID;
  info->sign = sign;
  info->data = data;
  return info;
}

RequestCodec *RequestFactory::createCodec(RequestInfo *info) {
  return new RequestCodec(info);
}

RequestCodec *RequestFactory::createCodec(string encstr) {
  return new RequestCodec(encstr);
}
