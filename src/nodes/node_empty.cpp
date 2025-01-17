#include "nodes.h"

namespace dip {

  std::shared_ptr<BaseNode> EmptyNode::is_node(Parser& parser) {
    if (!parser.do_continue())
      return std::make_shared<EmptyNode>(parser);
    parser.part_comment();
    if (parser.is_parsed(Parser::PART_COMMENT))
      return std::make_shared<EmptyNode>(parser);
    return nullptr;
  }
  
}
