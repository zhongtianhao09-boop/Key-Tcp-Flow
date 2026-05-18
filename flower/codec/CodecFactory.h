#ifndef CODECFACTORY_H
#define CODECFACTORY_H

#include "Codec.h"
#include <iostream>
using namespace std;

class CodecFactory {
public:
  CodecFactory();
  virtual ~CodecFactory();

  virtual Codec *createCodec() = 0;
};

#endif
