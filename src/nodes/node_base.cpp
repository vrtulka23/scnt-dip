#include "nodes.h"

#include <sstream>

namespace dip {

  BaseNode::BaseNode(Parser& parser) {
    line = parser.line;
    indent = parser.indent;
    name = parser.name;
    value_raw = parser.value_raw;
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
    dtype = dt;
    dtype_raw = parser.dtype_raw;
    dtype_prop = parser.dtype_prop;
    dimension = parser.dimension;
    declared = parser.declared;
  }

  BaseNode::NodeListType BaseNode::parse(Environment& env) {
    return {};
  }

  std::string BaseNode::clean_name() {
    // TODO: implement name cleaning
    return name;
  }
  
}
