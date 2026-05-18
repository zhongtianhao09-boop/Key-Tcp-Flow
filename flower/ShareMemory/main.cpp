#include "SecKeyShm.h"
#include "../SecKeyNodeInfo/SeckeyNodeInfo.h"
#include <iostream>
#include <cstring>
using namespace std;

int main() {
    cout << "=== 共享内存测试 ===" << endl;

    SecKeyShm shm("sharememory_test", 4);
    shm.shmInit();

    NodeSecKeyInfo node;
    strcpy(node.clientID, "test1");
    strcpy(node.serverID, "test2");
    strcpy(node.seckey, "testkey123");
    node.seckeyID = 1;
    node.status = 1;

    shm.shmWrite(&node);
    cout << "[1] 写入完成" << endl;

    NodeSecKeyInfo result = shm.shmRead("test1", "test2");
    cout << "[2] 读取: clientID=" << result.clientID
         << " serverID=" << result.serverID
         << " key=" << result.seckey
         << " status=" << result.status << endl;

    bool match = (strcmp(result.clientID, "test1") == 0 &&
                  strcmp(result.seckey, "testkey123") == 0 &&
                  result.status == 1);
    cout << (match ? "PASS" : "FAIL") << endl;
    return match ? 0 : 1;
}
