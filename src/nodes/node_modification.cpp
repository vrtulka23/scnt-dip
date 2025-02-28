#include "nodes.h"

namespace dip {

  std::shared_ptr<BaseNode> ModificationNode::is_node(Parser& parser) {
    parser.part_equal();
    if (parser.is_parsed(Parser::PART_EQUAL)) {
      parser.part_value();
      parser.part_units();
      parser.part_comment();
      return std::make_shared<ModificationNode>(parser);
    }
    return nullptr;
  }  
  
}
