#include <stdexcept>

#include "nodes.h"

namespace dip {

  std::shared_ptr<BaseNode> BooleanNode::is_node(Parser& parser) {
    if (parser.has_dtype(Node::NODE_BOOLEAN)) {
      parser.part_dimension();
      parser.part_equal();
      if (parser.is_parsed(Parser::PART_EQUAL))
	parser.part_value();
      else
	parser.declared = true;
      parser.part_units();
      parser.part_comment();
      return std::make_shared<BooleanNode>(parser);
    }
    return nullptr;
  }

  // TODO: set_value

  BaseNode::NodeListType BooleanNode::parse(Environment& env) {
    // TODO: process function
    // TODO: process expression
    // TODO: process units
    return {};
  }

  std::unique_ptr<BaseValue> BooleanNode::cast_scalar_value(const std::string value_input) {
    if (value_input==KEYWORD_TRUE)
      return std::make_unique<ScalarValue<bool>>(true, BaseValue::VALUE_BOOL);
    else if (value_input==KEYWORD_FALSE)
      return std::make_unique<ScalarValue<bool>>(false, BaseValue::VALUE_BOOL);
    else
      throw std::runtime_error("Value cannot be casted as boolean from the given string: "+value_input);
  }
  
  std::unique_ptr<BaseValue> BooleanNode::cast_array_value(const std::vector<std::string>& value_inputs, const std::vector<int>& shape) {
    std::vector<bool> bool_values;
    for (auto value: value_inputs) {
      if (value==KEYWORD_TRUE)
	bool_values.push_back(true);
      else if (value==KEYWORD_FALSE)
	bool_values.push_back(false);
      else
	throw std::runtime_error("Value cannot be casted as boolean from the given string: "+value);
    }    
    return std::make_unique<ArrayValue<bool>>(bool_values, shape, BaseValue::VALUE_BOOL);    
  }

  void BooleanNode::set_option(const std::string option_value, const std::string option_units, Environment& env) {
    throw std::runtime_error("Option property is not implemented for boolean nodes: "+line.code);
  }
  
  void BooleanNode::validate_options() {
    if (format.size()>0)
      throw std::runtime_error("Options property is not implemented for boolean nodes: "+line.code);
  }
  
}
