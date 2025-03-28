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

  inline BaseNode::NodeListType parse_nodes(const std::string& value_raw, const std::string& source_name) {
    std::queue<Line> lines;
    parse_lines(lines, value_raw, source_name);
    return parse_table_nodes(lines);
  }
  
  BaseNode::NodeListType TableNode::parse(Environment& env) {
    std::string source_name = line.source.name+"_"+std::string(TABLE_SOURCE);
    NodeListType nodes;
    switch (value_origin) {
    case ValueOrigin::FUNCTION:
      nodes = env.request_nodes(value_raw.at(0), Environment::FUNCTION);
      break;
    case ValueOrigin::REFERENCE:
      nodes = env.request_nodes(value_raw.at(0), Environment::REFERENCE);
      break;
    case ValueOrigin::REFERENCE_RAW:
      nodes = parse_nodes(env.request_code(value_raw.at(0)), source_name);
      break;
    case ValueOrigin::STRING:
      nodes = parse_nodes(value_raw.at(0), source_name);
      break;
    default:
      throw std::runtime_error("Table nodes could not be parsed: "+line.code);
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
