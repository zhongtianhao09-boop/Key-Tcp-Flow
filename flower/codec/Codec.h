#ifndef CODEC_H
#define CODEC_H

#include <iostream>
using namespace std;
class Codec {
public:
  Codec();
  virtual string encodeMsg() = 0;
  virtual void *decodeMsg() = 0;
  virtual ~Codec();
};

#endif
