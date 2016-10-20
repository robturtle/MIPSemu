#ifndef MIPSEMU_STRINGS_H
#define MIPSEMU_STRINGS_H

#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>

// the standard library is lack of modern useful string utilities
// and I don't want add 3rd party dependencies on this project

// credit: http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
namespace strings {

  inline std::string &ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                                    std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
  }

  inline std::string &rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
                         std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
  }

  inline std::string &trim(std::string &s) {
    return ltrim(rtrim(s));
  }

}

#endif //MIPSEMU_STRINGS_H
