#ifndef RESPONDCODEC_H
#define RESPONDCODEC_H

#include "Codec.h"
#include "message.pb.h"
#include <iostream>
using namespace std;

// 响应信息的原始数据结构，用于构造 RespondCodec 时传入
struct RespondInfo {
  bool status;        // 状态
  int seckeyID;       // 密钥 ID
  string clientID;    // 客户端 ID
  string serverID;    // 服务器 ID
  string data;        // 响应数据
};

// 响应消息编解码器，继承自 Codec 基类
class RespondCodec : public Codec {
public:
  RespondCodec();                      // 默认构造函数
  RespondCodec(string encstr);         // 用已编码的字符串构造，用于解码
  RespondCodec(RespondInfo *info);     // 用原始响应信息构造，用于编码
  string encodeMsg();                  // 将响应信息编码为字符串
  void *decodeMsg();                   // 将编码字符串解码为响应信息
  ~RespondCodec();                     // 析构函数

private:
  void initMessage(RespondInfo *info); // 从 RespondInfo 填充 protobuf 消息
  RespondMsg m_msg;    // protobuf 响应消息对象
  string m_encStr;     // 已编码的字符串（解码时使用）
};

#endif
