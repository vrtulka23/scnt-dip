#include "nodes.h"

namespace dip {

  std::shared_ptr<BaseNode> BooleanNode::is_node(Parser& parser) {
    if (parser.has_keyword(Node::NODE_BOOLEAN)) {
      parser.part_dimension();
      parser.part_equal();
      if (parser.is_parsed(Parser::PART_EQUAL))
	parser.part_value();
      else
	parser.defined = true;
      parser.part_units();
      parser.part_comment();
      return std::make_shared<BooleanNode>(parser);
    }
    return nullptr;
  }

  // TODO: set_value

  BaseNode::NodeListType BooleanNode::parse(Environment& env) {
    // TODO: process function
    // TODO: process expression
    // TODO: process units
    return {};
  }
  
}
