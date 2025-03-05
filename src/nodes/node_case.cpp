#include <regex>

#include "nodes.h"
#include "../environment.h"

namespace dip {

  std::shared_ptr<BaseNode> CaseNode::is_node(Parser& parser) {
    parser.kwd_case();
    if (parser.is_parsed(Parser::KWD_CASE)) {
      parser.part_value();
      parser.part_comment();
      return std::make_shared<CaseNode>(parser);
    }
    return nullptr;
  }  

  BaseNode::NodeListType CaseNode::parse(Environment& env) {
    std::ostringstream oss;
    oss << "^([a-zA-Z0-9_.-]*[" << SIGN_CONDITION << "])";
    oss << "(" << KEYWORD_CASE << "|" << KEYWORD_ELSE << "|" << KEYWORD_END << ")";
    std::regex pattern(oss.str());
    std::smatch matchResult;
    if (std::regex_search(name, matchResult, pattern)) {
      case_id = env.branching.register_case();
      case_type = matchResult[2].str();
      name = matchResult[1].str() + std::to_string(case_id);
      if (case_type == std::string(KEYWORD_CASE)) {
	// TODO: use logical solver to solve cases
	if (value_expr.empty()) {
	  value = (value_raw==std::string(KEYWORD_TRUE)) ? true : false;
	} else
	  value = (value_expr==std::string(KEYWORD_TRUE)) ? true : false;
      } else if (matchResult[2].str() == std::string(KEYWORD_ELSE)) {
	value = true;
      }
    }
    return {};
  }  
  
}
