#ifndef ANONFILE_JHANDLER_H
#define ANONFILE_JHANDLER_H
//#define DEBUG

#include <string>
#include <iostream>
#include <memory>
#include "../pjson/pjson.h"

class JHandler {
protected:

  struct url {
    std::string str_full;
    std::string str_short;
  };

  struct metadata {
    std::string id;
    std::string name;
    std::string size;
  };

  std::shared_ptr<url> url_ptr = std::make_shared<url>();
  std::shared_ptr<metadata> MD_ptr = std::make_shared<metadata>();
  std::string error_msg;
  bool status;

public:
 explicit JHandler(const std::string& cfg);
 JHandler() = default;
};


#endif //ANONFILE_JHANDLER_H
