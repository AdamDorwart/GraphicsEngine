#include "Util.h"
#include <algorithm>

// Split function
// http://stackoverflow.com/questions/236129/split-a-string-in-c
std::vector<std::string>& Util::split(const std::string &s, char delim, std::vector<std::string> &elems)
{
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim))
    {
    	if (item != "") {
    		elems.push_back(item);
    	}
    }
    return elems;
}

std::vector<std::string> Util::split(const std::string &s, char delim)
{
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

// Trim function
// http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring?page=1&tab=votes#tab-top
void Util::trim(std::string& s) {
     s.erase(s.begin(), std::find_if_not(s.begin(), s.end(), [](char c){ return std::isspace(c); }));
     s.erase(std::find_if_not(s.rbegin(), s.rend(), [](char c){ return std::isspace(c); }).base(), s.end());
}