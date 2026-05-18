#include "RequestCodec.h"

// 默认构造函数
RequestCodec::RequestCodec() {}

// 用已编码字符串构造，保存字符串供 decodeMsg() 解析
RequestCodec::RequestCodec(string encstr) : m_encStr(encstr) {}

// 用原始请求信息构造，调用 initMessage 填充 protobuf 消息
RequestCodec::RequestCodec(RequestInfo *info) { initMessage(info); }

// 将 RequestInfo 的字段逐一对映到 protobuf RequestMsg
void RequestCodec::initMessage(RequestInfo *info) {
  m_msg.set_cmdtype(info->cmd);
  m_msg.set_clientid(info->clientID);
  m_msg.set_serverid(info->serverID);
  m_msg.set_sign(info->sign);
  m_msg.set_data(info->data);
}

// 将 protobuf 消息序列化为字符串返回
string RequestCodec::encodeMsg() {
  string ret;
  m_msg.SerializeToString(&ret);
  return ret;
}

// 解析已编码字符串，构造并返回 RequestInfo（调用者负责 delete）
void *RequestCodec::decodeMsg() {
  m_msg.ParseFromString(m_encStr);
  RequestInfo *info = new RequestInfo;
  info->cmd = m_msg.cmdtype();
  info->clientID = m_msg.clientid();
  info->serverID = m_msg.serverid();
  info->sign = m_msg.sign();
  info->data = m_msg.data();
  return info;
}

// 析构函数
RequestCodec::~RequestCodec() {}
