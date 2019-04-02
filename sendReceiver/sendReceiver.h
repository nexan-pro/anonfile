#ifndef ANONFILE_SENDER_H
#define ANONFILE_SENDER_H

#define LINUX
//#define DEBUG

#include "../helpers/helpers.h"
#include "../json_handler/JHandler.h"
#include "curl/curl.h"
#include "openssl/ssl.h"
#include "openssl/err.h"

class SendReceiver : protected Helpers, protected JHandler {
private:
  static size_t writeCallback(char* buf, size_t size, size_t nmemb, void* up);
  static CURLcode sslctx_function(CURL *curl, void *sslctx, void *parm);
  std::string sendInfoRequest(std::string& id);
  void printRequestData();
public:
  explicit SendReceiver(std::string cfg) : JHandler(cfg) { }
  SendReceiver() = default;
  bool uploadFile(char*& pathToFile);
  bool getInfoReq(std::string& id); //TODO:
};


#endif //ANONFILE_SENDER_H
