#include "nodes.h"

#include <regex>
#include <sstream>
#include <iostream>
#include <stdexcept>

namespace dip {

  const std::array<std::string, 3> Parser::ESCAPE_SYMBOLS = {"\\\"", "\\'", "\\n"};
  
  void Parser::strip(const std::string text) {
    code = code.substr(text.length(), code.length());
  }

  bool Parser::do_continue() {
    return code.length();
  }

  /*
   * Escape symbol handling
   */

  void Parser::encode_escape_symbols(std::string& str) {
    for (int i=0; i<ESCAPE_SYMBOLS.size(); i++) {
      std::string replace_symbol = "Z@"+std::to_string(i)+";";
      size_t pos = 0;
      while ((pos = str.find(ESCAPE_SYMBOLS[i], pos)) != std::string::npos) {
        str.replace(pos, ESCAPE_SYMBOLS[i].length(), replace_symbol);
        pos += replace_symbol.length();
      }
    }
  }
   
  void Parser::decode_escape_symbols(std::string& str) {
    for (int i=0; i<ESCAPE_SYMBOLS.size(); i++) {
      std::string replace_symbol = "Z@"+std::to_string(i)+";";
      size_t pos = 0;
      while ((pos = str.find(replace_symbol, pos)) != std::string::npos) {
        str.replace(pos, replace_symbol.length(), ESCAPE_SYMBOLS[i]);
        pos += ESCAPE_SYMBOLS[i].length();
      }
    }
  }
  
  /*
   * Directive keywords
   */

  bool Parser::kwd_case() {
    std::ostringstream oss;
    oss << "^(" << PATTERN_PATH << "*[" << SIGN_CONDITION << "]" << KEYWORD_CASE << ")[ ]*";
    std::regex pattern(oss.str());
    std::smatch matchResult;
    if (std::regex_search(code, matchResult, pattern)) {
      name = matchResult[1].str();
      strip(matchResult[0].str());
      return true;
    } else {
      oss.str("");
      oss.clear();
      oss << "^" << PATTERN_PATH << "*(";
      oss << "[" << SIGN_CONDITION << "]" << KEYWORD_ELSE << "|";
      oss << "[" << SIGN_CONDITION << "]" << KEYWORD_END  << ")";
      pattern = oss.str();
      if (std::regex_search(code, matchResult, pattern)) {
	name = matchResult[0].str();
	strip(matchResult[0].str());
	return true;
      }
    }
    return false;
  }
  
  bool Parser::kwd_unit() {
    return false;
  }
  
  bool Parser::kwd_source() {
    std::ostringstream oss;
    oss << "^(" << PATTERN_PATH << "*[" << SIGN_VARIABLE << "]" << KEYWORD_SOURCE << ")[ ]*";
    std::regex pattern(oss.str());
    std::smatch matchResult;
    if (std::regex_search(code, matchResult, pattern)) {
      name = matchResult[1].str();
      strip(matchResult[0].str());
      return true;
    }
    return false;
  }
  
  bool Parser::kwd_options() {
    std::ostringstream oss;
    oss << "^[" << SIGN_VALIDATION << "]" << KEYWORD_OPTIONS << "[ ]*";
    std::regex pattern(oss.str());
    std::smatch matchResult;
    if (std::regex_search(code, matchResult, pattern)) {
      dimension.push_back({0,-1});
      strip(matchResult[0].str());
      return true;
    }
    return false;
  }
  
  bool Parser::kwd_constant() {
    std::ostringstream oss;
    oss << "^[" << SIGN_VALIDATION << "]" << KEYWORD_CONSTANT;
    std::regex pattern(oss.str());
    std::smatch matchResult;
    if (std::regex_search(code, matchResult, pattern)) {
      strip(matchResult[0].str());
      return true;
    }
    return false;
  } 
  
  bool Parser::kwd_format() {
    std::ostringstream oss;
    oss << "^[" << SIGN_VALIDATION << "]" << KEYWORD_FORMAT << "[ ]*";
    std::regex pattern(oss.str());
    std::smatch matchResult;
    if (std::regex_search(code, matchResult, pattern)) {
      strip(matchResult[0].str());
      return true;
    }
    return false;
  }
  
