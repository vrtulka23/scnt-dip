#include "lists.h"

namespace dip {

  void FunctionList::append(const std::string& name, FunctionType func) {
    auto result = functions.insert({name, func});
    if (!result.second)
      throw std::runtime_error("Function with the following name already exists: "+name);
  }

  FunctionList::FunctionType FunctionList::operator[](const std::string& name) const {
    auto it = functions.find(name);
    if (it == functions.end())
      throw std::runtime_error("Function with the following name does not exists: "+name);      
    else
      return it->second;
  }
  
}
