#include "node_list.h"

namespace dip {

  size_t NodeList::size() const {
    return nodes.size();
  }
    
  void NodeList::push_front(std::shared_ptr<BaseNode> node) {
    nodes.push_front(node);
  }
  
  void NodeList::push_back(std::shared_ptr<BaseNode> node) {
    nodes.push_back(node);
  }
  
  std::shared_ptr<BaseNode> NodeList::pop_front() {
    std::shared_ptr<BaseNode> node = nodes.front();
    nodes.pop_front();
    return node;
  }
  
  std::shared_ptr<BaseNode> NodeList::pop_back() {
    std::shared_ptr<BaseNode> node = nodes.back();
    nodes.pop_back();
    return node;
  }
  
  std::shared_ptr<BaseNode> NodeList::operator[](const size_t index) {
    if (index<nodes.size())
      return nodes[index];
    else
      throw std::invalid_argument("Index "+std::to_string(index)+" exceded number of nodes: "+std::to_string(nodes.size()));
  }

}
