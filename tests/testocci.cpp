#include <iostream>
#include <occi.h>

using namespace oracle::occi;

int main() {
    Environment* env = nullptr;
    Connection* conn = nullptr;

    try {
        env = Environment::createEnvironment(Environment::DEFAULT);
        std::cout << "[OK] OCCI Environment created" << std::endl;

        conn = env->createConnection("system", "flower123", "localhost:1521/FREE");
        std::cout << "[OK] Oracle connection successful!" << std::endl;

        Statement* stmt = conn->createStatement(
            "SELECT 'Hello from Oracle ' || version FROM v$instance");
        ResultSet* rs = stmt->executeQuery();
        while (rs->next()) {
            std::cout << "[OK] " << rs->getString(1) << std::endl;
        }

        stmt->closeResultSet(rs);
        conn->terminateStatement(stmt);
        env->terminateConnection(conn);
        Environment::terminateEnvironment(env);

        std::cout << "OCCI OK!" << std::endl;
        return 0;
    } catch (SQLException& e) {
        std::cerr << "[ERROR] ORA-" << e.getErrorCode() << ": " << e.what() << std::endl;
        return 1;
    }
}
