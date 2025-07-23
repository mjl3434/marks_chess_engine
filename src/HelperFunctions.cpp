#include "HelperFunctions.h"

#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <string>

// It seems dumb that C++ doesn't have a standard library function which does this (as of Dec 2024)
void
toLower(std::string& str)
{
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) {
        return std::tolower(c);
    });
}

bool
stringToInt(std::string& input, int& output)
{
    int converted_value = 0;

    for (char c : input) {
        if (!std::isdigit(static_cast<unsigned char>(c))) {
            return false;
        }
    }

    try {
        converted_value = std::stoi(input);
    } catch (const std::out_of_range&) {
        return false; // If the number is out of range for an integer
    }

    output = converted_value;
    return true;
}

std::string
popFrontLowercase(std::list<std::string>& tokens)
{
    std::string token = tokens.front();
    tokens.pop_front();
    toLower(token);
    return token;
}

std::string
atFrontLowercase(std::list<std::string>& tokens)
{
    std::string token = tokens.front();
    toLower(token);
    return token;
}
