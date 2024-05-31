/**
 * @file mkindex.cpp
 * @author Marc S. Ressl
 * @brief Makes a database index
 * @version 0.3
 *
 * @copyright Copyright (c) 2022-2024 Marc S. Ressl
 */

#include <iostream>
#include <string>
#include <filesystem>
#include <chrono>

#include <sqlite3.h>

#include "CommandLineParser.h"
#include "HtmlProcessor.h"

using namespace std;

static int onDatabaseEntry(void *userdata,
                           int argc,
                           char **argv,
                           char **azColName)
{
    cout << "--- Entry" << endl;
    for (int i = 0; i < argc; i++)
    {
        if (argv[i])
            cout << azColName[i] << ": " << argv[i] << endl;
        else
            cout << azColName[i] << ": " << "NULL" << endl;
    }

    return 0;
}

int main(int argc,
         const char *argv[])
{

    CommandLineParser parser(argc, argv);

    if (!parser.hasOption("-h"))
    {

        cout << "error: WWW_PATH must be specified." << endl;

        return 1;
    }

    filesystem::path wwwPath(parser.getOption("-h"));
    filesystem::path wikiPath = wwwPath.concat("/wiki");

    error_code wikiNotFound;
    filesystem::directory_iterator wiki(wikiPath, wikiNotFound);
    if (wikiNotFound)
    {

        cout << "error WIKI not founded." << endl;

        return 1;
    }

    HtmlProcessor processor;

    auto start = chrono::high_resolution_clock::now();

    for (auto file : wiki)
    {
        processor.addHtml(file.path());
        if(processor.errorDetected){
            cout << "error reading " << file.path().filename() << endl;
        }
    }

    auto stop = chrono::high_resolution_clock::now();

    cout << chrono::duration_cast<chrono::milliseconds>(stop-start).count() / 1000.0F << endl;

    char *databaseFile = (char *)"index.db";
    sqlite3 *database;
    char *databaseErrorMessage;


    // Open database file
    cout << "Opening database..." << endl;
    if (sqlite3_open(databaseFile, &database) != SQLITE_OK)
    {
        cout << "Can't open database: " << sqlite3_errmsg(database) << endl;

        return 1;
    }

    // Create a sample table
    cout << "Creating table..." << endl;
    if (sqlite3_exec(database,
                     "CREATE TABLE wiki_pages "
                     "(id INTEGER PRIMARY KEY,"
                     " page_name varchar DEFAULT NULL,"
                     " word varchar DEFAULT NULL);",
                     NULL,
                     0,
                     &databaseErrorMessage) != SQLITE_OK)
        cout << "Error: " << sqlite3_errmsg(database) << endl;

    // Delete previous entries if table already existed
    cout << "Deleting previous entries..." << endl;
    if (sqlite3_exec(database,
                     "DELETE FROM wiki_pages;",
                     NULL,
                     0,
                     &databaseErrorMessage) != SQLITE_OK)
        cout << "Error: " << sqlite3_errmsg(database) << endl;

    // // Create sample entries
    // cout << "Creating sample entries..." << endl;
    // if (sqlite3_exec(database,
    //                  "INSERT INTO room_occupation (room, reserved_from, reserved_until) VALUES "
    //                  "('401F','2022-03-03 15:00:00','2022-03-03 16:00:00');",
    //                  NULL,
    //                  0,
    //                  &databaseErrorMessage) != SQLITE_OK)
    //     cout << "Error: " << sqlite3_errmsg(database) << endl;
    // if (sqlite3_exec(database,
    //                  "INSERT INTO room_occupation (room, reserved_from, reserved_until) VALUES "
    //                  "('501F','2022-03-03 15:00:00','2022-03-03 17:00:00');",
    //                  NULL,
    //                  0,
    //                  &databaseErrorMessage) != SQLITE_OK)
    //     cout << "Error: " << sqlite3_errmsg(database) << endl;
    // if (sqlite3_exec(database,
    //                  "INSERT INTO room_occupation (room, reserved_from, reserved_until) VALUES "
    //                  "('001R','2022-03-03 15:00:00','2022-03-03 16:30:00');",
    //                  NULL,
    //                  0,
    //                  &databaseErrorMessage) != SQLITE_OK)
    //     cout << "Error: " << sqlite3_errmsg(database) << endl;
    // if (sqlite3_exec(database,
    //                  "INSERT INTO room_occupation (room, reserved_from, reserved_until) VALUES "
    //                  "('1001F','2022-03-03 15:30:00','2022-03-03 16:30:00');",
    //                  NULL,
    //                  0,
    //                  &databaseErrorMessage) != SQLITE_OK)
    //     cout << "Error: " << sqlite3_errmsg(database) << endl;

    // // Fetch entries
    // cout << "Fetching entries..." << endl;
    // if (sqlite3_exec(database,
    //                  "SELECT * from room_occupation;",
    //                  onDatabaseEntry,
    //                  0,
    //                  &databaseErrorMessage) != SQLITE_OK)
    //     cout << "Error: " << sqlite3_errmsg(database) << endl;

    // Close database
    cout << "Closing database..." << endl;
    sqlite3_close(database);
}
