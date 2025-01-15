#ifndef DIP_NODES_H
#define DIP_NODES_H

#include <memory>

#include "../settings.h"
#include "../nodes/node.h"

namespace dip {

  class NodeList {
  private:
    BaseNode::NodeListType nodes;
  public:
    size_t size() const;
    void push_front(std::shared_ptr<BaseNode> node);
    void push_back(std::shared_ptr<BaseNode> node);
    std::shared_ptr<BaseNode> pop_front();
    std::shared_ptr<BaseNode> pop_back();
    std::shared_ptr<BaseNode> operator[](const size_t index);
  };
  
}

#endif // DIP_NODES_H