  bool Parser::kwd_tags() {
    std::ostringstream oss;
    oss << "^[" << SIGN_VALIDATION << "]" << KEYWORD_TAGS << "[ ]*";
    std::regex pattern(oss.str());
    std::smatch matchResult;
    if (std::regex_search(code, matchResult, pattern)) {
      strip(matchResult[0].str());
      return true;
    }
    return false;
  }
  
  bool Parser::kwd_description() {
    std::ostringstream oss;
    oss << "^((";
    oss << "^[" << SIGN_VALIDATION << "]" << KEYWORD_DESCRIPTION << "|";
    oss << "^[" << SIGN_VALIDATION << "]" << std::string(KEYWORD_DESCRIPTION).substr(0,4);
    oss << ")[ ]*)";
    std::regex pattern(oss.str());
    std::smatch matchResult;
    if (std::regex_search(code, matchResult, pattern)) {
      strip(matchResult[1].str());
      return true;
    }
    return false;
  }
  
  bool Parser::kwd_condition() {
    return false;
  }
  
  /*
   * Node Parts
   */
  
  bool Parser::part_indent() {
    std::regex pattern("^[ ]+");
    std::smatch matchResult;
    if (std::regex_search(code, matchResult, pattern)) {
      indent = matchResult[0].str().length();
      strip(matchResult[0].str());
      return true;
    }
    return false;
  }
  
  bool Parser::part_name(bool required) {
    std::ostringstream oss;
    oss << "^" << PATTERN_PATH << "+";
    std::regex pattern(oss.str());
    std::smatch matchResult;
    if (std::regex_search(code, matchResult, pattern)) {
      name = matchResult[0].str();
      strip(matchResult[0].str());
      if (do_continue() and code[0]!=' ')
	throw std::runtime_error("Name has an invalid format: "+line.code);
      return true;
    } else if (required) {
      throw std::runtime_error("Name has an invalid format: "+line.code);
    }
    return false;
  }
  
  bool Parser::part_key(bool required) {
    std::ostringstream oss;
    oss << "^" << PATTERN_KEY << "+";
    std::regex pattern(oss.str());
    std::smatch matchResult;
    if (std::regex_search(code, matchResult, pattern)) {
      value_raw.push_back(matchResult[0].str());
      strip(matchResult[0].str());
      if (do_continue() and code[0]!=' ')
	throw std::runtime_error("Key has an invalid format: "+line.code);
      return true;
    } else if (required) {
      throw std::runtime_error("Key has an invalid format: "+line.code);
    }
    return false;
  }
  
  bool Parser::part_type(bool required) {
    std::smatch matchResult;
    std::regex pattern;
    pattern = "^[ ]+(u|)(bool|int|float|str|table)(16|32|64|128|x|)";
    if (std::regex_search(code, matchResult, pattern)) {
      dtype_raw = {matchResult[1].str(), matchResult[2].str(), matchResult[3].str()};
      strip(matchResult[0].str());
      return true;
    } else if (required) {
      throw std::runtime_error("Type not recognized: "+line.code);
    }
    return false;
  }

