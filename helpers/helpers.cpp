#include "helpers.h"

int Helpers::get_current_length(char* str) {
  int i = 0;
  while (str[i] != '\0')
    i++;
  return i;
}

bool Helpers::contains(char*& tmp) {
  for (size_t i = 0; i < get_current_length(tmp); i++)
    if (tmp[i] == '\\' || tmp[i] == '/')
      return true;
  return false;
}

char* Helpers::reverse(char*& str) {
  int ecx = get_current_length(str) - 1;
  char tmp;
  for (int i = 0; i < ecx; i++, ecx--) {
    tmp = str[i];
    str[i] = str[ecx];
    str[ecx] = tmp;
  }
  return str;
}

std::string Helpers::readFromFile(std::ifstream& _in) {
  std::string tmp;
  if (_in) {
    _in.seekg(0, std::ios::end);
    tmp.resize(_in.tellg());
    _in.seekg(0, std::ios::beg);
    _in.read(&tmp[0], tmp.size());
    printf("Size of input file: %d\n", tmp.size());
    _in.close();
  } else
    throw std::runtime_error("Error, unable to open file!");
  return tmp;
}

void Helpers::printRequestData() {
  if (status) {
    std::cout << "File was uploaded successfully!\n"
              << "Full url to file: " << url_ptr->str_full
              << "\nShort url to file: " << url_ptr->str_short
              << "\nFile id: " << MD_ptr->id
              << "\nFilename: " << MD_ptr->name
              << "\nSize of file: " << MD_ptr->size << '\n';
  } else
    std::cerr << "Error: " << error_msg << std::endl;
}