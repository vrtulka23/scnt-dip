#include "nodes.h"
#include "../environment.h"

namespace dip {

  std::shared_ptr<BaseNode> ConstantNode::is_node(Parser& parser) {
    parser.kwd_constant();
    if (parser.is_parsed(Parser::KWD_CONSTANT)) {
      return std::make_shared<ConstantNode>(parser);
    }
    return nullptr;
  }

  BaseNode::NodeListType ConstantNode::parse(Environment& env) {
    std::array<Node::NodeDtype, 4> allowed = {
      Node::NODE_BOOLEAN, Node::NODE_INTEGER, Node::NODE_FLOAT, Node::NODE_STRING
    };
    if (env.nodes.size()==0)
      throw std::runtime_error("Could not find a node that can be constant: "+line.to_string());
    std::shared_ptr<BaseNode> node = env.nodes[env.nodes.size()-1];
    if (std::find(allowed.begin(), allowed.end(), node->dtype) == allowed.end())
      throw std::runtime_error("Only str, int, float and bool nodes can be set as constant. Previous node is: "+node->line.to_string());
    node->constant = true;
    return {};
  }  
  
}
