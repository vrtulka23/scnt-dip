#include <array>
#include <fstream>

#include "nodes.h"
#include "../environment.h"
#include "../dip.h"
#include "../solvers/solvers.h"

namespace dip {

  BaseNode::PointerType ImportNode::is_node(Parser& parser) {
    if (parser.part_function()) {
      parser.part_comment();
      return std::make_shared<ImportNode>(parser);
    } else if (parser.part_reference()) {
      parser.part_comment();
      return std::make_shared<ImportNode>(parser);
    }
    return nullptr;
  }
  
  BaseNode::NodeListType ImportNode::parse(Environment& env) {
    NodeListType nodes;
    if (!value_func.empty()) {
      nodes = env.request_nodes(value_func, Environment::FUNCTION);
    } else if (!value_ref.empty()) {
      nodes = env.request_nodes(value_ref, Environment::REFERENCE);
    } else {
      throw std::runtime_error("Table node could not be parsed: "+line.code);      
    }
    // update node settings
    for (auto node: nodes) {
      int size = node->value_raw.size();
      node->indent += indent;
      if (!name.empty())
	node->name = name + std::string(1,SIGN_SEPARATOR) + node->name;
      node->value_shape = {size};
      if (node->dimension.empty())
	node->dimension = {{size,size}};
    }
    return nodes;
  }  
 
}
