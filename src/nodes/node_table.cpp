#include <stdexcept>

#include "nodes.h"
#include "../environment.h"
#include "../helpers.h"

namespace dip {
  
  std::shared_ptr<BaseNode> TableNode::is_node(Parser& parser) {
    if (parser.has_dtype(Node::NODE_TABLE)) {
      parser.part_dimension();
      parser.part_equal();
      if (parser.is_parsed(Parser::PART_EQUAL))
	parser.part_value();
      else 
	parser.declared = true;
      parser.part_units();
      parser.part_comment();
      return std::make_shared<TableNode>(parser);
    }
    return nullptr;
  }

  BaseNode::NodeListType TableNode::parse(Environment& env) {
    // create a line queue from the raw value
    std::queue<Line> lines;
    std::string token;
    std::istringstream stream(value_raw[0]);
    while (std::getline(stream, token)) {
      trim(token); // trim lines
      if (!token.empty())
	lines.push({token,line.source});
    }
    // parse nodes from a table header
    NodeListType nodes;
    while(!lines.empty()) {
      Line line = lines.front();
      lines.pop();
      // stop when the end of the table header is reached
      if (line.code.empty() or std::all_of(line.code.begin(), line.code.end(), isspace))
	break;
      if (line.code==SEPARATOR_TABLE_HEADER)
	break;
      // parse a node from the current line
      Parser parser(line);
      parser.part_name();
      parser.part_type();
      parser.part_dimension();
      parser.part_units();
      if (parser.do_continue())
	throw std::runtime_error("Incorrect header format: "+line.code);
      // initialize actual node
      std::shared_ptr<BaseNode> node(nullptr);
      if (node==nullptr) node = BooleanNode::is_node(parser);
      if (node==nullptr) node = IntegerNode::is_node(parser);
      if (node==nullptr) node = FloatNode::is_node(parser);
      if (node==nullptr) node = StringNode::is_node(parser);
      // make sure that everything was parsed
      if (node==nullptr)
	throw std::runtime_error("Node could not be determined from : "+line.code);
      if (parser.do_continue())
	throw std::runtime_error("Could not parse all text on the line: "+line.code);
      node->indent = indent;
      node->name = name + std::string(1,SIGN_SEPARATOR) + node->name;
      node->value_raw.reserve(lines.size()); // roughly reserve some memory to avoid reallocations
      nodes.push_back(node);
    }
    // read values from the rest of the table data and assign them to the node raw values
    while(!lines.empty()) {
      Line line = lines.front();
      lines.pop();
      // split a table line into node values
      Parser parser(line);
      for (size_t i=0; i<nodes.size(); i++) {
	if (i>0)
	  parser.part_delimiter(SEPARATOR_TABLE_COLUMNS);
	auto node = nodes.at(i);
	if (parser.part_string()) {
	  node->value_raw.push_back(parser.value_raw[0]);
	} else {
	  throw std::runtime_error("Could not parse column '"+node->name+"' from the table row: "+line.code);
	}
      }
      if (parser.do_continue())
	throw std::runtime_error("Could not parse all text on the line: "+line.code);
    }
    // update node value counters
    for (auto node: nodes) {
      int size = node->value_raw.size();
      node->value_shape = {size};
      if (node->dimension.empty())
	node->dimension = {{size,size}};
    }
    return nodes;
  }

}  
