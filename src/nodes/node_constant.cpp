#include "nodes.h"
#include "../environment.h"

namespace dip {

  BaseNode::PointerType ConstantNode::is_node(Parser& parser) {
    if (parser.kwd_constant()) {
      return std::make_shared<ConstantNode>(parser);
    }
    return nullptr;
  }

  void ConstantNode::set_property(Environment& env, ValueNode::PointerType vnode) {
    vnode->constant = true;
  }
    
}
