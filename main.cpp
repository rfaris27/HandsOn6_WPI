#include <iostream>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>

using namespace std;

int main()
{
    try
    {
        sql::mysql::MySQL_Driver *driver;
        sql::Connection *con;s

        driver = sql::mysql::get_mysql_driver_instance();
        // Host is "db" because we are inside the container network
        con = driver->connect("tcp://db:3306", "root", "p");
        con->setSchema("dev");

        sql::Statement *stmt;
        sql::ResultSet *res;
        stmt = con->createStatement();
        res = stmt->executeQuery("SELECT e.first_name, e.last_name, d.dept_name FROM employees e JOIN departments d ON e.dept_id = d.dept_id");

        cout << "--- Employee List ---" << endl;
        while (res->next())
        {
            cout << res->getString("first_name") << " "
                 << res->getString("last_name") << " | Dept: "
                 << res->getString("dept_name") << endl;
        }

        delete res;
        delete stmt;
        delete con;
    }
    catch (sql::SQLException &e)
    {
        cout << "# ERR: " << e.what() << endl;
    }

    return 0;
}