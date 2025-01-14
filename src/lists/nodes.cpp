#include "nodes.h"

namespace dip {
  
  void NodeList::append(std::shared_ptr<BaseNode> node) {
    nodes.push_back(node);
  }

}
