#include <stdexcept>

#include "nodes.h"

namespace dip {

  std::shared_ptr<BaseNode> BooleanNode::is_node(Parser& parser) {
    if (parser.has_keyword(Node::NODE_BOOLEAN)) {
      parser.part_dimension();
      parser.part_equal();
      if (parser.is_parsed(Parser::PART_EQUAL))
	parser.part_value();
      else
	parser.defined = true;
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
    if (value_input==KWD_TRUE)
      return std::make_unique<ScalarValue<bool>>(true, BaseValue::DataType::BOOL);
    else if (value_input==KWD_FALSE)
      return std::make_unique<ScalarValue<bool>>(false, BaseValue::DataType::BOOL);
    else
      throw std::runtime_error("Value cannot be casted as boolean from the given string: "+value_input);
  }
  
  std::unique_ptr<BaseValue> BooleanNode::cast_array_value(const std::vector<std::string>& value_inputs, const std::vector<int>& shape) {
    std::vector<bool> bool_values;
    for (auto value: value_inputs) {
      if (value==KWD_TRUE)
	bool_values.push_back(true);
      else if (value==KWD_FALSE)
	bool_values.push_back(false);
      else
	throw std::runtime_error("Value cannot be casted as boolean from the given string: "+value);
    }    
    return std::make_unique<ArrayValue<bool>>(bool_values, shape, BaseValue::DataType::BOOL);    
  }
  
}
