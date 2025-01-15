#include "node.h"

namespace dip {

  std::shared_ptr<BaseNode> FloatNode::is_node(Parser& parser) {
    if (parser.has_keyword(Node::NODE_FLOAT)) {
      parser.part_dimension();
      parser.part_equal();
      if (parser.is_parsed(Parser::PART_EQUAL))
	parser.part_value();
      else
	parser.defined = true;
      parser.part_units();
      parser.part_comment();
      return std::make_shared<FloatNode>(parser);
    }
    return nullptr;
  }

  // TODO: set_value
  
  BaseNode::NodeListType FloatNode::parse(Environment& env) {
    // TODO: process function
    // TODO: process expression
    // TODO: process units
    return {};
  }  
  
}
