#include "BaseShm.h"
#include <cstring>
#include <iostream>
#include <string>
#include <sys/ipc.h>
#include <sys/shm.h>

using namespace std;

BaseShm::BaseShm(int key) : m_key(key), m_size(0), m_shmID(-1), m_shmAddr(nullptr) {}

BaseShm::BaseShm(int key, int size) : m_key(key), m_size(size), m_shmID(-1), m_shmAddr(nullptr) {}

BaseShm::BaseShm(string name) : m_size(0), m_shmID(-1), m_shmAddr(nullptr) {
    m_key = ftok(name.c_str(), 'R');
    if (m_key == -1) {
        perror("ftok");
    }
}

BaseShm::BaseShm(string name, int size) : m_size(size), m_shmID(-1), m_shmAddr(nullptr) {
    m_key = ftok(name.c_str(), 'R');
    if (m_key == -1) {
        perror("ftok");
    }
}

BaseShm::~BaseShm() {
    unmapShm();
}

int BaseShm::getShmID(key_t key, int shmSize, int flag) {
    int id = shmget(key, shmSize, flag);
    if (id == -1) {
        perror("shmget");
    }
    return id;
}

void *BaseShm::mapShm() {
    if (m_shmAddr)
        return m_shmAddr;

    if (m_shmID == -1) {
        m_shmID = getShmID(m_key, m_size, IPC_CREAT | 0666);
        if (m_shmID == -1)
            return nullptr;
    }

    m_shmAddr = shmat(m_shmID, nullptr, 0);
    if (m_shmAddr == (void *)-1) {
        perror("shmat");
        m_shmAddr = nullptr;
    }
    return m_shmAddr;
}

int BaseShm::unmapShm() {
    if (m_shmAddr) {
        shmdt(m_shmAddr);
        m_shmAddr = nullptr;
    }
    return 0;
}

int BaseShm::delShm() {
    int ret = -1;
    if (m_shmID != -1) {
        ret = shmctl(m_shmID, IPC_RMID, nullptr);
        m_shmID = -1;
    }
    return ret;
}
