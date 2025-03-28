#include <sstream>

#include "nodes.h"

namespace dip {

  BaseNode::BaseNode(Parser& parser, const NodeDtype dt): dtype(dt) {
    line = parser.line;
    indent = parser.indent;
    name = parser.name;
    dtype_raw = parser.dtype_raw;
    dimension = parser.dimension;
    value_raw = parser.value_raw;
    value_shape = parser.value_shape;
    value_origin = parser.value_origin;
    units_raw = parser.units_raw;
  }
  
  BaseNode::NodeListType BaseNode::parse(Environment& env) {
    return {};
  }
  
}
