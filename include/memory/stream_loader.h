#ifndef MIPSEMU_STREAM_LOADER_H
#define MIPSEMU_STREAM_LOADER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include "Memory.h"
#include "../util/strings.h"

namespace mips {

  std::istream& operator>> (std::istream& in, Memory& memory) {
    while (!in.eof()) {
      std::string line;
      while (in.good() && !in.eof() && line.empty()) getline(in, line);
      strings::trim(line);
      if (in.bad()) throw std::invalid_argument(line);
      if (line.empty()) break;

      try {
        memory.memory.push_back(byte_t(line, 0, BYTE_SIZE));

      } catch (std::invalid_argument e) {
        throw std::invalid_argument(line);
      }
    }
    return in;
  }

  namespace memory {

    void loadFromFile(std::string fname, Memory &memory) {
      std::ifstream in(fname);
      if (in.bad() || !in.is_open()) throw std::invalid_argument((std::ostringstream()
          << "can't open file " << fname).str());

      try {
        in >> memory;
      } catch (std::invalid_argument e) {
        throw std::invalid_argument((std::ostringstream()
          << "bad format in file " << fname << " at line '" << e.what() << "'").str());
      }

    }

  }

}

#endif //MIPSEMU_STREAM_LOADER_H
