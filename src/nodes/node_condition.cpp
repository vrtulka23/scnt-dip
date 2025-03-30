#include <array>

#include "nodes.h"
#include "../environment.h"

namespace dip {

  BaseNode::PointerType ConditionNode::is_node(Parser& parser) {
    if (parser.kwd_condition()) {
      parser.part_value();
      parser.part_comment();
      return std::make_shared<ConditionNode>(parser);
    }
    return nullptr;
  }
  
  void ConditionNode::set_property(Environment& env, ValueNode::PointerType vnode) {
    vnode->condition = value_raw.at(0);
  }
   
}
