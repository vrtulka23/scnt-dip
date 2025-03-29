#include <stdexcept>

#include "nodes.h"
#include "../parsers.h"
#include "../solvers/solvers.h"

namespace dip {

  BaseNode::PointerType BooleanNode::is_node(Parser& parser) {
    if (parser.dtype_raw[1]=="bool") {
      parser.part_dimension();
      if (parser.part_equal(false))
	parser.part_value();
      parser.part_units();
      parser.part_comment();
      return std::make_shared<BooleanNode>(parser);
    }
    return nullptr;
  }

  BaseNode::NodeListType BooleanNode::parse(Environment& env) {
    if (!units_raw.empty())
      throw std::runtime_error("Boolean data type does not support units: "+line.code);
    switch (value_origin) {
    case ValueOrigin::FUNCTION:
      set_value(env.request_value(value_raw.at(0), Environment::FUNCTION));
      break;
    case ValueOrigin::REFERENCE:
      set_value(env.request_value(value_raw.at(0), Environment::REFERENCE));
      break;
    case ValueOrigin::REFERENCE_RAW: {
      std::string source_code = env.request_code(value_raw.at(0));
      std::vector<std::string> source_value_raw;
      BaseValue::ShapeType source_value_shape;
      parse_value(source_code, source_value_raw, source_value_shape);
      set_value(cast_value(source_value_raw, source_value_shape));
      break;
    }
    default:
      break;
    }
    // TODO: process expression
    return {};
  }
  
  BaseValue::PointerType BooleanNode::cast_scalar_value(const std::string& value_input) const {
    if (value_input==KEYWORD_TRUE)
      return std::make_unique<ScalarValue<bool>>(true, value_dtype);
    else if (value_input==KEYWORD_FALSE)
      return std::make_unique<ScalarValue<bool>>(false, value_dtype);
    else
      throw std::runtime_error("Value cannot be casted as boolean from the given string: "+value_input);
  }
  
  BaseValue::PointerType BooleanNode::cast_array_value(const std::vector<std::string>& value_inputs, const BaseValue::ShapeType& shape) const {
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

  void BooleanNode::set_option(const std::string& option_value, const std::string& option_units, Environment& env) {
    throw std::runtime_error("Option property is not implemented for boolean nodes: "+line.code);
  }
  
  BaseNode::PointerType BooleanNode::clone(const std::string& nm) const {
    if (value==nullptr) 
      return std::make_shared<BooleanNode>(nm, nullptr);
    else
      return std::make_shared<BooleanNode>(nm, std::move(value->clone()));
  }
  
  void BooleanNode::validate_options() const {
    if (format.size()>0)
      throw std::runtime_error("Options property is not implemented for boolean nodes: "+line.code);
  }
  
}
