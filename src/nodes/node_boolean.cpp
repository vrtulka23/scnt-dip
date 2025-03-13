#include <stdexcept>

#include "nodes.h"
#include "../solvers/solvers.h"

namespace dip {

  std::shared_ptr<BaseNode> BooleanNode::is_node(Parser& parser) {
    if (parser.has_dtype(Node::NODE_BOOLEAN)) {
      parser.part_dimension();
      parser.part_equal();
      if (parser.is_parsed(Parser::PART_EQUAL))
	parser.part_value();
      parser.part_units();
      parser.part_comment();
      return std::make_shared<BooleanNode>(parser);
    }
    return nullptr;
  }

  std::shared_ptr<BaseNode> BooleanNode::create_scalar(const std::string& name, const bool value) {
    std::unique_ptr<BaseValue> ptr_value = ScalarValue<bool>::create(value);
    std::shared_ptr<dip::BooleanNode> ptr_node = std::make_shared<dip::BooleanNode>(name, std::move(ptr_value));
    return ptr_node;
  }

  std::shared_ptr<BaseNode> BooleanNode::create_array(const std::string& name, const std::vector<bool>&  arr, std::vector<int> sh) {
    if (sh.empty())
      sh.push_back(arr.size());
    std::unique_ptr<BaseValue> ptr_value = ArrayValue<bool>::create(arr,sh);
    std::shared_ptr<dip::BooleanNode> ptr_node = std::make_shared<dip::BooleanNode>(name, std::move(ptr_value));
    return ptr_node;
  }
  
  BaseNode::NodeListType BooleanNode::parse(Environment& env) {
    if (!units_raw.empty())
      throw std::runtime_error("Boolean data type does not support units: "+line.code);
    if (!value_func.empty()) {
      FunctionSolver solver(env);
      set_value(solver.solve_value(value_func));
    }
    // TODO: process expression
    return {};
  }
  
  BaseValue::PointerType BooleanNode::cast_scalar_value(const std::string value_input) {
    if (value_input==KEYWORD_TRUE)
      return std::make_unique<ScalarValue<bool>>(true, value_dtype);
    else if (value_input==KEYWORD_FALSE)
      return std::make_unique<ScalarValue<bool>>(false, value_dtype);
    else
      throw std::runtime_error("Value cannot be casted as boolean from the given string: "+value_input);
  }
  
  BaseValue::PointerType BooleanNode::cast_array_value(const std::vector<std::string>& value_inputs, const std::vector<int>& shape) {
    std::vector<bool> bool_values;
    for (auto value: value_inputs) {
      if (value==KEYWORD_TRUE)
	bool_values.push_back(true);
      else if (value==KEYWORD_FALSE)
	bool_values.push_back(false);
      else
	throw std::runtime_error("Value cannot be casted as boolean from the given string: "+value);
    }    
    return std::make_unique<ArrayValue<bool>>(bool_values, shape, value_dtype);    
  }

  void BooleanNode::set_option(const std::string option_value, const std::string option_units, Environment& env) {
    throw std::runtime_error("Option property is not implemented for boolean nodes: "+line.code);
  }
  
  void BooleanNode::validate_options() {
    if (format.size()>0)
      throw std::runtime_error("Options property is not implemented for boolean nodes: "+line.code);
  }
  
}
