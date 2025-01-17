#include "nodes.h"

namespace dip {

  std::shared_ptr<BaseNode> IntegerNode::is_node(Parser& parser) {
    if (parser.has_keyword(Node::NODE_INTEGER)) {
      parser.part_dimension();
      parser.part_equal();
      if (parser.is_parsed(Parser::PART_EQUAL))
	parser.part_value();
      else
	parser.defined = true;
      parser.part_units();
      parser.part_comment();
      return std::make_shared<IntegerNode>(parser);
    }
    return nullptr;
  }

  // TODO: set_value
  
  BaseNode::NodeListType IntegerNode::parse(Environment& env) {
    // TODO: process function
    // TODO: process expression
    // TODO: process units
    return {};
  }  
  
}
