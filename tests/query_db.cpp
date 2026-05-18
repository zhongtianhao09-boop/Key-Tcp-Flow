#include <iostream>
#include <occi.h>
using namespace oracle::occi;

int main() {
    Environment* env = nullptr;
    Connection* conn = nullptr;
    try {
        env = Environment::createEnvironment(Environment::DEFAULT);
        conn = env->createConnection("SECMNG", "SECMNG", "localhost:1521/FREEPDB1");
        std::cout << "=== SECKEYINFO (密钥信息) ===" << std::endl;
        Statement* stmt = conn->createStatement(
            "SELECT clientid, serverid, keyid, state, seckey FROM SECMNG.SECKEYINFO ORDER BY keyid");
        ResultSet* rs = stmt->executeQuery();
        while (rs->next()) {
            std::cout << rs->getString(1) << " | " << rs->getString(2) << " | "
                      << rs->getInt(3) << " | " << rs->getInt(4) << " | "
                      << rs->getString(5) << std::endl;
        }
        stmt->closeResultSet(rs);
        conn->terminateStatement(stmt);

        std::cout << std::endl << "=== SECNODE (节点信息) ===" << std::endl;
        stmt = conn->createStatement("SELECT * FROM SECMNG.SECNODE");
        rs = stmt->executeQuery();
        while (rs->next()) {
            std::cout << rs->getString(1) << " | " << rs->getString(2) << std::endl;
        }
        stmt->closeResultSet(rs);
        conn->terminateStatement(stmt);

        env->terminateConnection(conn);
        Environment::terminateEnvironment(env);
        return 0;
    } catch (SQLException& e) {
        std::cerr << "ORA-" << e.getErrorCode() << ": " << e.what() << std::endl;
        return 1;
    }
}
