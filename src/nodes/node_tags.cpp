#include <array>

#include "nodes.h"
#include "../environment.h"

namespace dip {

  BaseNode::PointerType TagsNode::is_node(Parser& parser) {
    if (parser.kwd_tags()) {
      parser.part_value();
      parser.part_comment();
      return std::make_shared<TagsNode>(parser);
    }
    return nullptr;
  }
  
  void TagsNode::set_property(Environment& env, ValueNode::PointerType vnode) {
    vnode->tags = value_raw;
  }
   
}
