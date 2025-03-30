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
  
  void DescriptionNode::set_property(Environment& env, ValueNode::PointerType vnode) {
    vnode->description += value_raw.at(0);
  }
   
}
