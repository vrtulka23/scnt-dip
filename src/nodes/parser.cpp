#include "node.h"

#include <regex>
#include <sstream>
#include <iostream>
#include <stdexcept>

namespace dip {

  void Parser::_strip(const std::string text, ParsingFlag flag) {
    parsed.push_back(flag);
    code = code.substr(text.length(), code.length());
  }

  bool Parser::do_continue() {
    return code.length();
  }

  bool Parser::is_parsed(ParsingFlag flag) {
    for (ParsingFlag f: parsed)
      if (f==flag)
	return true;
    return false;
  }
  
  void Parser::part_indent() {
    std::regex pattern("^[ ]+");
    std::smatch matchResult;
    if (std::regex_search(code, matchResult, pattern)) {
      indent = matchResult[0].str().length();
      _strip(matchResult[0].str(), PART_INDENT);
    }
  }
  
  void Parser::part_name(const bool path) {
    std::regex pattern(path ? "^[a-zA-Z0-9_.-]+" : "^[a-zA-Z0-9_]+");
    std::smatch matchResult;
    if (std::regex_search(code, matchResult, pattern)) {
      name = matchResult[0].str();
      _strip(matchResult[0].str(), PART_NAME);
      if (do_continue() and code[0]!=' ')
	throw std::runtime_error("Name has an invalid format: "+line.code);
    } else {
      throw std::runtime_error("Name has an invalid format: "+line.code);
    }
  }
  
  void Parser::part_type() {
    std::smatch matchResult;
    std::regex pattern;
    pattern = "^[ ]+(u|)(bool|str|table|int|float)(16|32|64|)";
    if (std::regex_search(code, matchResult, pattern)) {
      dtype = matchResult[2].str();
      if (dtype=="int") {
	keyword = Node::NODE_INTEGER;
	dtype_prop = {matchResult[1].str(), matchResult[3].str()};
      } else if (dtype=="float") {
	keyword = Node::NODE_FLOAT;
	dtype_prop = {matchResult[2].str()};
      } else if (matchResult[1].str()!="" or matchResult[3].str()!="") {
	throw std::runtime_error("Incorrect type: "+line.code);
      } else if (dtype=="table") {
	keyword = Node::NODE_TABLE;
      } else if (dtype=="bool") {
	keyword = Node::NODE_BOOLEAN;
      } else if (dtype=="str") {
	keyword = Node::NODE_STRING;
      } else {
	throw std::runtime_error("Type not recognized: "+line.code);
      }
      _strip(matchResult[0].str(), PART_TYPE);
    } else {
      throw std::runtime_error("Type not recognized: "+line.code);
    }
  }

  void Parser::part_dimension() {
    std::regex pattern("^\\[([0-9:,]*)\\]");
    std::smatch matchResult;
    if (std::regex_search(code, matchResult, pattern)) {
      std::istringstream ss_dims(matchResult[1].str());
      std::string dim;
      while (getline(ss_dims, dim, SEPARATOR_DIMENSION)) {
	std::tuple<int,int> slice;
	std::size_t pos = dim.find_first_of(SEPARATOR_SLICE);
	if (pos==std::string::npos) {
	  slice = {std::stoi(dim), std::stoi(dim)};
	} else {
	  std::string dmin=dim.substr(0,pos), dmax=dim.substr(pos+1);
	  if (dmin=="" and dmax=="")
	    slice = {0, -1};
	  else if (dmin=="")
	    slice = {0, std::stoi(dmax)};
	  else if (dmax=="") {
	    slice = {std::stoi(dmin), -1};
	  } else
	    slice = {std::stoi(dmin), std::stoi(dmax)};
	}
	dimension.push_back(slice);
      }
      if (dimension.size()==0)
	throw std::runtime_error("Dimension settings cannot be empty: "+line.code);
      _strip(matchResult[0].str(), PART_DIMENSION);      
    }
  }

  void Parser::part_equal() {
    std::regex pattern("^[ ]*=[ ]*");
    std::smatch matchResult;
    if (std::regex_search(code, matchResult, pattern)) {
      _strip(matchResult[0].str(), PART_EQUAL);
    }
  }
    
  void Parser::part_value() {
    part_reference(true);
    if (is_parsed(PART_REFERENCE)) {
      parsed.push_back(PART_VALUE);
      return;
    }
    part_function();
    if (is_parsed(PART_FUNCTION)) {
      parsed.push_back(PART_VALUE);
      return;
    }
    part_expression();
    if (is_parsed(PART_EXPRESSION)) {
      parsed.push_back(PART_VALUE);
      return;
    }
    std::regex pattern("^(\"\"\"(.*)\"\"\"|\"(.*)\"|\'(.*)\'|([^# ]+))");
    std::smatch matchResult;
    if (std::regex_search(code, matchResult, pattern)) {
      for (int i=2; i<6; i++) {
	std::string vraw = matchResult[i].str();
	if (vraw!="") {
	  value_raw = vraw;
	  break;
	}
      }
      _strip(matchResult[0].str(), PART_VALUE);
    }
  }
  
  void Parser::part_reference(const bool inject) {
  }
  
  void Parser::part_slice() {
  }
    
  void Parser::part_format() {
  }
    
  void Parser::part_function() {
  }
    
  void Parser::part_expression() {
  }
    
  void Parser::part_units() {
  }
    
  void Parser::part_comment() {
    std::regex pattern("^[ ]*#[ ]*(.*)$");
    std::smatch matchResult;
    if (std::regex_search(code, matchResult, pattern)) {
      comment = matchResult[0].str();
      _strip(matchResult[0].str(), PART_COMMENT);
    }    
  }
  
}
