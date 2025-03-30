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
  
  void FormatNode::set_property(Environment& env, ValueNode::PointerType vnode) {
    if (vnode->dtype != NodeDtype::String)
      throw std::runtime_error("Format can be set only to string node. Previous node is: "+vnode->line.code);
    vnode->format = value_raw.at(0);
  }
   
}
