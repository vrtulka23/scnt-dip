#include "nodes.h"

namespace dip {

  std::shared_ptr<BaseNode> GroupNode::is_node(Parser& parser) {
    parser.part_comment();
    if (!parser.do_continue())
      return std::make_shared<GroupNode>(parser);
    return nullptr;
  }  
  
}
