CREATE TABLE IF NOT EXISTS departments (
    dept_id INT PRIMARY KEY AUTO_INCREMENT,
    dept_name VARCHAR(100),
    office_num VARCHAR(50)
);

CREATE TABLE IF NOT EXISTS employees (
    emp_id INT PRIMARY KEY AUTO_INCREMENT,
    first_name VARCHAR(50),
    last_name VARCHAR(50),
    address VARCHAR(100),
    phone VARCHAR(20),
    years_service INT NULL, -- NULL allowed for Pat Green
    dept_id INT,
    FOREIGN KEY (dept_id) REFERENCES departments(dept_id)
);

-- Seed data for testing
INSERT INTO departments (dept_name, office_num) VALUES ('Engineering', '101A'), ('HR', '202B');
INSERT INTO employees (first_name, last_name, address, phone, years_service, dept_id) 
VALUES ('Mike', 'Black', '123 Ave', '555-0101', 5, 1),
       ('Pat', 'Green', '456 Road', '555-0202', NULL, 2);