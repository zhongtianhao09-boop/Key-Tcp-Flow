// 命令行密钥协商测试 — 直接复现客户端 GUI 的 onAgree() 流程
#include "../flower/RsaCripto/RsaCripto.h"
#include "../flower/codec/RequestCodec.h"
#include "../flower/codec/RespondCodec.h"
#include "../flower/codec/RequestFactory.h"
#include "../flower/codec/RespondFactory.h"
#include "../flower/tcp/TcpSocket.h"
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

int main() {
    cout << "=== 密钥协商测试 ===" << endl;

    // 1. 生成 RSA 密钥对
    cout << "[1] 生成 RSA 密钥对..." << endl;
    Cryptographic rsa;
    rsa.generateRsakey(1024);
    cout << "    OK" << endl;

    // 2. 读取公钥
    ifstream ifs("public.pem");
    stringstream str;
    str << ifs.rdbuf();
    string pubKey = str.str();
    cout << "[2] 公钥长度: " << pubKey.size() << " bytes" << endl;

    // 3. 构建请求
    cout << "[3] 构建请求..." << endl;
    RequestInfo reqInfo;
    reqInfo.clientID = "robin";
    reqInfo.serverID = "Luffy";
    reqInfo.cmd = 1;
    reqInfo.data = pubKey;
    reqInfo.sign = rsa.rsaSign(pubKey);
    cout << "    签名长度: " << reqInfo.sign.size() << " bytes" << endl;

    // 4. 验证签名 (本地测试)
    cout << "[4] 本地验签测试..." << endl;
    Cryptographic verifyRsa("public.pem", false);
    bool verifyOk = verifyRsa.rsaVerify(pubKey, reqInfo.sign);
    cout << "    " << (verifyOk ? "PASS" : "FAIL") << endl;
    if (!verifyOk) {
        cout << "签名自验失败，终止" << endl;
        return 1;
    }

    // 5. 编码
    CodecFactory *factory = new RequestFactory(&reqInfo);
    Codec *c = factory->createCodec();
    string encstr = c->encodeMsg();
    delete factory;
    delete c;
    cout << "[5] 编码完成: " << encstr.size() << " bytes" << endl;

    // 6. 连接服务器
    cout << "[6] 连接 127.0.0.1:9898..." << endl;
    TcpSocket tcp;
    int ret = tcp.connectToHost("127.0.0.1", 9898);
    if (ret != 0) {
        perror("连接失败");
        return 1;
    }
    cout << "    连接成功" << endl;

    // 7. 发送
    tcp.sendMsg(encstr);
    cout << "[7] 已发送请求" << endl;

    // 8. 接收
    string msg = tcp.recvMsg();
    cout << "[8] 收到响应: " << msg.size() << " bytes" << endl;

    // 9. 解码响应
    c = RespondFactory::createCodec(msg);
    RespondInfo *resData = (RespondInfo *)c->decodeMsg();
    cout << "[9] status=" << resData->status
         << " clientID=" << resData->clientID
         << " serverID=" << resData->serverID
         << " dataLen=" << resData->data.size() << endl;

    if (!resData->status) {
        cout << "FAIL: 服务器拒绝" << endl;
        delete c;
        return 1;
    }

    // 10. 解密密钥
    string key = rsa.rsaPriKeyDecrypt(resData->data);
    cout << "[10] 协商密钥: " << key << endl;

    delete c;
    tcp.disconnect();
    cout << "=== 协商成功 ===" << endl;
    return 0;
}
