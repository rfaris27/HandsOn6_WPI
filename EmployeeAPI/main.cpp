#include "crow_all.h"
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <memory>
#include <string>

using namespace std;

// Function to handle Database Connection
sql::Connection* connectDB() {
    try {
        sql::mysql::MySQL_Driver *driver = sql::mysql::get_mysql_driver_instance();
        // Replace "p" with your actual password if "p" is not it
        sql::Connection *con = driver->connect("tcp://127.0.0.1:3306", "root", "p");
        con->setSchema("dev");
        return con;
    } catch (sql::SQLException &e) {
        return nullptr;
    }
}

int main() {
    crow::SimpleApp app;

    // a) GET ALL
    CROW_ROUTE(app, "/employees")([](){
        auto con = connectDB();
        if (!con) return crow::response(500, "DB Connection Failed");

        sql::Statement *stmt = con->createStatement();
        sql::ResultSet *res = stmt->executeQuery("CALL GetAllEmployeeInfo()");
        
        crow::json::wvalue x;
        int i = 0;
        while (res->next()) {
            x[i]["id"] = res->getInt("emp_id");
            x[i]["first_name"] = res->getString("first_name");
            x[i]["last_name"] = res->getString("last_name");
            x[i]["department"] = res->getString("dept_name");
            i++;
        }
        delete res; delete stmt; delete con;
        return crow::response(x);
    });

    // b) GET BY ID
    CROW_ROUTE(app, "/employees/<int>")([](int id){
        auto con = connectDB();
        if (!con) return crow::response(500, "DB Connection Failed");

        unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("CALL GetEmployeeInfoById(?)"));
        pstmt->setInt(1, id);
        sql::ResultSet *res = pstmt->executeQuery();
        
        if (res->next()) {
            crow::json::wvalue x;
            x["id"] = res->getInt("emp_id");
            x["first_name"] = res->getString("first_name");
            x["last_name"] = res->getString("last_name");
            x["department"] = res->getString("dept_name");
            delete res; delete con;
            return crow::response(x);
        }
        delete res; delete con;
        return crow::response(404, "Not Found");
    });

    // c) UPDATE (PUT) - Fixed the (string) conversion here
    CROW_ROUTE(app, "/employees/<int>").methods("PUT"_method)([](const crow::request& req, int id){
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, "Invalid JSON");
        
        auto con = connectDB();
        unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("CALL UpdateEmployeeInfo(?, ?, ?)"));
        pstmt->setInt(1, id);
        pstmt->setString(2, (string)body["first_name"].s()); // Added (string) cast
        pstmt->setString(3, (string)body["last_name"].s());  // Added (string) cast
        
        sql::ResultSet *res = pstmt->executeQuery();
        res->next();
        int rows = res->getInt("rows_affected");
        delete res; delete con;
        return crow::response(to_string(rows));
    });

    // d) CREATE (POST) - Fixed the (string) conversion here
    CROW_ROUTE(app, "/employees").methods("POST"_method)([](const crow::request& req){
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, "Invalid JSON");

        auto con = connectDB();
        unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("CALL CreateEmployeeInfo(?, ?, ?)"));
        pstmt->setString(1, (string)body["first_name"].s()); // Added (string) cast
        pstmt->setString(2, (string)body["last_name"].s());  // Added (string) cast
        pstmt->setInt(3, body["dept_id"].i());
        
        sql::ResultSet *res = pstmt->executeQuery();
        res->next();
        int newId = res->getInt("new_id");
        delete res; delete con;
        return crow::response(to_string(newId));
    });

    // e) DELETE
    CROW_ROUTE(app, "/employees/<int>").methods("DELETE"_method)([](int id){
        auto con = connectDB();
        unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("CALL DeleteEmployee(?)"));
        pstmt->setInt(1, id);
        sql::ResultSet *res = pstmt->executeQuery();
        res->next();
        int rows = res->getInt("rows_affected");
        delete res; delete con;
        return crow::response(to_string(rows));
    });

    app.port(18080).multithreaded().run();
}