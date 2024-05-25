#ifndef HTMLPROCESSOR_H
#define HTMLPROCESSOR_H

#include <filesystem>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>

using namespace std;

class HtmlProcessor
{
public:
    void addHtml(filesystem::path HtmlPath);
    bool errorDetected;

private:
    map<string, vector<string>> HtmlList;
};

#endif