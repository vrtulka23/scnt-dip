#ifndef DIP_NODES_H
#define DIP_NODES_H

#include <memory>
#include <vector>

#include "../settings.h"
#include "../nodes/node.h"

namespace dip {

  class NodeList {
  private:
    std::vector<std::shared_ptr<BaseNode>> nodes;
  public:
    void append(std::shared_ptr<BaseNode> node);
  };
  
}

#endif // DIP_NODES_H
