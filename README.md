# HandsOn6_WPI# HandsOn 6 - Employee Management System (3-Layer Architecture)

This project is a RESTful API built with **C++** using the **Crow** framework and **MySQL**. It manages employee and department data following a strict 3-layered software architecture.

## Features & Requirements Met
- **Layered Architecture**: Separated into API (Routes), DAL (Data Access), and Model (Data Structures).
- **Database Normalization**: Uses `employees` and `departments` tables with Foreign Key relationships.
- **SQL Joins**: All "Get" requests perform a `JOIN` to retrieve department names and office numbers.
- **NULL Handling**: Specifically handles missing "Years of Service" data (e.g., Pat Green) by returning "No Data Available" instead of a blank or error.
- **CRUD Operations**: Full support for Creating, Reading, Updating, and Deleting employees.
- **Query String Search**: Implements Part F requirement via `/api/employees/search?id=X`.

---

##  Project Structure

```text
HandsOn6_WPI/
├── api/             # Presentation Layer (Crow Routes & HTTP Logic)
├── dal/             # Data Access Layer (SQL Queries & DB Connection)
├── model/           # Data Models (C++ Structs)
├── database/        # SQL Scripts (init.sql)
└── wpi_app          # Compiled Executable