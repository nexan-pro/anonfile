#ifndef ANONFILE_HELPERS_H
#define ANONFILE_HELPERS_H

#include <string>
#include <fstream>
#include "../json_handler/JHandler.h"

class Helpers {
public:
  Helpers() = default;

protected:
  static int get_current_length(char* str);
  static bool contains(char*& tmp);
  static char* reverse(char*& str);
  static std::string readFromFile(std::ifstream& _in);
};
#endif //ANONFILE_HELPERS_H
