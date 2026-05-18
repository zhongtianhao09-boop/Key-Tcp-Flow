#ifndef REQUESTCODEC_H
#define REQUESTCODEC_H

#include "Codec.h"
#include "message.pb.h"
#include <iostream>
using namespace std;

// 请求信息的原始数据结构，用于构造 RequestCodec 时传入
struct RequestInfo {
  int cmd;          // 命令类型
  string clientID;  // 客户端 ID
  string serverID;  // 服务器 ID
  string sign;      // 签名
  string data;      // 请求数据
};

// 请求消息编解码器，继承自 Codec 基类
class RequestCodec : public Codec {
public:
  RequestCodec();                      // 默认构造函数
  RequestCodec(string encstr);         // 用已编码的字符串构造，用于解码
  RequestCodec(RequestInfo *info);     // 用原始请求信息构造，用于编码
  string encodeMsg();                  // 将请求信息编码为字符串
  void *decodeMsg();                   // 将编码字符串解码为请求信息
  ~RequestCodec();                     // 析构函数

private:
  void initMessage(RequestInfo *info);  // 从 RequestInfo 填充 protobuf 消息
  RequestMsg m_msg;    // protobuf 请求消息对象
  string m_encStr;     // 已编码的字符串（解码时使用）
};

#endif
