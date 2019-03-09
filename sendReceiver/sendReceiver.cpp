#include "sendReceiver.h"
#include "curl/curl.h"

size_t SendReceiver::writeCallback(char* buf, size_t size, size_t nmemb, void* up) {
  ((std::string*)up)->append(buf, size * nmemb);
  return size * nmemb;
}

bool SendReceiver::uploadFile(char*& pathToFile) {
  std::string data, contents;
  int ecx = Helpers::get_current_length(pathToFile), i = 0;
  char *formattedFilename = new char[ecx];

  if (Helpers::contains(pathToFile)) {
    while (pathToFile[ecx] != '\\') {
      ecx--;
      if (ecx < 0) break;
      if (pathToFile[ecx] == '/') break;
      formattedFilename[i] = pathToFile[ecx];
      i++;
    }
    Helpers::reverse(formattedFilename);

#ifdef DEBUG
    std::cout << "[DEBUG] : formatted filename is: " << formattedFilename << std::endl;
#endif

  } else
    for (int c = 0; c <= ecx; c++)
      formattedFilename[c] = pathToFile[c];
  std::cout << "pathToFile is: " << pathToFile << std::endl;

  std::ifstream in(pathToFile, std::ios::in | std::ios::binary);

  contents = Helpers::readFromFile(in);

  CURL* curl;
  CURLcode res;

  struct curl_httppost* formpost = nullptr;
  struct curl_httppost* lastptr = nullptr;
  struct curl_slist* headerlist = nullptr;

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
    if (res != CURLE_OK) {
      std::cerr << "Error, check you internet connection [" << curl_easy_strerror(res) << "]\n";
      return false;
    }

#ifdef DEBUG
    std::cout << std::endl << "[DEBUG|DATA] : " << data << std::endl;
#endif

    curl_easy_cleanup(curl);
    curl_formfree(formpost);
    curl_slist_free_all(headerlist);
  }
  curl_global_cleanup();

  delete[] pathToFile;
  delete[] formattedFilename;

  SendReceiver handler(data);
  handler.printRequestData();
  return status;
}

std::string SendReceiver::sendInfoRequest(std::string& id) {
  std::string data;
  std::string link = "https://anonfile.com/api/v2/file/" + static_cast<std::string>(id) + "/info";
  CURL* curl = curl_easy_init();
  CURLcode res;
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, link.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);

    res = curl_easy_perform(curl);
    if (res != CURLE_OK)
      std::cout << "Error, check your internet connection!\n";
  }
  curl_easy_cleanup(curl);
  return data;
}

bool SendReceiver::getInfoReq(std::string& id) {
  try {
    (id.empty() || id.size() == 0) ? throw std::string("Error, NULLPTR exception!") : 0;
    std::string request_data = sendInfoRequest(id);
#ifdef DEBUG
    std::cout << "[DEBUG|getInfoReq] : request_data is: " << request_data << std::endl;
#endif
    SendReceiver handler(request_data);
    handler.printRequestData();
  } catch (const std::string str) {
    std::cout << str << std::endl;
  }
  return true;
}

void SendReceiver::printRequestData() {
  if (status) {
    std::cout << "Full url to file: " << url_ptr->str_full
              << "\nShort url to file: " << url_ptr->str_short
              << "\nFile id: " << MD_ptr->id
              << "\nFilename: " << MD_ptr->name
              << "\nSize of file: " << MD_ptr->size << '\n';
  } else
    std::cerr << "Error: " << error_msg << std::endl;
}