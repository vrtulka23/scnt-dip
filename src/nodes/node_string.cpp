#include <regex>

#include "nodes.h"

namespace dip {

  std::shared_ptr<BaseNode> StringNode::is_node(Parser& parser) {
    if (parser.has_dtype(Node::NODE_STRING)) {
      parser.part_dimension();
      parser.part_equal();
      if (parser.is_parsed(Parser::PART_EQUAL))
	parser.part_value();
      else
	parser.declared = true;
      parser.part_units();
      parser.part_comment();
      return std::make_shared<StringNode>(parser);
    }
    return nullptr;
  }

  // TODO: set_value
  
  BaseNode::NodeListType StringNode::parse(Environment& env) {
    // TODO: process function
    // TODO: process expression
    // TODO: process units
    return {};
  }

  std::unique_ptr<BaseValue> StringNode::cast_scalar_value(const std::string value_input) {
    return std::make_unique<ScalarValue<std::string>>(value_input, BaseValue::VALUE_STRING);
  }

  std::unique_ptr<BaseValue> StringNode::cast_array_value(const std::vector<std::string>& value_inputs, const std::vector<int>& shape) {      
    return std::make_unique<ArrayValue<std::string>>(value_inputs, shape, BaseValue::VALUE_STRING);
  }
  
  void StringNode::set_option(const std::string option_value, const std::string option_units, Environment& env) {
    std::unique_ptr<BaseValue> ovalue = std::make_unique<ScalarValue<std::string>>(option_value, BaseValue::VALUE_STRING);
    options.push_back({std::move(ovalue), option_value, option_units});
  }

  void StringNode::validate_format() {
    if (format.size()>0) {
      std::regex pattern(format);
      if (!std::regex_match(value->to_string(), pattern)) {
	throw std::runtime_error("Node value '"+value->to_string()+"' does not match with expected format '"+format+"'");
      }
    }
  }
  
}
