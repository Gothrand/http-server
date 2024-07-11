#ifndef UTILS_H
#define UTILS_H
#include <string>
#include <vector>

/*
 * @brief Splits a string into a vector of strings, on the given delimiter (default is single space ' ')
 *
 * @param str: string to split
 * @param delim: delimiter to split on. Default is `' '`
 *
 * @returns a vector of strings, the strings being the words of the original string split on the delimiter
 */
std::vector<std::string> split(const std::string &str, const char delim = ' ')
{
    std::vector<std::string> result;
    if (str == "")
    {
        return result;
    }

    std::string temp = "";
    for (const char c : str)
    {
        if (c == delim)
        {
            if (temp != "")
            {
                result.push_back(temp);
                temp = "";
            }
        }
        else
        {
            temp += c;
        }
    }

    if (temp != "")
    {
        result.push_back(temp);
    }

    return result;
}

void print_raw(const std::string &str)
{
    for (const char c : str)
    {
        if (c == ' ')
        {
            std::cout << "\\s";
        }
        else if (c == '\n')
        {
            std::cout << "\\n";
        }
        else if (c == '\r')
        {
            std::cout << "\\r";
        }
        else
        {
            std::cout << c;
        }
    }
    std::cout << '\n';
    return;
}

std::string get_content_from_file(const std::string &file_name)
{
    std::ifstream fin(file_name);
    std::string result;
    std::string temp;
    if (fin)
    {
        while (getline(fin, temp))
        {
            result += temp;
            result += '\n';
        }
    }
    result += '\n';

    // clean up duplicate newline characters
    std::string final_result = "";
    char prev = -1;
    for (const char c : result)
    {
        if (prev == '\n' && c == '\n')
        {
            continue;
        }
        final_result += c;
        prev = c;
    }

    return final_result;
}

#endif