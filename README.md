# Moderated Learning Management System (LMS)

![C++](https://img.shields.io/badge/C%2B%2B-17-blue.svg)
![SQLite](https://img.shields.io/badge/Database-SQLite-lightgrey.svg)
![Build](https://img.shields.io/badge/Build-Passing-brightgreen.svg)
![License](https://img.shields.io/badge/License-MIT-green.svg)

## 📖 Overview
The **Moderated Learning Management System (LMS)** is a robust, offline-first Command Line Interface (CLI) application developed for a secondary school environment. It manages the complete end-to-end assessment lifecycle, ensuring that academic integrity is maintained through a strict moderation workflow. 

Built in **C++17** with **SQLite** for persistent storage, this system solves the critical conflict between lecturers wanting to release marks quickly and the school's requirement that no student sees a mark until it has been reviewed by a Head of Department (HoD).

## ✨ Key Features
*   **Role-Based Access Control (RBAC):** Distinct, secure access levels for Administrators, Lecturers, Heads of Department, and Students.
*   **Moderation Workflow:** A built-in state machine that prevents marks from being released to students until explicitly approved by the HoD.
*   **Automated Late Penalties:** Accurately calculates a 10% penalty per day for up to 3 days, after which the mark becomes zero.
*   **Audit Logging:** Every mark change (Lecturer marking or HoD moderating) is recorded in a database audit log tracking *who* changed it, *when*, *the old value*, and the *reason*.
*   **Offline-First:** Uses an embedded SQLite database, meaning no internet connection is required.
*   **Security:** Passwords are never stored in plain text. They are cryptographically hashed and salted before database insertion.

## 🏗️ System Architecture & Design

The project follows a **Three-Tier Layered Architecture** to ensure separation of concerns:

1.  **Presentation Layer (CLI):** Handles user input/output and displays role-specific menus.
2.  **Business Logic Layer:** Enforces the rules (e.g., access control, late penalty math, moderation state).
3.  **Data Access Layer:** Manages all SQLite interactions using secure, parameterised SQL queries to prevent SQL injection.

### Design Patterns Applied
*   **Strategy Pattern:** Used for the `PenaltyCalculator`, allowing the late penalty algorithm to be changed without modifying the core `Submission` class.
*   **Repository Pattern:** The database layer acts as a repository, isolating raw SQL queries from the business logic.
*   **Factory Pattern:** Used to instantiate the different User roles dynamically upon login.
*   **RAII (Resource Acquisition Is Initialization):** Custom C++ wrapper classes ensure SQLite statements are safely closed, preventing memory leaks.

## 🗄️ Database Schema (ERD)
The database is normalized to **Third Normal Form (3NF)**. 
*   `users`: Stores user credentials, roles, and department links.
*   `courses` & `assignments`: Core academic entities.
*   `submissions`: Tracks student work, submission timestamps, and final marks.
*   `mark_audit`: An immutable ledger tracking every single mark modification.
*   `extensions`: Tracks individual and class-wide deadline extensions.

## 🚀 Getting Started

### Prerequisites
To build and run this project, you will need:
*   A C++17 compatible compiler (GCC, Clang, or MSVC).
*   CMake (version 3.10 or higher).
*   SQLite3 development libraries installed on your system.

### Installation & Build (Linux/macOS)
```bash
# 1. Clone the repository
git clone https://github.com/YourUsername/Moderated-LMS.git
cd Moderated-LMS

# 2. Create a build directory
mkdir build && cd build

# 3. Configure the project with CMake
cmake ..

# 4. Build the executable
cmake --build .

# 5. Run the application
./lms_app
