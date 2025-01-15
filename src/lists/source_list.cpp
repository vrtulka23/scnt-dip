#include <stdexcept>

#import "source_list.h"

namespace dip {

  SourceList::SourceList() {
  }

  void SourceList::append(const std::string name, const std::string path, const std::string code, const Source parent) {
    if (sources.contains(name))
      throw std::invalid_argument("Source with the same name already exists: "+name);
    sources.insert({name, {name, path, code, parent}});
  }

  EnvSource& SourceList::operator[](const std::string name) {
    return sources.at(name);
  }
  
}