  bool Parser::part_dimension() {
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
      if (dimension.empty())
	throw std::runtime_error("Dimension settings cannot be empty: "+line.code);
      strip(matchResult[0].str());
      return true;
    }
    return false;
  }

  bool Parser::part_equal(bool required) {
    if (part_delimiter('=', required)) {
      return true;
    }
    return false;
  }

  bool Parser::part_reference(const bool inject) {
    std::ostringstream oss;
    oss << "^[ ]*[{](" << PATTERN_KEY << "*[?]" << PATTERN_PATH << "*)[}]";
    std::regex pattern(oss.str());
    std::smatch matchResult;
    if (std::regex_search(code, matchResult, pattern)) {
      value_raw.clear();
      value_ref = matchResult[1].str();
      // TODO: implement inject switch
      strip(matchResult[0].str());
      return true;
    }
    return false;
  }
  
  bool Parser::part_function() {
    std::ostringstream oss;
    oss << "^[ ]*[(](" << PATTERN_KEY << "+)[)]";
    std::regex pattern(oss.str());
    std::smatch matchResult;
    if (std::regex_search(code, matchResult, pattern)) {
      value_raw.clear();
      value_func = matchResult[1].str();
      strip(matchResult[0].str());
      return true;
    }
    return false;
  }
    
  bool Parser::part_expression() {
    return false;
  }
    
  bool Parser::part_array() {
    std::stringstream ss(code), rm;
    char ch;

    // test for an openning bracket
    ss.get(ch);
    rm << ch;
    if (ch!='[')
      return false;
    
    std::string value;
    bool inString = false;
    int  dim = 1;
    value_shape.push_back(0);
    
    while (ss.get(ch) and dim>0) {
      rm << ch;
      if (ch == '[') {
	dim++;
	if (value_shape.size()<dim)
	  value_shape.push_back(0);
	value.clear();
      } else if (ch == ',') {
	if (!value.empty()) {
	  value_raw.push_back(value);
	  value.clear();
	}
	value_shape[dim-1]++;
      } else if (ch == ']') {
	if (!value.empty()) {
	  value_raw.push_back(value);
	  value.clear();
	}
	value_shape[dim-1]++;
	dim--;
      } else if (ch == '"') {
	value.clear();
	while (ss.get(ch) && ch != '"') {
	  value += ch;
	  rm << ch;
	}
	rm << ch;
      } else if (ch == '\'') {
	value.clear();
	while (ss.get(ch) && ch != '\'') {
	  value += ch;
	  rm << ch;
	}
	rm << ch;
      } else if (ch == ' ') {
	continue;
      } else {
	value += ch;
      }
    }
    
    // Check if all nested arrays are closed
    if (dim!=0)
      throw std::runtime_error("Definition of an array has some unclosed brackets or quotes: "+line.code);

    // Normalize shape and check coherence of nested arrays
    int coef = 1;
    for (int d=1; d<value_shape.size(); d++) {
      coef *= value_shape[d-1];
      if (value_shape[d]%coef != 0) 
	throw std::runtime_error("Items in dimension "+std::to_string(d+1)+" do not have the same shape: "+line.code);
      value_shape[d] /= coef;
    }
    //std::cout << line.code << std::endl;
    //std::cout << ss.str() << std::endl;
    //std::cout << rm.str() << std::endl;
    strip(rm.str());
    return true;
  }

  bool Parser::part_string() {
    std::regex pattern("^(\"\"\"([^\"]*)\"\"\"|\"([^\"]*)\"|\'([^\']*)\'|((?!#)[^ ]+))");
    std::smatch matchResult;
    if (std::regex_search(code, matchResult, pattern)) {
      for (int i=2; i<6; i++) {
	std::string vraw = matchResult[i].str();
	if (vraw!="") {
	  value_raw.push_back(vraw);
	  break;
	}
      }
      strip(matchResult[0].str());
      return true;
    }
    return false;
  }
  
  bool Parser::part_value() {
    if (part_reference(true))
      return true;
    if (part_function())
      return true;
    if (part_expression())
      return true;
    if (part_array())
      return true;
    if (part_string())
      return true;
    return false;
  }
  
  bool Parser::part_slice() {
    return false;
  }
    
  bool Parser::part_units() {
    // In numerical expressions starting signs +-*/ have to be explicitely excluded
    std::regex pattern1("^[ ]+([^#= ]+)"), pattern2("^[ ]+[/*+-]+");
    std::smatch matchResult;
    if (std::regex_search(code, matchResult, pattern1) and !std::regex_match(code, pattern2)) {
      units_raw = matchResult[1].str();
      strip(matchResult[0].str());
      return true;
    }
    return false;
  }
    
  bool Parser::part_comment() {
    std::regex pattern("^[ ]*#[ ]*(.*)$");
    std::smatch matchResult;
    if (std::regex_search(code, matchResult, pattern)) {
      comment = matchResult[1].str();
      strip(matchResult[0].str());
      return true;
    }
    return false;
  }

  bool Parser::part_delimiter(char symbol, bool required) {
    std::ostringstream oss;
    oss << "^[ ]*[" << symbol << "][ ]*";
    std::regex pattern(oss.str());
    std::smatch matchResult;
    if (std::regex_search(code, matchResult, pattern)) {
      strip(matchResult[0].str());
      return true;
    } else if (required) {
      throw std::runtime_error("Delimiter '"+std::string(1,symbol)+"' is required: "+line.code);
    }
    return false;
  }
  
}
