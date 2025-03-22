#include <regex>

#include "nodes.h"
#include "../environment.h"

namespace dip {

  BaseNode::PointerType CaseNode::is_node(Parser& parser) {
    if (parser.kwd_case()) {
      parser.part_value();
      parser.part_comment();
      return std::make_shared<CaseNode>(parser);
    }
    return nullptr;
  }  

  BaseNode::NodeListType CaseNode::parse(Environment& env) {
    std::ostringstream oss;
    oss << "^(" << PATTERN_PATH << "*[" << SIGN_CONDITION << "])";
    oss << "(" << KEYWORD_CASE << "|" << KEYWORD_ELSE << "|" << KEYWORD_END << ")";
    std::regex pattern(oss.str());
    std::smatch matchResult;
    if (std::regex_search(name, matchResult, pattern)) {
      case_id = env.branching.register_case();
      case_type = matchResult[2].str();
      name = matchResult[1].str() + "C" + std::to_string(case_id);
      if (case_type==KEYWORD_CASE) {
	// TODO: use logical solver to solve cases
	if (value_raw.empty())
	  throw std::runtime_error("Case node requires an input value: "+line.code);
	value = (value_raw.at(0)==KEYWORD_TRUE) ? true : false;
      } else if (case_type==KEYWORD_ELSE) {
	value = true;
      }
    }
    return {};
  }  
  
}
