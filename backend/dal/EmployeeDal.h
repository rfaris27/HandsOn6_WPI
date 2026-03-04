#ifndef EMPLOYEEDAL_H
#define EMPLOYEEDAL_H

#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/prepared_statement.h>
#include <vector>
#include <memory>
#include "../model/Employee.h"

class EmployeeDAL {
private:
    sql::Connection* con;

public:
    // Constructor: Takes the database connection
    EmployeeDAL(sql::Connection* dbConnection);

    // (a) returns all employee data and department data (using JOIN)
    std::vector<EmployeeModel> GetAllEmployeeInfo();

    // (b & f) return employee data and department data for one person
    EmployeeModel GetEmployeeInfoById(int id);

    // (c) update employee data. Returns number of rows updated.
    int UpdateEmployeeInfo(int id, std::string firstName, std::string lastName, int years);

    // (d) create employee data.
    int CreateEmployeeInfo(std::string first, std::string last, std::string addr, std::string ph, int years, int deptId);

    // (e) remove the employee. Returns number of rows deleted.
    int DeleteEmployee(int id);
};

#endif