#include "flower/codec/RequestCodec.h"
#include "flower/codec/RespondCodec.h"
#include <iostream>

using namespace std;

static void printSep(const char *title) {
  cout << "\n========== " << title << " ==========\n";
}

int main() {
  // ============================
  // 1. 客户端：构造请求并编码
  // ============================
  printSep("客户端发送请求");

  RequestInfo reqInfo;
  reqInfo.cmd = 1001;
  reqInfo.clientID = "client_A";
  reqInfo.serverID = "server_1";
  reqInfo.sign = "abc123";
  reqInfo.data = "你好，服务器";

  RequestCodec reqCodec(&reqInfo);
  string encodedReq = reqCodec.encodeMsg();
  cout << "编码后的请求 (" << encodedReq.size() << " 字节)" << endl;

  // ============================
  // 2. 服务端：接收请求并解码
  // ============================
  printSep("服务端收到请求并解码");

  RequestCodec reqDecoder(encodedReq);
  RequestInfo *decodedReq = (RequestInfo *)reqDecoder.decodeMsg();

  cout << "cmd     : " << decodedReq->cmd << endl;
  cout << "clientID: " << decodedReq->clientID << endl;
  cout << "serverID: " << decodedReq->serverID << endl;
  cout << "sign    : " << decodedReq->sign << endl;
  cout << "data    : " << decodedReq->data << endl;

  delete decodedReq;

  // ============================
  // 3. 服务端：构造响应并编码
  // ============================
  printSep("服务端返回响应");

  RespondInfo respInfo;
  respInfo.status = true;
  respInfo.seckeyID = 888;
  respInfo.clientID = "client_A";
  respInfo.serverID = "server_1";
  respInfo.data = "收到，这是响应数据";

  RespondCodec respCodec(&respInfo);
  string encodedResp = respCodec.encodeMsg();
  cout << "编码后的响应 (" << encodedResp.size() << " 字节)" << endl;

  // ============================
  // 4. 客户端：接收响应并解码
  // ============================
  printSep("客户端收到响应并解码");

  RespondCodec respDecoder(encodedResp);
  RespondInfo *decodedResp = (RespondInfo *)respDecoder.decodeMsg();

  cout << "status  : " << (decodedResp->status ? "成功" : "失败") << endl;
  cout << "seckeyID: " << decodedResp->seckeyID << endl;
  cout << "clientID: " << decodedResp->clientID << endl;
  cout << "serverID: " << decodedResp->serverID << endl;
  cout << "data    : " << decodedResp->data << endl;

  delete decodedResp;

  printSep("通信模拟结束");
  return 0;
}
