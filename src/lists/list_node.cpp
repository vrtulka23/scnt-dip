#include "lists.h"

namespace dip {

  size_t NodeList::size() const {
    return nodes.size();
  }
    
  void NodeList::push_front(BaseNode::PointerType node) {
    nodes.push_front(node);
  }
  
  void NodeList::push_back(BaseNode::PointerType node) {
    nodes.push_back(node);
  }
  
  BaseNode::PointerType NodeList::pop_front() {
    BaseNode::PointerType node = nodes.front();
    nodes.pop_front();
    return node;
  }
  
  BaseNode::PointerType NodeList::pop_back() {
    BaseNode::PointerType node = nodes.back();
    nodes.pop_back();
    return node;
  }
  
  BaseNode::PointerType NodeList::operator[](const size_t index) {
    if (index<nodes.size())
      return nodes[index];
    else
      throw std::invalid_argument("Index "+std::to_string(index)+" exceded number of nodes: "+std::to_string(nodes.size()));
  }
  
  BaseNode::PointerType NodeList::operator[](const size_t index) const {
    if (index<nodes.size())
      return nodes[index];
    else
      throw std::invalid_argument("Index "+std::to_string(index)+" exceded number of nodes: "+std::to_string(nodes.size()));
  }
  
}
