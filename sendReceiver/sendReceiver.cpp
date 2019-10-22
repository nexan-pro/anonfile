#include "sendReceiver.h"

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
    curl_easy_setopt(curl, CURLOPT_URL, "https://api.anonfile.com/upload");

#ifndef LINUX
    curl_easy_setopt(curl, CURLOPT_SSLCERTTYPE, "PEM");
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_CAINFO, NULL);
    curl_easy_setopt(curl, CURLOPT_CAPATH, NULL);
    curl_easy_setopt(curl, CURLOPT_FRESH_CONNECT, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_CTX_FUNCTION, *sslctx_function); // set void* and load cacert.pem from memory
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

CURLcode SendReceiver::sslctx_function(CURL* curl, void* sslctx, void* parm) {
  CURLcode rv = CURLE_ABORTED_BY_CALLBACK;

  /****************************** cacert.pem *********************************/
  static const char mypem[] =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIDSjCCAjKgAwIBAgIQRK+wgNajJ7qJMDmGLvhAazANBgkqhkiG9w0BAQUFADA/\n"
    "MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT\n"
    "DkRTVCBSb290IENBIFgzMB4XDTAwMDkzMDIxMTIxOVoXDTIxMDkzMDE0MDExNVow\n"
    "PzEkMCIGA1UEChMbRGlnaXRhbCBTaWduYXR1cmUgVHJ1c3QgQ28uMRcwFQYDVQQD\n"
    "Ew5EU1QgUm9vdCBDQSBYMzCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB\n"
    "AN+v6ZdQCINXtMxiZfaQguzH0yxrMMpb7NnDfcdAwRgUi+DoM3ZJKuM/IUmTrE4O\n"
    "rz5Iy2Xu/NMhD2XSKtkyj4zl93ewEnu1lcCJo6m67XMuegwGMoOifooUMM0RoOEq\n"
    "OLl5CjH9UL2AZd+3UWODyOKIYepLYYHsUmu5ouJLGiifSKOeDNoJjj4XLh7dIN9b\n"
    "xiqKqy69cK3FCxolkHRyxXtqqzTWMIn/5WgTe1QLyNau7Fqckh49ZLOMxt+/yUFw\n"
    "7BZy1SbsOFU5Q9D8/RhcQPGX69Wam40dutolucbY38EVAjqr2m7xPi71XAicPNaD\n"
    "aeQQmxkqtilX4+U9m5/wAl0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNV\n"
    "HQ8BAf8EBAMCAQYwHQYDVR0OBBYEFMSnsaR7LHH62+FLkHX/xBVghYkQMA0GCSqG\n"
    "SIb3DQEBBQUAA4IBAQCjGiybFwBcqR7uKGY3Or+Dxz9LwwmglSBd49lZRNI+DT69\n"
    "ikugdB/OEIKcdBodfpga3csTS7MgROSR6cz8faXbauX+5v3gTt23ADq1cEmv8uXr\n"
    "AvHRAosZy5Q6XkjEGB5YGV8eAlrwDPGxrancWYaLbumR9YbK+rlmM6pZW87ipxZz\n"
    "R8srzJmwN0jP41ZL9c8PDHIyh8bwRLtTcm1D9SZImlJnt1ir/md2cXjbDaJWFBM5\n"
    "JDGFoqgCWjBH4d1QB7wCCZAA62RjYJsWvIjJEubSfZGL+T0yjWW06XyxV3bqxbYo\n"
    "Ob8VZRzI9neWagqNdwvYkQsEjgfbKbYK7p2CNTUQ\n"
    "-----END CERTIFICATE-----\n";
  /***********************************************************************/

  BIO* cbio = BIO_new_mem_buf(mypem, sizeof(mypem));
  X509_STORE *cts = SSL_CTX_get_cert_store((SSL_CTX *) sslctx);
  X509_INFO *itmp;
  int i, count = 0;
  STACK_OF(X509_INFO) *inf;
  (void) curl;
  (void) parm;

  if (!cts || !cbio) {
    return rv;
  }

  inf = PEM_X509_INFO_read_bio(cbio, NULL, NULL, NULL);

  if (!inf) {
    BIO_free(cbio);
    return rv;
  }

  for (i = 0; i < sk_X509_INFO_num(inf); i++) {
    itmp = sk_X509_INFO_value(inf, i);
    if (itmp->x509) {
      X509_STORE_add_cert(cts, itmp->x509);
      count++;
    }
    if (itmp->crl) {
      X509_STORE_add_crl(cts, itmp->crl);
      count++;
    }
  }
}

std::string SendReceiver::sendInfoRequest(std::string& id) {
  std::string data;
  std::string link = "https://anonfile.com/api/v2/file/" + static_cast<std::string>(id) + "/info";
  CURL* curl = curl_easy_init();
  CURLcode res;
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, link.c_str());

#ifndef LINUX
    curl_easy_setopt(curl, CURLOPT_SSLCERTTYPE, "PEM");
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt (curl, CURLOPT_CAINFO, NULL);
    curl_easy_setopt(curl, CURLOPT_CAPATH, NULL);
    curl_easy_setopt(curl, CURLOPT_FRESH_CONNECT, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_CTX_FUNCTION, *sslctx_function); // set void* and load cacert.pem from memory
#endif

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
