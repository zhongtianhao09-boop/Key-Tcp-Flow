#include "interface.h"
#include <iostream>

int main() {
    Interface iface = Interface::fromJsonFile("../.claude/Client.json");
    std::string enc = iface.encryptData("hello flower");
    std::cout << "[encrypt] " << enc.size() << " bytes" << std::endl;
    std::string dec = iface.decryptData(enc);
    std::cout << "[decrypt] " << dec << std::endl;
    return 0;
}
