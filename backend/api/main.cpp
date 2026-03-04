#include "crow.h"
#include "../dal/EmployeeDAL.h"
#include "../model/Employee.h"
#include <mysql_driver.h>
#include <mysql_connection.h>

int main() {
    // 1. Database Connection Setup
    sql::mysql::MySQL_Driver *driver;
    sql::Connection *con;
    driver = sql::mysql::get_mysql_driver_instance();
    
    // Using "db" because we are in the Dev Container environment
    con = driver->connect("tcp://db:3306", "root", "p");
    con->setSchema("dev");

    EmployeeDAL dal(con);
    crow::SimpleApp app;

    // (a) Get All - Returns a JSON list of everyone
    CROW_ROUTE(app, "/api/employees")([&dal](){
        auto employees = dal.GetAllEmployeeInfo();
        std::vector<crow::json::wvalue> json_list;
        
        for (const auto& emp : employees) {
            crow::json::wvalue j;
            j["id"] = emp.id;
            j["name"] = emp.firstName + " " + emp.lastName;
            j["dept"] = emp.deptName;
            j["office"] = emp.officeNum;
            // Handle the NULL/No data requirement
            j["years_service"] = (emp.yearsService == -1) ? "No Data Available" : std::to_string(emp.yearsService);
            json_list.push_back(j);
        }
        return crow::json::wvalue(json_list);
    });

    // (b) Get By ID - Route Parameter (/api/employees/1)
    CROW_ROUTE(app, "/api/employees/<int>")([&dal](int id){
        auto emp = dal.GetEmployeeInfoById(id);
        if (emp.id == 0) return crow::response(404, "Employee Not Found");

        crow::json::wvalue j;
        j["id"] = emp.id;
        j["firstName"] = emp.firstName;
        j["lastName"] = emp.lastName;
        j["department"] = emp.deptName;
        return crow::response(j);
    });

    // (f) Get By ID - Query String (/api/employees/search?id=1)
    CROW_ROUTE(app, "/api/employees/search")([&dal](const crow::request& req){
        auto id_ptr = req.url_params.get("id");
        if (id_ptr == nullptr) return crow::response(400, "Missing id parameter");
        
        int id = std::stoi(id_ptr);
        auto emp = dal.GetEmployeeInfoById(id);
        
        if (emp.id == 0) return crow::response(404, "Employee Not Found");

        crow::json::wvalue j;
        j["id"] = emp.id;
        j["fullName"] = emp.firstName + " " + emp.lastName;
        j["method"] = "Query String Search";
        return crow::response(j);
    });

    // (e) Delete Employee
    CROW_ROUTE(app, "/api/employees/delete/<int>")([&dal](int id){
        int rows = dal.DeleteEmployee(id);
        if (rows > 0) return crow::response(200, "Employee deleted successfully");
        return crow::response(404, "No employee found to delete");
    });

    app.port(18080).multithreaded().run();
    
    delete con;
    return 0;
}