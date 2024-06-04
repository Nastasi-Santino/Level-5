/**
 * @file HttpRequestHandler.h
 * @author Marc S. Ressl
 * @author Santino Nastasi
 * @author Camila Castro
 * @brief EDAoggle search engine
 * @version 0.3
 *
 * @copyright Copyright (c) 2022-2024 Marc S. Ressl
 */

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sqlite3.h>
#include <chrono>

#include "HttpRequestHandler.h"

using namespace std;

HttpRequestHandler::HttpRequestHandler(string homePath)
{
    this->homePath = homePath;
}

/**
 * @brief callback function that works with database response to query. In this case, saves the page names to list.
 *
 * @param list pointer to our list
 * @param argc argc amount of columns in the response
 * @param argv list with the string of each row
 * @param azColName list with the string of each rows name
 * @return 0 if no problems occur
 */
static int databaseResponse(void *list,
                            int argc,
                            char **argv,
                            char **azColName)
{

    // Saves the first colum (only one) in list.
    if (argv[0])
    {
        string page = "https://es.wikipedia.org/wiki/";
        page += argv[0];
        ((vector<string> *)list)->push_back(page);
    }

    return 0;
}

/**
 * @brief Serves a webpage from file
 *
 * @param url The URL
 * @param response The HTTP response
 * @return true URL valid
 * @return false URL invalid
 */
bool HttpRequestHandler::serve(string url, vector<char> &response)
{
    // Blocks directory traversal
    // e.g. https://www.example.com/show_file.php?file=../../MyFile
    // * Builds absolute local path from url
    // * Checks if absolute local path is within home path
    auto homeAbsolutePath = filesystem::absolute(homePath);
    auto relativePath = homeAbsolutePath / url.substr(1);
    string path = filesystem::absolute(relativePath.make_preferred()).string();

    if (path.substr(0, homeAbsolutePath.string().size()) != homeAbsolutePath)
        return false;

    // Serves file
    ifstream file(path);
    if (file.fail())
        return false;

    file.seekg(0, ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, ios::beg);

    response.resize(fileSize);
    file.read(response.data(), fileSize);

    return true;
}

bool HttpRequestHandler::handleRequest(string url,
                                       HttpArguments arguments,
                                       vector<char> &response)
{
    string searchPage = "/search";
    if (url.substr(0, searchPage.size()) == searchPage)
    {
        string searchString;
        if (arguments.find("q") != arguments.end())
            searchString = arguments["q"];

        // Header
        string responseString = string("<!DOCTYPE html>\
<html>\
\
<head>\
    <meta charset=\"utf-8\" />\
    <title>EDAoogle</title>\
    <link rel=\"preload\" href=\"https://fonts.googleapis.com\" />\
    <link rel=\"preload\" href=\"https://fonts.gstatic.com\" crossorigin />\
    <link href=\"https://fonts.googleapis.com/css2?family=Inter:wght@400;800&display=swap\" rel=\"stylesheet\" />\
    <link rel=\"preload\" href=\"../css/style.css\" />\
    <link rel=\"stylesheet\" href=\"../css/style.css\" />\
</head>\
\
<body>\
    <article class=\"edaoogle\">\
        <div class=\"title\"><a href=\"/\">EDAoogle</a></div>\
        <div class=\"disclaimer\">Logical operators: ~ (NOT); | (OR); & (AND)</div>\
        <div class=\"search\">\
            <form action=\"/search\" method=\"get\">\
                <input type=\"text\" name=\"q\" value=\"" +
                                       searchString + "\" autofocus>\
            </form>\
        </div>\
        ");

        // YOUR JOB: fill in results
        float searchTime = 0.1F;
        vector<string> results;

        char *databaseFile = (char *)"index.db";
        sqlite3 *database;
        char *databaseErrorMessage;

        auto start = chrono::high_resolution_clock::now();

        // Open database file
        if (sqlite3_open(databaseFile, &database) != SQLITE_OK)
        {
            cout << "Can't open database: " << sqlite3_errmsg(database) << endl;

            return false;
        }

        // Includes Operators, & = AND, | = OR, ~ = NOT. and takes out problematic simbols.
        string newSearchString;
        for (auto simbol : searchString)
        {
            if (simbol == '&')
            {
                newSearchString += " AND ";
            }
            else if (simbol == '|')
            {
                newSearchString += " OR ";
            }
            else if (simbol == '~')
            {
                newSearchString += " NOT ";
            }
            else if (simbol == 32 || (simbol > 47 && simbol < 57) || (simbol > 64 && simbol < 123) || (simbol > 127 && simbol < 166))
            {
                newSearchString += simbol;
            }
        }

        // Command to search with fts.
        string sqlCommand = "SELECT page_name from wiki_pages_fts WHERE wiki_pages_fts MATCH '" + newSearchString + "'";

        if (sqlite3_exec(database,
                         sqlCommand.c_str(),
                         databaseResponse,
                         &results,
                         &databaseErrorMessage) != SQLITE_OK)
        {
            cout << "Error: " << sqlite3_errmsg(database) << endl;
        }

        // Close database
        sqlite3_close(database);

        auto stop = chrono::high_resolution_clock::now();

        searchTime = chrono::duration_cast<chrono::milliseconds>(stop - start).count() / 1000.0F;

        // Print search results (add target= "_blank" in the href so it opens up in a new tab)
        responseString += "<div class=\"results\">" + to_string(results.size()) +
                          " results (" + to_string(searchTime) + " seconds):</div>";
        for (auto &result : results)
            responseString += "<div class=\"result\"><a href=\"" +
                              result + "\" target=\"_blank\">" + result + "</a></div>";

        // Trailer
        responseString += "    </article>\
</body>\
</html>";

        response.assign(responseString.begin(), responseString.end());

        return true;
    }
    else
        return serve(url, response);

    return false;
}
