#include <stdexcept>

#include "nodes.h"
#include "../environment.h"
#include "../parsers.h"

namespace dip {
  
  BaseNode::PointerType TableNode::is_node(Parser& parser) {
    if (parser.dtype_raw[1]=="table") {
      parser.part_dimension();
      if (parser.part_equal(false))
	parser.part_value();
      parser.part_units();
      parser.part_comment();
      return std::make_shared<TableNode>(parser);
    }
    return nullptr;
  }
  
  BaseNode::NodeListType TableNode::parse(Environment& env) {
    std::string source_name = line.source.name+"_"+std::string(TABLE_SOURCE);
    NodeListType nodes;
    if (!value_func.empty()) {
      nodes = env.request_nodes(value_func, Environment::FUNCTION);
    } else if (!value_ref.empty()) {
      nodes = env.request_nodes(value_ref, Environment::REFERENCE);
    } else if (!value_raw.empty() and !value_raw[0].empty()) {
      std::queue<Line> lines;
      parse_lines(lines, value_raw[0], source_name);
      nodes = parse_table_nodes(lines);
    } else {
      throw std::runtime_error("Table node could not be parsed: "+line.code);      
    }
    // update node settings
    for (auto node: nodes) {
      int size = node->value_raw.size();
      node->indent += indent;
      node->name = name + std::string(1,SIGN_SEPARATOR) + node->name;
      node->value_shape = {size};
      if (node->dimension.empty())
	node->dimension = {{size,size}};
    }
    return nodes;
  }

}  
