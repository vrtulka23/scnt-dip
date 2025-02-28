#include "nodes.h"

#include <sstream>

namespace dip {

  BaseNode::BaseNode(Parser& parser) {
    line = parser.line;
    indent = parser.indent;
    name = parser.name;
    value_raw = parser.value_raw;
    keyword = parser.keyword;
    dtype = parser.dtype;
    dtype_prop = parser.dtype_prop;
    dimension = parser.dimension;
  }

  BaseNode::BaseNode(Parser& parser, const NodeKeyword kwd) {
    line = parser.line;
    indent = parser.indent;
    name = parser.name;
    value_raw = parser.value_raw;
    keyword = kwd;
    dtype = parser.dtype;
    dtype_prop = parser.dtype_prop;
    dimension = parser.dimension;
  }

  BaseNode::NodeListType BaseNode::parse(const Environment& env) {
    return {};
  }

  std::string BaseNode::clean_name() {
    // TODO: implement name cleaning
    return name;
  }
  
}
