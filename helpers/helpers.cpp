#include "helpers.h"


std::string Helpers::readFromFile(std::ifstream& _in) {
  std::string tmp;
  if (_in) {
    _in.seekg(0, std::ios::end);
    tmp.resize(_in.tellg());
    _in.seekg(0, std::ios::beg);
    _in.read(&tmp[0], tmp.size());

    std::cout << "Size of input file: " << tmp.size() << std::endl;
    
    _in.close();
  } else
    throw std::runtime_error("Error, unable to open file!");
  return tmp;
}
