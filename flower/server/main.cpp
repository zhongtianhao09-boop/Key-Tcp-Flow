#include "ServerOP.h"
#include <cstdio>
int main() {
  ServerOP op("server.json");
  op.startServer();
}