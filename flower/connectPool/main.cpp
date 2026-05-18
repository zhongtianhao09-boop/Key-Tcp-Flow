// 连接池模式服务端 — 使用 ThreadPool + TaskQuene + Logger + SocketPool
// 启动方式: ./connectPool server.json

#include "../server/ServerOP.h"
#include "../Client/ClientOp.h"
#include "SocketPool/SocketPool.h"
#include "TaskQuene/TaskQuene.h"
#include "ThreadPool/ThreadPool.h"
#include "Logger/Logger.h"
#include <iostream>
#include <thread>
#include <chrono>
using namespace std;

int main(int argc, char **argv) {
    string configFile = argc > 1 ? argv[1] : "server.json";
    cout << "=== Flower Connection Pool Server ===" << endl;

    try {
        ServerOP server(configFile);
        cout << "[OK] Server initialized: " << configFile << endl;
        cout << "[OK] Pooled mode, 4 worker threads" << endl;
        server.startServerPooled(4);
    } catch (exception &e) {
        cerr << "[FATAL] " << e.what() << endl;
        return 1;
    }
    return 0;
}
