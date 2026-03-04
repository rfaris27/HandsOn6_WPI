#include "crow.h"
#include "../dal/EmployeeDAL.h"
#include "../model/Employee.h"
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <string>
#include <vector>

int main() {
    // 1. Database Connection
    sql::mysql::MySQL_Driver *driver;
    sql::Connection *con;
    driver = sql::mysql::get_mysql_driver_instance();
    
    // Connect to the 'db' container
    con = driver->connect("tcp://db:3306", "root", "p");
    con->setSchema("dev");

    EmployeeDAL dal(con);
    crow::SimpleApp app;

    // --- ROUTES ---

    // (a) Get All Employee Info (JOIN requirement)
    CROW_ROUTE(app, "/api/employees")([&dal](){
        auto employees = dal.GetAllEmployeeInfo();
        std::vector<crow::json::wvalue> json_list;
        
        for (const auto& emp : employees) {
            crow::json::wvalue j;
            j["id"] = emp.id;
            j["name"] = emp.firstName + " " + emp.lastName;
            j["address"] = emp.address;
            j["phone"] = emp.phone;
            j["dept"] = emp.deptName;
            j["office"] = emp.officeNum;
            // Handle Part B: NULL years_service requirement
            j["years_service"] = (emp.yearsService == -1) ? "No Data Available" : std::to_string(emp.yearsService);
            json_list.push_back(j);
        }
        return crow::json::wvalue(json_list);
    });

    // (b) Get By ID - Route Parameter (Example: /api/employees/1)
    CROW_ROUTE(app, "/api/employees/<int>")([&dal](int id){
        auto emp = dal.GetEmployeeInfoById(id);
        if (emp.id == 0) return crow::response(404, "Employee Not Found");

        crow::json::wvalue j;
        j["id"] = emp.id;
        j["firstName"] = emp.firstName;
        j["lastName"] = emp.lastName;
        j["department"] = emp.deptName;
        j["years_service"] = (emp.yearsService == -1) ? "No Data Available" : std::to_string(emp.yearsService);
        return crow::response(j);
    });

    // (c) Update Employee Info
    // Example: /api/employees/update/1?first=Mikey&last=Black&years=10
    CROW_ROUTE(app, "/api/employees/update/<int>")([&dal](const crow::request& req, int id){
        auto first = req.url_params.get("first") ? req.url_params.get("first") : "";
        auto last = req.url_params.get("last") ? req.url_params.get("last") : "";
        auto years_str = req.url_params.get("years") ? req.url_params.get("years") : "0";
        
        int rows = dal.UpdateEmployeeInfo(id, first, last, std::stoi(years_str));
        if (rows > 0) return crow::response(200, "Update Successful");
        return crow::response(404, "Update Failed - Employee not found");
    });

    // (d) Create Employee Info (Returns New ID)
    // Example: /api/employees/add?first=John&last=Doe&addr=MainSt&phone=555&years=2&dept=1
    CROW_ROUTE(app, "/api/employees/add")([&dal](const crow::request& req){
        auto first = req.url_params.get("first") ? req.url_params.get("first") : "New";
        auto last = req.url_params.get("last") ? req.url_params.get("last") : "User";
        auto addr = req.url_params.get("addr") ? req.url_params.get("addr") : "N/A";
        auto ph = req.url_params.get("phone") ? req.url_params.get("phone") : "N/A";
        auto years = req.url_params.get("years") ? std::stoi(req.url_params.get("years")) : 0;
        auto dept = req.url_params.get("dept") ? std::stoi(req.url_params.get("dept")) : 1;

        int newId = dal.CreateEmployeeInfo(first, last, addr, ph, years, dept);
        return crow::response(200, "Created Employee with ID: " + std::to_string(newId));
    });

    // (e) Delete Employee
    CROW_ROUTE(app, "/api/employees/delete/<int>")([&dal](int id){
        int rows = dal.DeleteEmployee(id);
        if (rows > 0) return crow::response(200, "Employee deleted successfully");
        return crow::response(404, "No employee found to delete");
    });

    // (f) Get By ID - Query String Requirement
    // Example: /api/employees/search?id=1
    CROW_ROUTE(app, "/api/employees/search")([&dal](const crow::request& req){
        auto id_ptr = req.url_params.get("id");
        if (id_ptr == nullptr) return crow::response(400, "Missing id parameter");
        
        int id = std::stoi(id_ptr);
        auto emp = dal.GetEmployeeInfoById(id);
        
        if (emp.id == 0) return crow::response(404, "Employee Not Found");

        crow::json::wvalue j;
        j["id"] = emp.id;
        j["fullName"] = emp.firstName + " " + emp.lastName;
        j["department"] = emp.deptName;
        return crow::response(j);
    });

    app.port(18080).multithreaded().run();
    
    delete con;
    return 0;
}