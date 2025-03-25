#include <array>

#include "nodes.h"
#include "../environment.h"

namespace dip {

  BaseNode::PointerType OptionsNode::is_node(Parser& parser) {
    if (parser.part_equal(false)) {
      parser.part_value();
      parser.part_units();
      parser.part_comment();
      return std::make_shared<OptionsNode>(parser);
    }
    if (parser.kwd_options()) {
      parser.part_value();
      parser.part_units();
      parser.part_comment();
      return std::make_shared<OptionsNode>(parser);
    }
    return nullptr;
  }
  
  BaseNode::NodeListType OptionsNode::parse(Environment& env) {
    if (env.nodes.size()==0)
      throw std::runtime_error("Could not find a node that can have options: "+line.code);
    BaseNode::PointerType node = env.nodes.at(env.nodes.size()-1);
    ValueNode::PointerType vnode = std::dynamic_pointer_cast<ValueNode>(node);
    if (vnode) {
      if (vnode->indent>=indent)
	throw std::runtime_error("The indent '"+std::to_string(indent)+"' of a property is not higher than the indent '"+std::to_string(vnode->indent)+"' of a preceding node: "+line.code);
      if (dimension.empty()) {
	vnode->set_option(value_raw.at(0), units_raw, env);
      } else {
	for (auto value_option: value_raw)
	  vnode->set_option(value_option, units_raw, env); 
      }
    } else {
      throw std::runtime_error("Only value nodes (bool, int, float and str) can have properties. Previous node is: "+node->line.code);
    }
    return {};
  }  
 
}
