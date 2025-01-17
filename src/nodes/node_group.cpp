#include "nodes.h"

namespace dip {

  std::shared_ptr<BaseNode> GroupNode::is_node(Parser& parser) {
    if (!parser.do_continue())
      return std::make_shared<GroupNode>(parser);
    parser.part_comment();
    if (parser.is_parsed(Parser::PART_COMMENT))
      return std::make_shared<GroupNode>(parser);
    return nullptr;
  }  
  
}
