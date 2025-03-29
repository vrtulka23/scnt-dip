#include <array>

#include "nodes.h"
#include "../environment.h"

namespace dip {

  BaseNode::PointerType FormatNode::is_node(Parser& parser) {
    if (parser.kwd_format()) {
      parser.part_value();
      parser.part_comment();
      return std::make_shared<FormatNode>(parser);
    }
    return nullptr;
  }
  
  BaseNode::NodeListType FormatNode::parse(Environment& env) {
    if (env.nodes.size()==0)
      throw std::runtime_error("Could not find a string node that can have a format: "+line.code);
    BaseNode::PointerType node = env.nodes.at(env.nodes.size()-1);
    ValueNode::PointerType vnode = std::dynamic_pointer_cast<ValueNode>(node);
    if (vnode) {
      if (vnode->indent>=indent)
	throw std::runtime_error("The indent '"+std::to_string(indent)+"' of a property is not higher than the indent '"+std::to_string(vnode->indent)+"' of a preceding node: "+line.code);
      if (vnode->dtype != NodeDtype::String)
	throw std::runtime_error("Format can be set only to string node. Previous node is: "+vnode->line.code);
      vnode->format = value_raw.at(0);
    } else {
      throw std::runtime_error("Only value nodes (bool, int, float and str) can have properties. Previous node is: "+node->line.code);
    }
    return {};
  }  
 
}
