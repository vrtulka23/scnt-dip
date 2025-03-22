#include "nodes.h"
#include "../parsers.h"
#include "../solvers/solvers.h"

namespace dip {

  BaseNode::PointerType IntegerNode::is_node(Parser& parser) {
    if (parser.dtype_raw[1]=="int") {
      parser.part_dimension();
      if (parser.part_equal(false))
	parser.part_value();
      parser.part_units();
      parser.part_comment();
      return std::make_shared<IntegerNode>(parser);
    }
    return nullptr;
  }
  
  IntegerNode::IntegerNode(Parser& parser): BaseNode(parser, BaseNode::INTEGER) {
    if (dtype_raw[2]=="16") {
      value_dtype = (dtype_raw[0]=="u") ? BaseValue::INTEGER_16_U : BaseValue::INTEGER_16;
    } else if (dtype_raw[2]=="32" or dtype_raw[2]=="") {
      value_dtype = (dtype_raw[0]=="u") ? BaseValue::INTEGER_32_U : BaseValue::INTEGER_32;
    } else if (dtype_raw[2]=="64") {
      value_dtype = (dtype_raw[0]=="u") ? BaseValue::INTEGER_64_U : BaseValue::INTEGER_64;
    } else {
      throw std::runtime_error("Value data type cannot be determined from the node settings");
    }
  };
  
  BaseNode::NodeListType IntegerNode::parse(Environment& env) {
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
    // TODO: process units
    return {};
  }  
    
  BaseValue::PointerType IntegerNode::cast_scalar_value(const std::string value_input) const {
    // TODO: variable precision x should be implemented
    switch (value_dtype) {
    case BaseValue::INTEGER_16_U:
      return std::make_unique<ScalarValue<unsigned short>>((unsigned short)std::stoi(value_input), BaseValue::INTEGER_16_U);
      break;
    case BaseValue::INTEGER_16:
      return std::make_unique<ScalarValue<short>>((short)std::stoi(value_input), BaseValue::INTEGER_16);
      break;
    case BaseValue::INTEGER_32_U:
      return std::make_unique<ScalarValue<unsigned int>>(std::stoi(value_input), BaseValue::INTEGER_32_U);
      break;
    case BaseValue::INTEGER_32:
      return std::make_unique<ScalarValue<int>>(std::stoi(value_input), BaseValue::INTEGER_32);
      break;
    case BaseValue::INTEGER_64_U:
      return std::make_unique<ScalarValue<unsigned long long>>(std::stoull(value_input), BaseValue::INTEGER_64_U);
      break;
    case BaseValue::INTEGER_64:
      return std::make_unique<ScalarValue<long long>>(std::stoll(value_input), BaseValue::INTEGER_64);
      break;
    default:
      if (dtype_raw[0]=="u")
	throw std::runtime_error("Value cannot be casted as unsigned "+dtype_raw[0]+" bit integer type from the given string: "+value_input);
      else
	throw std::runtime_error("Value cannot be casted as "+dtype_raw[0]+" bit integer type from the given string: "+value_input);
    }
  }

  BaseValue::PointerType IntegerNode::cast_array_value(const std::vector<std::string>& value_inputs, const BaseValue::ShapeType& shape) const {
    // TODO: variable precision x should be implemented
    switch (value_dtype) {
    case BaseValue::INTEGER_16_U: {
	std::vector<unsigned short> arr;
	for (auto s: value_inputs) arr.push_back((unsigned short)std::stoul(s));
	return std::make_unique<ArrayValue<unsigned short>>(arr, shape, BaseValue::INTEGER_16_U);
    }
    case BaseValue::INTEGER_16: {
	std::vector<short> arr;
	for (auto s: value_inputs) arr.push_back((short)std::stoi(s));
	return std::make_unique<ArrayValue<short>>(arr, shape, BaseValue::INTEGER_16);
    }
    case BaseValue::INTEGER_32_U: {
	std::vector<unsigned int> arr;
	for (auto s: value_inputs) arr.push_back(std::stoul(s));
	return std::make_unique<ArrayValue<unsigned int>>(arr, shape, BaseValue::INTEGER_32_U);
    }
    case BaseValue::INTEGER_32: {
	std::vector<int> arr;
	for (auto s: value_inputs) arr.push_back(std::stoi(s));
	return std::make_unique<ArrayValue<int>>(arr, shape, BaseValue::INTEGER_32);
    }
    case BaseValue::INTEGER_64_U: {
	std::vector<unsigned long long> arr;
	for (auto s: value_inputs) arr.push_back(std::stoull(s));
	return std::make_unique<ArrayValue<unsigned long long>>(arr, shape, BaseValue::INTEGER_64_U);
    }
    case BaseValue::INTEGER_64: {
	std::vector<long long> arr;
	for (auto s: value_inputs) arr.push_back(std::stoll(s));
	return std::make_unique<ArrayValue<long long>>(arr, shape, BaseValue::INTEGER_64);
    }
    default:
      std::ostringstream oss;
      for (auto s: value_inputs)
	oss << s;
      if (dtype_raw[0]=="u") {
	throw std::runtime_error("Value cannot be casted as unsigned "+dtype_raw[0]+" bit integer type from the given string: "+oss.str());
      } else {
	throw std::runtime_error("Value cannot be casted as "+dtype_raw[0]+" bit integer type from the given string: "+oss.str());
      }
    }    
  }

  void IntegerNode::set_option(const std::string option_value, const std::string option_units, Environment& env) {
    // TODO: variable precision x should be implemented
    BaseValue::PointerType ovalue;
    switch (value_dtype) {
    case BaseValue::INTEGER_16_U:
	ovalue = std::make_unique<ScalarValue<unsigned short>>((unsigned short)std::stoul(option_value), BaseValue::INTEGER_16_U);
	break;
    case BaseValue::INTEGER_16:
	ovalue = std::make_unique<ScalarValue<short>>((short)std::stoi(option_value), BaseValue::INTEGER_16);
	break;
    case BaseValue::INTEGER_32_U:
	ovalue = std::make_unique<ScalarValue<unsigned int>>(std::stoul(option_value), BaseValue::INTEGER_32_U);
	break;
    case BaseValue::INTEGER_32:
	ovalue = std::make_unique<ScalarValue<int>>(std::stoi(option_value), BaseValue::INTEGER_32);
	break;
    case BaseValue::INTEGER_64_U:
	ovalue = std::make_unique<ScalarValue<unsigned long long>>(std::stoull(option_value), BaseValue::INTEGER_64_U);
	break;
    case BaseValue::INTEGER_64:
	ovalue = std::make_unique<ScalarValue<long long>>(std::stoll(option_value), BaseValue::INTEGER_64);
	break;
    default:
      if (dtype_raw[0]=="u")
	throw std::runtime_error("Option value cannot be casted as unsigned "+dtype_raw[0]+" bit integer type from the given string: "+option_value);
      else
	throw std::runtime_error("Option value cannot be casted as "+dtype_raw[0]+" bit integer type from the given string: "+option_value);
    }
    // TODO: cast option value into the units of the node
    options.push_back({std::move(ovalue), option_value, option_units});
  }
  
  BaseNode::PointerType IntegerNode::clone(const std::string& nm) const {
    if (value==nullptr) 
      return std::make_shared<IntegerNode>(nm, nullptr, value->dtype);
    else
      return std::make_shared<IntegerNode>(nm, std::move(value->clone()), value->dtype);
  }
  
}
