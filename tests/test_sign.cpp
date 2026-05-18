#include "../flower/RsaCripto/RsaCripto.h"
#include "../flower/codec/RequestCodec.h"
#include "../flower/codec/RespondCodec.h"
#include "../flower/codec/RequestFactory.h"
#include "../flower/codec/RespondFactory.h"
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

int main() {
    cout << "========== 1. RSA 签名/验签测试 ==========" << endl;

    Cryptographic rsa;
    rsa.generateRsakey(1024);
    cout << "[OK] RSA 密钥对生成成功" << endl;

    // 读取公钥
    ifstream ifs("public.pem");
    stringstream str;
    str << ifs.rdbuf();
    string pubKey = str.str();
    cout << "[OK] 公钥长度: " << pubKey.size() << " bytes" << endl;

    // 签名
    string sign = rsa.rsaSign(pubKey);
    cout << "[OK] 签名长度: " << sign.size() << " bytes" << endl;

    // 验签 (同对象内 - 用公钥验)
    bool ok = rsa.rsaVerify(pubKey, sign);
    cout << "[同对象验签] " << (ok ? "PASS" : "FAIL") << endl;

    // 模拟服务端: 公钥写入文件，重新加载，再验签
    ofstream ofs("public_test.pem");
    ofs << pubKey;
    ofs.close();

    Cryptographic rsa2("public_test.pem", false);
    bool ok2 = rsa2.rsaVerify(pubKey, sign);
    cout << "[重载公钥验签] " << (ok2 ? "PASS" : "FAIL") << endl;

    cout << endl << "========== 2. 编解码往返测试 ==========" << endl;

    RequestInfo reqInfo;
    reqInfo.clientID = "robin";
    reqInfo.serverID = "Luffy";
    reqInfo.cmd = 1;
    reqInfo.data = pubKey;
    reqInfo.sign = sign;

    RequestCodec encoder(&reqInfo);
    string encoded = encoder.encodeMsg();
    cout << "[OK] 编码长度: " << encoded.size() << " bytes" << endl;

    RequestCodec decoder(encoded);
    RequestInfo *decoded = (RequestInfo *)decoder.decodeMsg();
    cout << "[解码] clientID=" << decoded->clientID << endl;
    cout << "[解码] serverID=" << decoded->serverID << endl;
    cout << "[解码] data len=" << decoded->data.size() << endl;
    cout << "[解码] sign len=" << decoded->sign.size() << endl;

    bool dataMatch = (decoded->data == pubKey);
    bool signMatch = (decoded->sign == sign);
    cout << "[data 一致] " << (dataMatch ? "PASS" : "FAIL") << endl;
    cout << "[sign 一致] " << (signMatch ? "PASS" : "FAIL") << endl;

    // 解码后验签
    bool ok3 = rsa2.rsaVerify(decoded->data, decoded->sign);
    cout << "[解码后验签] " << (ok3 ? "PASS" : "FAIL") << endl;

    delete (RequestInfo *)decoded;

    cout << endl << "========== 3. 响应编解码测试 ==========" << endl;

    RespondInfo resp;
    resp.status = true;
    resp.seckeyID = 1;
    resp.clientID = "robin";
    resp.serverID = "Luffy";
    resp.data = "test_key_data";

    RespondCodec respEnc(&resp);
    string respStr = respEnc.encodeMsg();

    RespondCodec respDec(respStr);
    RespondInfo *respMsg = (RespondInfo *)respDec.decodeMsg();
    cout << "[响应 status] " << respMsg->status << endl;
    cout << "[响应 data] " << respMsg->data << endl;

    delete respMsg;

    cout << endl << "ALL TESTS DONE" << endl;
    return 0;
}
