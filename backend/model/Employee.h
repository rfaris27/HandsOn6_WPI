#ifndef EMPLOYEE_H
#define EMPLOYEE_H
#include <string>

struct EmployeeModel {
    int id;
    std::string firstName;
    std::string lastName;
    std::string address;
    std::string phone;
    std::string deptName;
    std::string officeNum;
    int yearsService; // -1 means "No Data"
};
#endif