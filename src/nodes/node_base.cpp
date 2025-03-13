#include <sstream>

#include "nodes.h"

namespace dip {
  
  BaseNode::BaseNode(Parser& parser) {
    line = parser.line;
    indent = parser.indent;
    name = parser.name;
    value_raw = parser.value_raw;
    value_shape = parser.value_shape;
    value_func = parser.value_func;
    value_expr = parser.value_expr;
    dtype = parser.dtype;
    dtype_raw = parser.dtype_raw;
    dtype_prop = parser.dtype_prop;
    dimension = parser.dimension;
  }

  BaseNode::BaseNode(Parser& parser, const NodeDtype dt) {
    line = parser.line;
    indent = parser.indent;
    name = parser.name;
    value_raw = parser.value_raw;
    value_shape = parser.value_shape;
    value_func = parser.value_func;
    value_expr = parser.value_expr;
    dtype = dt;
    dtype_raw = parser.dtype_raw;
    dtype_prop = parser.dtype_prop;
    dimension = parser.dimension;
  }

  BaseNode::NodeListType BaseNode::parse(Environment& env) {
    return {};
  }
  
}
