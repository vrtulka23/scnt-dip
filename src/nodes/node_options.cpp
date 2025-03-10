#include <array>

#include "nodes.h"
#include "../environment.h"

namespace dip {

  std::shared_ptr<BaseNode> OptionsNode::is_node(Parser& parser) {
    parser.part_equal();
    if (parser.is_parsed(Parser::PART_EQUAL)) {
      parser.part_value();
      parser.part_units();
      parser.part_comment();
      return std::make_shared<OptionsNode>(parser);
    }
    parser.kwd_options();
    if (parser.is_parsed(Parser::KWD_OPTIONS)) {
      parser.part_value();
      parser.part_comment();
      return std::make_shared<OptionsNode>(parser);
    }
    return nullptr;
  }
  
  BaseNode::NodeListType OptionsNode::parse(Environment& env) {
    if (env.nodes.size()==0)
      throw std::runtime_error("Could not find a node that can have options: "+line.code);
    std::shared_ptr<BaseNode> node = env.nodes[env.nodes.size()-1];
    std::shared_ptr<ValueNode> vnode = std::dynamic_pointer_cast<ValueNode>(node);
    if (vnode) {
      if (vnode->indent>=indent)
	throw std::runtime_error("The indent '"+std::to_string(indent)+"' of a property is not higher than the indent '"+std::to_string(vnode->indent)+"' of a preceding node: "+line.code);
      if (dimension.size()>0) {
	for (auto value_option: value_raw)
	  vnode->set_option(value_option, units_raw, env); 
      } else {
	vnode->set_option(value_raw[0], units_raw, env);
      }
    } else {
      throw std::runtime_error("Only value nodes (bool, int, float and str) can have properties. Previous node is: "+node->line.code);
    }
    return {};
  }  
 
}
