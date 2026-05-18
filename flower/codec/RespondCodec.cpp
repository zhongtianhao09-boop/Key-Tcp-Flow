#include "RespondCodec.h"

// 默认构造函数
RespondCodec::RespondCodec() {}

// 用已编码字符串构造，保存字符串供 decodeMsg() 解析
RespondCodec::RespondCodec(string encstr) : m_encStr(encstr) {}

// 用原始响应信息构造，调用 initMessage 填充 protobuf 消息
RespondCodec::RespondCodec(RespondInfo *info) {
  initMessage(info);
}

// 将 RespondInfo 的字段逐一对映到 protobuf RespondMsg
void RespondCodec::initMessage(RespondInfo *info) {
  m_msg.set_status(info->status);
  m_msg.set_seckeyid(info->seckeyID);
  m_msg.set_clientid(info->clientID);
  m_msg.set_serverid(info->serverID);
  m_msg.set_data(info->data);
}

// 将 protobuf 消息序列化为字符串返回
string RespondCodec::encodeMsg() {
  string ret;
  m_msg.SerializeToString(&ret);
  return ret;
}

// 解析已编码字符串，构造并返回 RespondInfo（调用者负责 delete）
void *RespondCodec::decodeMsg() {
  m_msg.ParseFromString(m_encStr);
  RespondInfo *info = new RespondInfo;
  info->status = m_msg.status();
  info->seckeyID = m_msg.seckeyid();
  info->clientID = m_msg.clientid();
  info->serverID = m_msg.serverid();
  info->data = m_msg.data();
  return info;
}

// 析构函数
RespondCodec::~RespondCodec() {}
