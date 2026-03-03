#include "crow_all.h" 
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>

using namespace std;

// Function to handle DB connection
sql::Connection* connectDB() {
    sql::mysql::MySQL_Driver *driver = sql::mysql::get_mysql_driver_instance();
    sql::Connection *con = driver->connect("tcp://127.0.0.1:3306", "root", "yourpassword");
    con->setSchema("dev");
    return con;
}

int main() {
    crow::SimpleApp app;

    // a) GET ALL
    CROW_ROUTE(app, "/employees")([](){
        auto con = connectDB();
        sql::Statement *stmt = con->createStatement();
        sql::ResultSet *res = stmt->executeQuery("CALL GetAllEmployeeInfo()");
        
        crow::json::wvalue x;
        int i = 0;
        while (res->next()) {
            x[i]["id"] = res->getInt("emp_id");
            x[i]["name"] = res->getString("first_name") + " " + res->getString("last_name");
            x[i]["department"] = res->getString("dept_name");
            i++;
        }
        delete res; delete stmt; delete con;
        return x;
    });

    // b) GET BY ID
    CROW_ROUTE(app, "/employees/<int>")([](int id){
        auto con = connectDB();
        unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("CALL GetEmployeeInfoById(?)"));
        pstmt->setInt(1, id);
        sql::ResultSet *res = pstmt->executeQuery();
        
        if (res->next()) {
            crow::json::wvalue x;
            x["first_name"] = res->getString("first_name");
            x["dept"] = res->getString("dept_name");
            return crow::response(x);
        }
        return crow::response(404);
    });

    // d) CREATE (POST)
    CROW_ROUTE(app, "/employees").methods("POST"_method)([](const crow::request& req){
        auto body = crow::json::load(req.body);
        auto con = connectDB();
        unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("CALL CreateEmployeeInfo(?, ?, ?)"));
        pstmt->setString(1, body["first_name"].s());
        pstmt->setString(2, body["last_name"].s());
        pstmt->setInt(3, body["dept_id"].i());
        
        sql::ResultSet *res = pstmt->executeQuery();
        res->next();
        return crow::response(to_string(res->getInt("new_id")));
    });

    // e) DELETE
    CROW_ROUTE(app, "/employees/<int>").methods("DELETE"_method)([](int id){
        auto con = connectDB();
        unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("CALL DeleteEmployee(?)"));
        pstmt->setInt(1, id);
        sql::ResultSet *res = pstmt->executeQuery();
        res->next();
        return crow::response(to_string(res->getInt("rows_affected")));
    });

    app.port(18080).multithreaded().run();
}