#include <regex>
#include <sstream>
#include <iostream>
#include <stdexcept>

#include "nodes.h"
#include "../parsers.h"
#include "../helpers.h"

namespace dip {

  const std::array<std::string, 3> Parser::ESCAPE_SYMBOLS = {"\\\"", "\\'", "\\n"};
  
  void Parser::strip(const std::string& text) {
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
    constexpr auto pstr = ce_concat<50>("^(", PATTERN_PATH, "*[", SIGN_CONDITION, "]", KEYWORD_CASE, ")[ ]*");
    std::regex pattern(pstr.data());
    std::smatch matchResult;
    if (std::regex_search(code, matchResult, pattern)) {
      name = matchResult[1].str();
      strip(matchResult[0].str());
      return true;
    } else {
      constexpr auto pstr = ce_concat<50>("^", PATTERN_PATH, "*(",
					   "[", SIGN_CONDITION, "]", KEYWORD_ELSE, "|"
					   "[", SIGN_CONDITION, "]", KEYWORD_END , ")");
      pattern = pstr.data();
      if (std::regex_search(code, matchResult, pattern)) {
	name = matchResult[0].str();
	strip(matchResult[0].str());
	return true;
      }
    }
    return false;
  }
  
  bool Parser::kwd_unit() {
    constexpr auto pstr = ce_concat<50>("^(", PATTERN_PATH, "*[", SIGN_VARIABLE, "]", KEYWORD_UNIT, ")[ ]*");
    std::regex pattern(pstr.data());
    std::smatch matchResult;
    if (std::regex_search(code, matchResult, pattern)) {
      name = matchResult[1].str();
      strip(matchResult[0].str());
      return true;
    }
    return false;
  }

  bool Parser::kwd_source() {
    constexpr auto pstr = ce_concat<50>("^(", PATTERN_PATH, "*[", SIGN_VARIABLE, "]", KEYWORD_SOURCE, ")[ ]*");
    std::regex pattern(pstr.data());
    std::smatch matchResult;
    if (std::regex_search(code, matchResult, pattern)) {
      name = matchResult[1].str();
      strip(matchResult[0].str());
      return true;
    }
    return false;
  }

  bool Parser::kwd_property(PropertyType& ptype) {
    constexpr auto pstr = ce_concat<50>("^[", SIGN_VALIDATION, "](", PATTERN_KEY, "+)[ ]*");
    std::regex pattern(pstr.data());
    std::smatch matchResult;
    if (std::regex_search(code, matchResult, pattern)) {
      std::string key = matchResult[1].str();
      if (key==KEYWORD_OPTIONS)	          ptype = PropertyType::Options;
      else if (key==KEYWORD_CONSTANT)	  ptype = PropertyType::Constant;
      else if (key==KEYWORD_FORMAT)	  ptype = PropertyType::Format;
      else if (key==KEYWORD_TAGS)	  ptype = PropertyType::Tags;
      else if (key==KEYWORD_DESCRIPTION)  ptype = PropertyType::Description;
      else if (key==KEYWORD_CONDITION)	  ptype = PropertyType::Condition;
      else if (key==KEYWORD_DELIMITER)	  ptype = PropertyType::Delimiter;
      dimension.push_back({0,Array::max_range});
      strip(matchResult[0].str());
      return true;
    }
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
  
  bool Parser::part_name(const bool required) {
    constexpr auto pstr = ce_concat<50>("^", PATTERN_PATH, "+");
    std::regex pattern(pstr.data());
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
  
  bool Parser::part_key(const bool required) {
    constexpr auto pstr = ce_concat<50>("^", PATTERN_KEY, "+");
    std::regex pattern(pstr.data());
    std::smatch matchResult;
    if (std::regex_search(code, matchResult, pattern)) {
      value_raw.push_back(matchResult[0].str());
      value_origin = ValueOrigin::String;
      strip(matchResult[0].str());
      if (do_continue() and code[0]!=' ')
	throw std::runtime_error("Key has an invalid format: "+line.code);
      return true;
    } else if (required) {
      throw std::runtime_error("Key has an invalid format: "+line.code);
    }
    return false;
  }
  
  bool Parser::part_type(const bool required) {
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
      std::string slices = matchResult[1].str();
      parse_slices(slices, dimension);
      if (dimension.empty())
	throw std::runtime_error("Dimension settings cannot be empty: "+line.code);
      strip(matchResult[0].str());
      return true;
    }
    return false;
  }

  bool Parser::part_equal(const bool required) {
    constexpr auto pstr = ce_concat<50>("^[ ]*[", SIGN_EQUAL, "][ ]*");
    std::regex pattern(pstr.data());
    std::smatch matchResult;
    if (std::regex_search(code, matchResult, pattern)) {
      strip(matchResult[0].str());
      return true;
    } else if (required) {
      throw std::runtime_error("Equal sign '"+std::string(1,SIGN_EQUAL)+"' is required: "+line.code);
    }
    return false;
  }

  bool Parser::part_reference(const bool inject) {
    constexpr auto pstr = ce_concat<50>("^[ ]*[{](", PATTERN_KEY, "*([?]", PATTERN_PATH, "*|))[}]");
    std::regex pattern(pstr.data());
    std::smatch matchResult;
    if (std::regex_search(code, matchResult, pattern)) {
      value_raw.push_back( matchResult[1].str() );
      if (!matchResult[2].str().empty())
	value_origin = ValueOrigin::Reference;
      else if (!matchResult[1].str().empty())
	value_origin = ValueOrigin::ReferenceRaw;
      else
	throw std::runtime_error("Reference cannot be empty: "+line.code);
      // TODO: implement inject switch
      strip(matchResult[0].str());
      part_slice();
      return true;
    }
    return false;
  }
  
  bool Parser::part_function() {
    constexpr auto pstr = ce_concat<50>("^[ ]*[(](", PATTERN_KEY, "+)[)]");
    std::regex pattern(pstr.data());
    std::smatch matchResult;
    if (std::regex_search(code, matchResult, pattern)) {
      value_raw.push_back( matchResult[1].str() );
      value_origin = ValueOrigin::Function;
      strip(matchResult[0].str());
      return true;
    }
    return false;
  }
    
  bool Parser::part_expression() {
    return false;
  }
    
  bool Parser::part_array() {
    if (code.empty() or code.at(0)!=SIGN_ARRAY_OPEN)
      return false;
    std::string rm = parse_array(code, value_raw, value_shape);
    strip(rm);
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
	  value_origin = ValueOrigin::String;
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
    std::regex pattern("^\\[([0-9:,]*)\\]");
    std::smatch matchResult;
    if (std::regex_search(code, matchResult, pattern)) {
      std::string slices = matchResult[1].str();
      parse_slices(slices, value_slice);
      if (value_slice.empty())
	return false;
      strip(matchResult[0].str());
      return true;
    }
    return false;
  }
    
  bool Parser::part_units() {
    // In numerical expressions starting signs +-*/ have to be explicitely excluded
    std::regex pattern1("^[ ]+([^#= ]+)");
    std::regex pattern2("^[ ]+[/*+-]+");
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

  bool Parser::part_delimiter(const char symbol, const bool required) {
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
