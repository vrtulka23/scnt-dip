#include <sstream>

#include "nodes.h"

namespace dip {

  BaseNode::BaseNode(Parser& parser) {
    line = parser.line;
    indent = parser.indent;
    name = parser.name;
    value_raw = parser.value_raw;
    value_shape = parser.value_shape;
    dtype = parser.dtype;
    dtype_raw = parser.dtype_raw;
    dtype_prop = parser.dtype_prop;
    dimension = parser.dimension;
    declared = parser.declared;
  }

  BaseNode::BaseNode(Parser& parser, const NodeDtype dt) {
    line = parser.line;
    indent = parser.indent;
    name = parser.name;
    value_raw = parser.value_raw;
    value_shape = parser.value_shape;
    dtype = dt;
    dtype_raw = parser.dtype_raw;
    dtype_prop = parser.dtype_prop;
    dimension = parser.dimension;
    declared = parser.declared;
  }

  BaseNode::NodeListType BaseNode::parse(Environment& env) {
    return {};
  }
  
}
