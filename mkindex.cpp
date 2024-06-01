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
    {
        cout << "Error: " << sqlite3_errmsg(database) << endl;
    }

    // Delete previous entries if table already existed
    cout << "Deleting previous entries..." << endl;
    if (sqlite3_exec(database,
                     "DELETE FROM wiki_pages;",
                     NULL,
                     0,
                     &databaseErrorMessage) != SQLITE_OK)
    {
        cout << "Error: " << sqlite3_errmsg(database) << endl;
    }

    HtmlProcessor processor;

    // Create sample entries
    cout << "Creating sample entries..." << endl;

    auto start = chrono::high_resolution_clock::now();

    for (auto file : wiki)
    {
        std::string pageName = file.path().filename();
        processor.addHtml(file.path());
        if (processor.errorDetected)
            cout << "error reading " << pageName << endl;
        // else
        // {
        //     for (auto word : processor.returnWords(pageName))
        //     {
        //         std::string newWikiPage = "INSERT INTO wiki_pages (page_name, word) VALUES ('" + pageName + "','" + word + "');";
        //         cout << pageName + " " + word << endl;
        //         if (sqlite3_exec(database,
        //                          newWikiPage.c_str(),
        //                          NULL,
        //                          0,
        //                          &databaseErrorMessage) != SQLITE_OK)
        //             cout << "Error: " << sqlite3_errmsg(database) << endl;
        //     }
        // }
    }
    // INSERT INTO wiki_pages(page_name, word) VALUES('ABBA.html', 'hola');

    auto stop = chrono::high_resolution_clock::now();

    cout << chrono::duration_cast<chrono::milliseconds>(stop - start).count() / 1000.0F << endl;

    // Fetch entries
    cout << "Fetching entries..." << endl;
    if (sqlite3_exec(database,
                     "SELECT * from wiki_pages WHERE id = 1;",
                     onDatabaseEntry,
                     0,
                     &databaseErrorMessage) != SQLITE_OK)
    {
        cout << "Error: " << sqlite3_errmsg(database) << endl;
    }
    // Close database
    cout << "Closing database..." << endl;
    sqlite3_close(database);
}
