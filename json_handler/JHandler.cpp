#include "JHandler.h"

JHandler::JHandler(const std::string &cfg) {
  status = false;
  pjson::document doc;
  try {
    if (doc.deserialize_in_place(const_cast<char*>(cfg.c_str()))) {
      if (doc.has_key("status") && doc.has_key("data")) {
        status = doc["status"].as_bool();
        url_ptr->str_full = doc["data"]["file"].get_object()[0].get_value()[0].as_string_ptr();
        url_ptr->str_short = doc["data"]["file"].get_object()[0].get_value()[1].as_string_ptr();
        MD_ptr->id = doc["data"]["file"].get_object()[1].get_value()[0].as_string_ptr();
        MD_ptr->name = doc["data"]["file"].get_object()[1].get_value()[1].as_string_ptr();
        MD_ptr->size = doc["data"]["file"]["metadata"]["size"].get_object()[1].get_value().as_string_ptr();

#ifdef DEBUG
        std::cout << "[DEBUG|CFG] : status is: " << status << std::endl;
        std::cout << "[DEBUG|CFG] : url full is: " << url_ptr->str_full << std::endl;
        std::cout << "[DEBUG|CFG] : url short is: " << url_ptr->str_short << std::endl;
        std::cout << "[DEBUG|CFG] MD id is: " << MD_ptr->id << std::endl;
        std::cout << "[DEBUG|CFG] MD name is: " << MD_ptr->name << std::endl;
        std::cout << "[DEBUG|CFG] MD size is: " << MD_ptr->size << std::endl;
#endif

      } else if(doc.has_key("error")) {
        error_msg = doc["error"].get_object()[0].get_value().as_string_ptr();
#ifdef DEBUG
        std::cout << "[DEBUG|CFG] error_msg : " << error_msg << std::endl;
#endif
      }
      else
        throw std::string("Error, bad JSON format, key [status] not found.");
    } else
      throw std::string("Error, bad data. [doc.deserialize_in_place()]");
  }
  catch(const std::string& e) {
    std::cerr << e << std::endl;
  }
}