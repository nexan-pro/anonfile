#include <stdio.h>
#include <curl/curl.h>
#include <iostream>
#include <cstring>
#include "helpers/helpers.h"
#include "json_handler/JHandler.h"

#define LINUX
//#define DEBUG

size_t writeCallback(char* buf, size_t size, size_t nmemb, void* up) {
  ((std::string*)up)->append(buf, size * nmemb);
  return size * nmemb;
}

int main(int argc, char** argv) {
  std::string contents, data;
  char *filename = new char[256];
  if (argc == 2)
    strcpy(filename, argv[1]);
  else if (argc == 1) {
    std::cout << "Enter filename: ";
    std::cin.getline(filename, 256);
  }
  else {
    std::cerr << "Error too many arguments provided, usage: anonfile <your_path_to_file>\n"
              << "File path must be without spaces.\n";
    return -1;
  }

  int ecx = Helpers::get_current_length(filename), i = 0;
  char *formattedFilename = new char[ecx];

  if (Helpers::contains(filename)) {
    while (filename[ecx] != '\\') {
      ecx--;
      if (ecx < 0) break;
      if (filename[ecx] == '/') break;
      formattedFilename[i] = filename[ecx];
      i++;
    }
    Helpers::reverse(formattedFilename);

#ifdef DEBUG
    std::cout << "[DEBUG] : formatted filename is: " << formattedFilename << std::endl;
#endif

  } else
    for (int c = 0; c <= ecx; c++)
      formattedFilename[c] = filename[c];
  std::cout << "filename is: " << filename << std::endl;

  std::ifstream in(filename, std::ios::in | std::ios::binary);

  contents = Helpers::readFromFile(in);

  CURL* curl;
  CURLcode res;

  struct curl_httppost* formpost = NULL;
  struct curl_httppost* lastptr = NULL;
  struct curl_slist* headerlist = NULL;

  curl_global_init(CURL_GLOBAL_ALL);

// set up the header
  curl_formadd(&formpost,
               &lastptr,
               CURLFORM_COPYNAME, "cache-control:",
               CURLFORM_COPYCONTENTS, "no-cache",
               CURLFORM_END);

  curl_formadd(&formpost,
               &lastptr,
               CURLFORM_COPYNAME, "content-type:",
               CURLFORM_COPYCONTENTS, "multipart/form-data",
               CURLFORM_END);

  curl_formadd(&formpost, &lastptr,
               CURLFORM_COPYNAME, "file",  // the (in this case) wanted file-Tag!
               CURLFORM_BUFFER, formattedFilename,
               CURLFORM_BUFFERPTR, contents.data(),
               CURLFORM_BUFFERLENGTH, contents.size(),
               CURLFORM_END);

  curl = curl_easy_init();
  headerlist = curl_slist_append(headerlist, "pragma:");
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, "https://anonfile.com/api/upload");

#ifndef LINUX
    curl_easy_setopt (curl, CURLOPT_CAINFO, "cacert.pem"); // set path to SSL CA cert issue "cacert.pem"
#endif

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
    curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);

    res = curl_easy_perform(curl);

    /* Check for errors */
    if (res != CURLE_OK)
      std::cerr <<  "Error, check you internet connection [" << curl_easy_strerror(res)<< "]\n";

#ifdef DEBUG
    std::cout << std::endl << "[DEBUG|DATA] : " << data << std::endl;
#endif

    curl_easy_cleanup(curl);
    curl_formfree(formpost);
    curl_slist_free_all(headerlist);
  }
  curl_global_cleanup();

  delete[] filename;
  delete[] formattedFilename;

  Helpers handler(data);
  handler.printRequestData();

  std::cout << "Press ENTER to exit.\n";
  std::cin.get();
  return 0;
}