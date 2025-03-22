#ifndef DIP_ENVIRONMENT_H
#define DIP_ENVIRONMENT_H

#include "lists/lists.h"

namespace dip {

  class Environment {
  private:
  public:
    enum RequestType {
      FUNCTION, REFERENCE
    };
    SourceList sources;
    NodeList nodes;
    HierarchyList hierarchy;
    BranchingList branching;
    FunctionList functions;
    Environment();
    std::string request_code(const std::string& source_name) const;
    BaseValue::PointerType request_value(const std::string& request, const RequestType rtype, const std::string& in_units="") const;
    BaseNode::NodeListType request_nodes(const std::string& request, const RequestType rtype, const std::string& in_units="") const;
  };

}

#endif
