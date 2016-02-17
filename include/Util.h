#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <iterator>
#include <iostream>

namespace Util {
    std::vector<std::string>& split(const std::string &s, char delim, std::vector<std::string> &elems);

    std::vector<std::string> split(const std::string &s, char delim);

    void trim(std::string& s);

    template<typename T>
	std::string to_string(std::vector<T> &vec) {
	    std::ostringstream oss;

	    // Convert all but the last element to avoid a trailing ","
	    std::copy(vec.begin(), vec.end()-1,
	        std::ostream_iterator<T>(oss, ","));

	    // Now add the last element with no delimiter
	    oss << vec.back();

	    return oss.str();
	};
}