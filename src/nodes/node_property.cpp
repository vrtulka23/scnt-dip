#include "nodes.h"
#include "../environment.h"

namespace dip {
  
  BaseNode::NodeListType PropertyNode::parse(Environment& env) {
    if (env.nodes.size()==0)
      throw std::runtime_error("Could not find a node that can have the following property: "+line.code);
    BaseNode::PointerType node = env.nodes.at(env.nodes.size()-1);
    ValueNode::PointerType vnode = std::dynamic_pointer_cast<ValueNode>(node);
    if (vnode) {
      if (vnode->indent>=indent)
	throw std::runtime_error("The indent '"+std::to_string(indent)+"' of a property is not higher than the indent '"+std::to_string(node->indent)+"' of a preceding node: "+line.code);
      set_property(env, vnode);
    } else {
      throw std::runtime_error("Only value nodes (bool, int, float and str) can have properties. Previous node is: "+node->line.code);
    }
    return {};
  }  
  
}
