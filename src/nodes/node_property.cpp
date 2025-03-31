#include "nodes.h"
#include "../environment.h"

namespace dip {

  static constexpr std::array<NodeDtype,4> nodes_value = {
    NodeDtype::Boolean,NodeDtype::Integer,NodeDtype::Float,NodeDtype::String
  };
  
  BaseNode::NodeListType PropertyNode::parse(Environment& env) {
    if (std::find(nodes_value.begin(), nodes_value.end(), env.previous_node) == nodes_value.end())
      throw std::runtime_error("Only value nodes (bool, int, float and str) can have properties: "+line.code);
    BaseNode::PointerType node = env.nodes.at(env.nodes.size()-1);
    if (node->indent>=indent)
      throw std::runtime_error("The indent '"+std::to_string(indent)+"' of a property is not higher than the indent '"+std::to_string(node->indent)+"' of a preceding node: "+line.code);
    ValueNode::PointerType vnode = std::dynamic_pointer_cast<ValueNode>(node);
    if (vnode)
      set_property(env, vnode);
    else
      throw std::runtime_error("Only value nodes (bool, int, float and str) can have properties: "+line.code);
    return {};
  }  
  
}
