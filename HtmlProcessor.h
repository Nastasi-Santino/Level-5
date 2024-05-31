#ifndef HTMLPROCESSOR_H
#define HTMLPROCESSOR_H

#include <filesystem>
#include <iostream>
#include <fstream>
#include <map>
#include <set>

using namespace std;

class HtmlProcessor
{
public:
    void addHtml(filesystem::path HtmlPath);
    bool errorDetected;
    set<string> returnWords(string filename);

private:
    map<string, set<string>> HtmlList;
};

#endif