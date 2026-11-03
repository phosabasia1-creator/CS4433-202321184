#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <functional>
#include <sqlite3.h>

using namespace std;

// Global database connection
sqlite3* db = nullptr;

// ==========================================
// 1. DATABASE UTILITIES
// ==========================================

// Callback function to print query results
int printCallback(void* /*NotUsed*/, int argc, char** argv, char** azColName) {
    for (int i = 0; i < argc; i++) {
        cout << azColName[i] << ": " << (argv[i] ? argv[i] : "NULL") << " | ";
    }
    cout << "\n";
    return 0;
}

// Execute a simple SQL command (CREATE, INSERT, UPDATE, DELETE)
bool executeSQL(const string& sql) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL Error: " << (errMsg ? errMsg : "unknown") << "\n";
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

// Simulated Hashing (keeps code simple without OpenSSL, but shows security logic)
string hashPassword(const string& password) {
    hash<string> hasher;
    return to_string(hasher(password));
}

// Run the "released marks" query for a student and report the result
void studentChecksMarks(const string& student, bool afterRelease) {
    const char* checkSql =
        "SELECT final_score FROM Submissions WHERE student_name = ? AND status = 'RELEASED';";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, checkSql, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";
        return;
    }
    sqlite3_bind_text(stmt, 1, student.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        if (afterRelease) {
            cout << ">> SUCCESS: Student sees final score: " << sqlite3_column_int(stmt, 0) << " <<\n\n";
        } else {
            cout << "Student sees mark: " << sqlite3_column_int(stmt, 0) << "\n\n";
        }
    } else {
        cout << ">> ACCESS DENIED: Mark is pending HoD moderation. <<\n\n";
    }
    sqlite3_finalize(stmt);
}

// ==========================================
// 2. MAIN SYSTEM WORKFLOW
// ==========================================

int main() {
    // 1. CONNECT TO DATABASE
    int rc = sqlite3_open("lms.db", &db);
    if (rc != SQLITE_OK) {
        cerr << "Cannot open database: " << sqlite3_errmsg(db) << "\n";
        sqlite3_close(db);
        return 1;
    }
    cout << "Connected to SQLite database successfully.\n\n";

    // 2. CREATE TABLES (Matches Phase 3 ERD)
    executeSQL("PRAGMA foreign_keys=ON;");
    executeSQL("CREATE TABLE IF NOT EXISTS Users (id INTEGER PRIMARY KEY AUTOINCREMENT, username TEXT UNIQUE, password_hash TEXT, role TEXT);");
    executeSQL("CREATE TABLE IF NOT EXISTS Submissions (id INTEGER PRIMARY KEY AUTOINCREMENT, student_name TEXT, assignment_title TEXT, raw_score INTEGER, final_score INTEGER, status TEXT);");

    // Clear database for a fresh run (so you can run it multiple times)
    executeSQL("DELETE FROM Users;");
    executeSQL("DELETE FROM Submissions;");

    // 3. DEMONSTRATE THE WORKFLOW
    cout << "--- STEP 1: Admin Registers Users ---\n";
    string adminPass = hashPassword("admin123");
    string studentPass = hashPassword("stud123");
    executeSQL("INSERT INTO Users (username, password_hash, role) VALUES ('admin', '" + adminPass + "', 'ADMIN');");
    executeSQL("INSERT INTO Users (username, password_hash, role) VALUES ('john_doe', '" + studentPass + "', 'STUDENT');");
    cout << "Users created with hashed passwords.\n\n";

    cout << "--- STEP 2: Student Submits Work ---\n";
    executeSQL("INSERT INTO Submissions (student_name, assignment_title, raw_score, final_score, status) VALUES ('john_doe', 'Database Design', 0, 0, 'SUBMITTED');");
    cout << "Submission recorded.\n\n";

    cout << "--- STEP 3: Lecturer Marks Work (Late Penalty Applied) ---\n";
    // Business Logic: Raw Score = 80, Max = 100, 2 Days Late = 20% Penalty. Final = 60.
    executeSQL("UPDATE Submissions SET raw_score = 80, final_score = 60, status = 'MARKED' WHERE student_name = 'john_doe';");
    cout << "Lecturer marked work. Final score: 60.\n\n";

    cout << "--- STEP 4: Student Checks Marks (Before HoD Release) ---\n";
    studentChecksMarks("john_doe", false);

    cout << "--- STEP 5: HoD Releases Marks ---\n";
    executeSQL("UPDATE Submissions SET status = 'RELEASED' WHERE status = 'MARKED';");
    cout << "HoD has released the marks.\n\n";

    cout << "--- STEP 6: Student Checks Marks (After HoD Release) ---\n";
    studentChecksMarks("john_doe", true);

    // 4. VIEW FULL DATABASE TABLE (proves data is saved)
    cout << "--- FINAL DATABASE TABLE (Submissions) ---\n";
    char* errMsg = nullptr;
    if (sqlite3_exec(db, "SELECT * FROM Submissions;", printCallback, nullptr, &errMsg) != SQLITE_OK) {
        cerr << "SQL Error: " << (errMsg ? errMsg : "unknown") << "\n";
        sqlite3_free(errMsg);
    }

    sqlite3_close(db);
    return 0;
}