#include "HtmlProcessor.h"

void HtmlProcessor::addHtml(filesystem::path HtmlPath)
{

    vector<string> words;
    ifstream html;
    this->errorDetected = false;

    html.open(HtmlPath, std::ios::in);
    if (!html.is_open())
    {
        std::cout << "error opening " << HtmlPath.filename() << std::endl;
        this->errorDetected = true;
        return;
    }

    while (!html.eof())
    {

        if (html.peek() == '<')
        {
            html.ignore(std::numeric_limits<std::streamsize>::max(), '>');
        }
        else
        {
            string word;
            while (html.peek() != ' ' && html.peek() != '<' && !html.eof())
            {
                word.push_back(html.get());
            }

            if (html.peek() == ' ')
            {
                html.ignore(1);
            }

            if (!word.empty())
            {
                words.push_back(word);
            }
        }
    }

    this->HtmlList[HtmlPath.filename()] = words;

    html.close();
}