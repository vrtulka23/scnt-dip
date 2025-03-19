#include <sstream>

#include "nodes.h"

namespace dip {

  BaseNode::BaseNode(Parser& parser, const NodeDtype dt): dtype(dt) {
    line = parser.line;
    indent = parser.indent;
    name = parser.name;
    value_raw = parser.value_raw;
    value_shape = parser.value_shape;
    value_func = parser.value_func;
    value_expr = parser.value_expr;
    value_ref = parser.value_ref;
    dtype_raw = parser.dtype_raw;
    dimension = parser.dimension;
  }
  
  BaseNode::NodeListType BaseNode::parse(Environment& env) {
    return {};
  }
  
}
