#include <stdexcept>

#include "nodes.h"
#include "../environment.h"
#include "../helpers.h"
#include "../solvers/solvers.h"

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

  BaseNode::NodeListType TableNode::parse_lines(std::queue<Line>& lines) {
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
      BaseNode::PointerType node(nullptr);
      if (node==nullptr) node = BooleanNode::is_node(parser);
      if (node==nullptr) node = IntegerNode::is_node(parser);
      if (node==nullptr) node = FloatNode::is_node(parser);
      if (node==nullptr) node = StringNode::is_node(parser);
      // make sure that everything was parsed
      if (node==nullptr)
	throw std::runtime_error("Node could not be determined from : "+line.code);
      if (parser.do_continue())
	throw std::runtime_error("Could not parse all text on the line: "+line.code);
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
	parser.value_raw.clear();
	if (parser.part_string()) {
	  node->value_raw.push_back(parser.value_raw[0]);
	} else {
	  throw std::runtime_error("Could not parse column '"+node->name+"' from the table row: "+line.code);
	}
      }
      if (parser.do_continue())
	throw std::runtime_error("Could not parse all text on the line: "+line.code);
    }
    return nodes;
  }
  
  BaseNode::NodeListType TableNode::parse(Environment& env) {
    NodeListType nodes;
    if (!value_func.empty()) {
      // parse nodes using a custom function
      FunctionSolver solver(env);
      nodes = solver.solve_table(value_func);
    } else if (!value_raw[0].empty()) {
      // parse nodes from a text
      std::queue<Line> lines;
      std::string token;
      std::istringstream stream(value_raw[0]);
      while (std::getline(stream, token)) {
	trim(token); // trim lines
	if (!token.empty())
	  lines.push({token, line.source});
      }
      nodes = parse_lines(lines);
    } else {
      throw std::runtime_error("Table node could not be parsed: "+line.code);      
    }
    // update node settings
    for (auto node: nodes) {
      int size = node->value_raw.size();
      node->indent = indent;
      node->name = name + std::string(1,SIGN_SEPARATOR) + node->name;
      node->value_shape = {size};
      if (node->dimension.empty())
	node->dimension = {{size,size}};
    }
    return nodes;
  }

}  
