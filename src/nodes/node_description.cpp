#include <array>

#include "nodes.h"
#include "../environment.h"

namespace dip {

  BaseNode::PointerType DescriptionNode::is_node(Parser& parser) {
    if (parser.kwd_description()) {
      parser.part_value();
      parser.part_comment();
      return std::make_shared<DescriptionNode>(parser);
    }
    return nullptr;
  }
  
  BaseNode::NodeListType DescriptionNode::parse(Environment& env) {
    if (env.nodes.size()==0)
      throw std::runtime_error("Could not find a node that can have a description: "+line.code);
    BaseNode::PointerType node = env.nodes[env.nodes.size()-1];
    ValueNode::PointerType vnode = std::dynamic_pointer_cast<ValueNode>(node);
    if (vnode) {
      if (vnode->indent>=indent)
	throw std::runtime_error("The indent '"+std::to_string(indent)+"' of a property is not higher than the indent '"+std::to_string(vnode->indent)+"' of a preceding node: "+line.code);
      vnode->description += value_raw[0];
    } else {
      throw std::runtime_error("Only value nodes (bool, int, float and str) can have properties. Previous node is: "+node->line.code);
    }
    return {};
  }  
 
}
