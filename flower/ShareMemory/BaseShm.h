#pragma once
#include <string>
#include <sys/ipc.h>
using namespace std;

class BaseShm {
public:
  BaseShm(int key);
  BaseShm(int key, int size);
  BaseShm(string name);
  BaseShm(string name, int size);
  void *mapShm();
  int unmapShm();
  int delShm();
  ~BaseShm();

private:
  int getShmID(key_t key, int shmSize, int flag);

  key_t m_key = -1;
  int m_size = 0;
  int m_shmID = -1;

protected:
  void *m_shmAddr = NULL;
};
