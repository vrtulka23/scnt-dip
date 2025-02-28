#include <array>

#include "nodes.h"
#include "../environment.h"

namespace dip {

  std::shared_ptr<BaseNode> DescriptionNode::is_node(Parser& parser) {
    parser.kwd_description();
    if (parser.is_parsed(Parser::KWD_DESCRIPTION)) {
      parser.part_value();
      parser.part_comment();
      return std::make_shared<DescriptionNode>(parser);
    }
    return nullptr;
  }
  
  BaseNode::NodeListType DescriptionNode::parse(Environment& env) {
    std::array<Node::NodeDtype, 4> allowed = {
      Node::NODE_BOOLEAN, Node::NODE_INTEGER, Node::NODE_FLOAT, Node::NODE_STRING
    };
    if (env.nodes.size()==0)
      throw std::runtime_error("Could not find a node that can have a description: "+line.to_string());
    std::shared_ptr<BaseNode> node = env.nodes[env.nodes.size()-1];
    if (std::find(allowed.begin(), allowed.end(), node->dtype) == allowed.end())
      throw std::runtime_error("Description can be set only to str, int, float and bool nodes. Previous node is: "+node->line.to_string());
    node->description += value_raw;
    return {};
  }  
 
}
