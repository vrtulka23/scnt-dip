#ifndef DIP_HIERARCHY_H
#define DIP_HIERARCHY_H

#include <string>
#include <vector>

#include "../nodes/node.h"

namespace dip {

  struct Parent {
    int indent;
    std::string name;
  };
  
  class HierarchyList {
  public:
    std::vector<Parent> parents;
    void record(std::shared_ptr<BaseNode> node, const std::vector<Node::NodeKeyword>& excluded);
  };
  
}

#endif // DIP_HIERARCHY_H
