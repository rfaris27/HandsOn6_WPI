#include "EmployeeDAL.h"
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <iostream>

// Constructor
EmployeeDAL::EmployeeDAL(sql::Connection* dbConnection) : con(dbConnection) {}

// (a) GetAllEmployeeInfo
std::vector<EmployeeModel> EmployeeDAL::GetAllEmployeeInfo() {
    std::vector<EmployeeModel> employees;
    try {
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        sql::ResultSet *res = stmt->executeQuery(
            "SELECT e.emp_id, e.first_name, e.last_name, e.address, e.phone, "
            "e.years_service, d.dept_name, d.office_num "
            "FROM employees e "
            "JOIN departments d ON e.dept_id = d.dept_id"
        );

        while (res->next()) {
            EmployeeModel emp;
            emp.id = res->getInt("emp_id");
            emp.firstName = res->getString("first_name");
            emp.lastName = res->getString("last_name");
            emp.address = res->getString("address");
            emp.phone = res->getString("phone");
            emp.deptName = res->getString("dept_name");
            emp.officeNum = res->getString("office_num");
            
            // Handle NULL years of service
            if (res->isNull("years_service")) {
                emp.yearsService = -1; 
            } else {
                emp.yearsService = res->getInt("years_service");
            }
            employees.push_back(emp);
        }
        delete res;
    } catch (sql::SQLException &e) {
        std::cerr << "SQL Error in GetAll: " << e.what() << std::endl;
    }
    return employees;
}

// (b & f) GetEmployeeInfoById
EmployeeModel EmployeeDAL::GetEmployeeInfoById(int id) {
    EmployeeModel emp;
    emp.id = 0; // Default to 0 if not found
    try {
        std::unique_ptr<sql::PreparedStatement> stmt(con->prepareStatement(
            "SELECT e.*, d.dept_name, d.office_num FROM employees e "
            "JOIN departments d ON e.dept_id = d.dept_id WHERE e.emp_id = ?"
        ));
        stmt->setInt(1, id);
        sql::ResultSet *res = stmt->executeQuery();

        if (res->next()) {
            emp.id = res->getInt("emp_id");
            emp.firstName = res->getString("first_name");
            emp.lastName = res->getString("last_name");
            emp.deptName = res->getString("dept_name");
            emp.officeNum = res->getString("office_num");
            emp.yearsService = res->isNull("years_service") ? -1 : res->getInt("years_service");
        }
        delete res;
    } catch (sql::SQLException &e) {
        std::cerr << "SQL Error in GetById: " << e.what() << std::endl;
    }
    return emp;
}

// (c) UpdateEmployeeInfo
int EmployeeDAL::UpdateEmployeeInfo(int id, std::string firstName, std::string lastName, int years) {
    try {
        std::unique_ptr<sql::PreparedStatement> stmt(con->prepareStatement(
            "UPDATE employees SET first_name = ?, last_name = ?, years_service = ? WHERE emp_id = ?"
        ));
        stmt->setString(1, firstName);
        stmt->setString(2, lastName);
        stmt->setInt(3, years);
        stmt->setInt(4, id);
        return stmt->executeUpdate();
    } catch (sql::SQLException &e) { return 0; }
}

// (d) CreateEmployeeInfo
int EmployeeDAL::CreateEmployeeInfo(std::string first, std::string last, std::string addr, std::string ph, int years, int deptId) {
    try {
        std::unique_ptr<sql::PreparedStatement> stmt(con->prepareStatement(
            "INSERT INTO employees (first_name, last_name, address, phone, years_service, dept_id) VALUES (?, ?, ?, ?, ?, ?)"
        ));
        stmt->setString(1, first);
        stmt->setString(2, last);
        stmt->setString(3, addr);
        stmt->setString(4, ph);
        stmt->setInt(5, years);
        stmt->setInt(6, deptId);
        stmt->executeUpdate();
        
        // Return the last inserted ID as requested
        std::unique_ptr<sql::Statement> stmtId(con->createStatement());
        sql::ResultSet *res = stmtId->executeQuery("SELECT LAST_INSERT_ID()");
        int newId = 0;
        if (res->next()) newId = res->getInt(1);
        delete res;
        return newId;
    } catch (sql::SQLException &e) { return 0; }
}

// (e) DeleteEmployee
int EmployeeDAL::DeleteEmployee(int id) {
    try {
        std::unique_ptr<sql::PreparedStatement> stmt(con->prepareStatement(
            "DELETE FROM employees WHERE emp_id = ?"
        ));
        stmt->setInt(1, id);
        return stmt->executeUpdate();
    } catch (sql::SQLException &e) { return 0; }
}