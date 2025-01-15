#ifndef DIP_SOURCES_H
#define DIP_SOURCES_H

#include <map>

#include "../settings.h"
#include "node_list.h"

namespace dip {

  struct EnvSource {
    std::string name;
    std::string path;
    std::string code;
    Source parent;
    NodeList nodes;
    //SourceList* sources = NULL;
  };
  
  class SourceList {
  private:
    std::map<std::string,EnvSource> sources;
  public:
    SourceList();
    void append(const std::string name, const std::string path, const std::string code, const Source parent);
    EnvSource& operator[](const std::string name);
  };

}
  
#endif // DIP_SOURCES_H
