#include <stdio.h>
#include <curl/curl.h>
#include <iostream>
#include <cstring>
#include "sendReceiver/sendReceiver.h"

int main(int argc, char** argv) {
  std::string contents, data;
  char *filename = new char[256];
  if (argc == 3 && strncmp(argv[1], "-i", 2) == 0) {
    std::string id = static_cast<std::string>(argv[2]);
    SendReceiver ctrl;
    ctrl.getInfoReq(id);

  }
  else if (argc == 2) {
    strcpy(filename, argv[1]);
    SendReceiver ctrl;
    ctrl.uploadFile(filename);
  }
  else if (argc == 1) {
    std::cout << "Enter filename: ";
    std::cin.getline(filename, 256);
    SendReceiver ctrl;
    ctrl.uploadFile(filename);
  }
  else {
    std::cerr << "Error too many arguments provided, usage: anonfile <your_path_to_file>\n"
              << "File path must be without spaces.\n";
    return -1;
  }


  std::cout << "Press ENTER to exit.\n";
  std::cin.get();
  return 0;
}