#ifndef REQUESTFACTORY_H
#define REQUESTFACTORY_H

#include "CodecFactory.h"
#include "RequestCodec.h"
#include <iostream>
using namespace std;

class RequestFactory : public CodecFactory {
public:
  RequestFactory(RequestInfo *info);
  ~RequestFactory();

  Codec *createCodec() override;

  // 创建请求信息
  static RequestInfo *createRequestInfo(int cmd, string clientID,
                                        string serverID, string sign,
                                        string data);

  // 基于请求信息创建编码器
  static RequestCodec *createCodec(RequestInfo *info);

  // 从编码字符串创建解码器
  static RequestCodec *createCodec(string encstr);

private:
  RequestInfo *m_info;
};

#endif
