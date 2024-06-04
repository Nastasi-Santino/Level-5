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
#include <fstream>
#include <chrono>

#include <sqlite3.h>

#include "CommandLineParser.h"

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

string processHtmls(filesystem::path HtmlPath)
{

    string processedText;
    ifstream html;

    html.open(HtmlPath, std::ios::in);
    if (!html.is_open())
    {
        std::cout << "error opening " << HtmlPath.filename() << std::endl;
        return "";
    }

    while (!html.eof())
    {

        if (html.peek() == '<')
        {
            html.ignore(std::numeric_limits<std::streamsize>::max(), '>');
        }
        else
        {
            // cout << "entre al else" << endl;
            string word;
            while (html.peek() != '<' && !html.eof())
            {
                if (html.peek() == '\'')
                {
                    html.ignore(1);
                }
                else
                {
                    word.push_back(html.get());
                }
            }

            if (!word.empty())
            {
                processedText += word;
            }
        }
    }

    return processedText;
}

string PageNameEditor(string name)
{

    string finalName;
    const char *n = name.c_str();
    int i = 0;

    while (*(n + i) != '.')
    {
        if (*(n + i) != '\'')
        {
            finalName += *(n + i);
        }
        i++;
    }

    return finalName;
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
                     "CREATE TABLE wiki_pages"
                     "(id INTEGER PRIMARY KEY,"
                     " page varchar DEFAULT NULL,"
                     " pageText text DEFAULT NULL);",
                     NULL,
                     0,
                     &databaseErrorMessage) != SQLITE_OK)
    {
        cout << "Error: " << sqlite3_errmsg(database) << endl;
    }

    // Create a sample table
    cout << "Creating virtual table..." << endl;
    if (sqlite3_exec(database,
                     "CREATE VIRTUAL TABLE wiki_pages_fts USING fts5 "
                     "(page_name,"
                     " content);",
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

    cout << "Deleting previous entries..." << endl;
    if (sqlite3_exec(database,
                     "DELETE FROM wiki_pages_fts;",
                     NULL,
                     0,
                     &databaseErrorMessage) != SQLITE_OK)
    {
        cout << "Error: " << sqlite3_errmsg(database) << endl;
    }

    // Create sample entries
    cout << "Creating sample entries..." << endl;

    int i = 1;
    sqlite3_stmt *stmt;
    for (auto file : wiki)
    {

        string pageName = PageNameEditor(file.path().filename());
        string text = processHtmls(file.path());

        if (!text.empty())
        {
            string sqlCommand = "INSERT INTO wiki_pages (page, pageText) VALUES (?, ?);";
            if (sqlite3_prepare_v2(database,
                                   sqlCommand.c_str(),
                                   -1,
                                   &stmt,
                                   NULL) != SQLITE_OK)
                cout << "Error: " << sqlite3_errmsg(database) << endl;

            else if (sqlite3_bind_text(stmt, 1, pageName.c_str(), -1, SQLITE_STATIC) != SQLITE_OK || sqlite3_bind_text(stmt, 2, text.c_str(), -1, SQLITE_STATIC) != SQLITE_OK)
                cout << "Error: " << sqlite3_errmsg(database) << endl;

            if (sqlite3_step(stmt) != SQLITE_DONE)
                cout << "Error: " << sqlite3_errmsg(database) << endl;

            sqlite3_finalize(stmt);
        }
    }

    cout << "Copying entries to virtual table..." << endl;
    if (sqlite3_exec(database,
                     "INSERT INTO wiki_pages_fts (rowid, page_name, content) SELECT id, page, pageText FROM wiki_pages",
                     NULL,
                     0,
                     &databaseErrorMessage) != SQLITE_OK)
        cout << "Error: " << sqlite3_errmsg(database) << endl;

    // Close database
    cout << "Closing database..." << endl;
    sqlite3_close(database);
}
