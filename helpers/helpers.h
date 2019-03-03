#ifndef ANONFILE_HELPERS_H
#define ANONFILE_HELPERS_H

#include <string>
#include <fstream>
#include "../json_handler/JHandler.h"

class Helpers : protected JHandler {
public:
  Helpers(std::string cfg) : JHandler(cfg) {}

  Helpers() = default;
  static int get_current_length(char* str);

  static bool contains(char* &tmp);

  static char* reverse(char*& str);

  static std::string readFromFile(std::ifstream& _in);

  void printRequestData();
};
#endif //ANONFILE_HELPERS_H
