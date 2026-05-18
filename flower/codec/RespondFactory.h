#ifndef RESPONDFACTORY_H
#define RESPONDFACTORY_H

#include "CodecFactory.h"
#include "RespondCodec.h"
#include <iostream>
using namespace std;

class RespondFactory : public CodecFactory {
public:
  RespondFactory(RespondInfo *info);
  ~RespondFactory();

  Codec *createCodec() override;

  // 创建响应信息
  static RespondInfo *createRespondInfo(bool status, int seckeyID,
                                        string clientID, string serverID,
                                        string data);

  // 基于响应信息创建编码器
  static RespondCodec *createCodec(RespondInfo *info);

  // 从编码字符串创建解码器
  static RespondCodec *createCodec(string encstr);

private:
  RespondInfo *m_info;
};

#endif
