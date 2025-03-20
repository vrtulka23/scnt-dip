#include <iostream>
#include <fstream>

#include "parsers.h"
#include "dip.h"
#include "helpers.h"

namespace dip {

  EnvSource parse_source(const std::string& source_name, const std::string& source_file, const Source& parent) {
    if (source_file.size() >= FILE_SUFFIX_DIP.size() &&
	source_file.compare(source_file.size() - FILE_SUFFIX_DIP.size(), FILE_SUFFIX_DIP.size(), FILE_SUFFIX_DIP) == 0) {
      DIP d(parent);
      d.add_file(source_file, source_name);
      Environment senv = d.parse();
      return EnvSource({source_name, source_file, senv.sources[source_name].code, parent, senv.nodes});
    } else {
      std::ifstream file(source_file);
      if (!file) 
	throw std::runtime_error("Following file could not be found: "+source_file);
      std::ostringstream source_code;
      source_code << file.rdbuf();
      return EnvSource({source_name, source_file, source_code.str(), parent});
    }    
  }

  std::queue<Line> parse_lines(std::queue<Line>& lines, const std::string& source_code, const std::string& source_name) {
    // parse nodes from a text
    std::string line;
    std::istringstream ss(source_code);
    int line_number = 0;
    while (std::getline(ss, line)) {
      if (!line.empty() and !std::all_of(line.begin(), line.end(), isspace))
	lines.push({line, {source_name, line_number}});
      line_number++;
    }
    return lines;
  }
  
  BaseNode::NodeListType parse_code_nodes(std::queue<Line>& lines) {
    BaseNode::NodeListType nodes;
    while (!lines.empty()) {
      Line line = lines.front();
      lines.pop();
      
      // group block strings
      size_t pos = 0;
      if ((pos = line.code.find(SIGN_BLOCK)) != std::string::npos) {      // opening block quotes
	pos += SIGN_BLOCK.length();
	std::ostringstream oss;
	oss << line.code;
	if (line.code.find(SIGN_BLOCK, pos) == std::string::npos) {       // closing block quotes on the same line
	  while (lines.size()>0) {
	    Line block_line = lines.front();
	    lines.pop();
	    oss << SEPARATOR_NEWLINE << block_line.code;
	    if (block_line.code.find(SIGN_BLOCK) != std::string::npos) {  // closing block quotes on a subsequent line
	      break;
	    }
	  }
	}
	line.code = oss.str();
      }
      
      // add replacement mark for escape symbols
      Parser::encode_escape_symbols(line.code);
      
      // determine node type
      Parser parser(line);
      BaseNode::PointerType node = nullptr;
      node = EmptyNode::is_node(parser);
      if (node==nullptr) parser.part_indent();
      if (node==nullptr) node = ImportNode::is_node(parser);
      // if (node==nullptr) node = UnitNode::is_node(parser);
      if (node==nullptr) node = SourceNode::is_node(parser);
      if (node==nullptr) node = CaseNode::is_node(parser);
      if (node==nullptr) node = OptionsNode::is_node(parser);
      if (node==nullptr) node = ConstantNode::is_node(parser);
      if (node==nullptr) node = FormatNode::is_node(parser);
      if (node==nullptr) node = TagsNode::is_node(parser);
      if (node==nullptr) node = DescriptionNode::is_node(parser);
      // if (node==nullptr) node = ConditionNode::is_node(parser);
      if (node==nullptr) parser.part_name();
      if (node==nullptr) node = GroupNode::is_node(parser);
      if (node==nullptr) node = ImportNode::is_node(parser);
      if (node==nullptr) node = ModificationNode::is_node(parser);
      if (node==nullptr) parser.part_type();
      if (node==nullptr) node = BooleanNode::is_node(parser);
      if (node==nullptr) node = IntegerNode::is_node(parser);
      if (node==nullptr) node = FloatNode::is_node(parser);
      if (node==nullptr) node = StringNode::is_node(parser);
      if (node==nullptr) node = TableNode::is_node(parser);

      // make sure that everything was parsed
      if (node==nullptr)
	throw std::runtime_error("Node could not be determined from : "+line.code);
      if (parser.do_continue())
	throw std::runtime_error("Could not parse all text on the line: "+line.code);

      // convert escape symbols to original characterss
      for (size_t i=0; i<node->value_raw.size(); i++)
	Parser::decode_escape_symbols(node->value_raw[i]);
      Parser::decode_escape_symbols(node->line.code);

      // put node into the list
      nodes.push_back(node);
    }
    return nodes;
  }

  BaseNode::NodeListType parse_table_nodes(std::queue<Line>& lines) { 
    // parse nodes from a table header
    BaseNode::NodeListType nodes;
    while(!lines.empty()) {
      Line line = lines.front();
      lines.pop();
      trim(line.code);
      // stop when the end of the table header is reached
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
      trim(line.code);
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
  
}
