#include "nodes.h"
#include "../parsers.h"
#include "../solvers/solvers.h"

namespace dip {

  BaseNode::PointerType FloatNode::is_node(Parser& parser) {
    if (parser.dtype_raw[1]=="float") { 
      parser.part_dimension();
      if (parser.part_equal(false))
	parser.part_value();
      parser.part_units();
      parser.part_comment();
      return std::make_shared<FloatNode>(parser);
    }
    return nullptr;
  }

  FloatNode::FloatNode(Parser& parser): BaseNode(parser, BaseNode::FLOAT) {
    if (dtype_raw[2]=="32") {
      value_dtype = BaseValue::FLOAT_32;
    } else if (dtype_raw[2]=="64" or dtype_raw[2]=="") {
      value_dtype = BaseValue::FLOAT_64;
    } else if (dtype_raw[2]=="128" and max_float_size==128) {
      value_dtype = BaseValue::FLOAT_128;
    } else {
      throw std::runtime_error("Value data type cannot be determined from the node settings");
    }
  };
  
  BaseNode::NodeListType FloatNode::parse(Environment& env) {
    switch (value_origin) {
    case ValueOrigin::FUNCTION:
      set_value(env.request_value(value_raw.at(0), Environment::FUNCTION, units_raw));
      break;
    case ValueOrigin::REFERENCE:
      set_value(env.request_value(value_raw.at(0), Environment::REFERENCE, units_raw));
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
  
  BaseValue::PointerType FloatNode::cast_scalar_value(const std::string value_input) const {
    // TODO: variable precision x should be implemented
    switch (value_dtype) {
    case BaseValue::FLOAT_32:
      return std::make_unique<ScalarValue<float>>(std::stof(value_input), BaseValue::FLOAT_32);
    case BaseValue::FLOAT_64:
      return std::make_unique<ScalarValue<double>>(std::stod(value_input), BaseValue::FLOAT_64);
    case BaseValue::FLOAT_128:
      return std::make_unique<ScalarValue<long double>>(std::stold(value_input), BaseValue::FLOAT_128);
    default:
      throw std::runtime_error("Value cannot be casted as "+dtype_raw[2]+" bit floating-point type from the given string: "+value_input);
    }
  }
  
  BaseValue::PointerType FloatNode::cast_array_value(const std::vector<std::string>& value_inputs, const BaseValue::ShapeType& shape) const {
    // TODO: variable precision x should be implemented
    switch (value_dtype) {
    case BaseValue::FLOAT_32: {
      std::vector<float> arr;
      for (auto s: value_inputs) arr.push_back(std::stof(s));
      return std::make_unique<ArrayValue<float>>(arr, shape, BaseValue::FLOAT_32);
    }
    case BaseValue::FLOAT_64: {
      std::vector<double> arr;
      for (auto s: value_inputs) arr.push_back(std::stod(s));
      return std::make_unique<ArrayValue<double>>(arr, shape, BaseValue::FLOAT_64);
    }
    case BaseValue::FLOAT_128: {
      std::vector<long double> arr;
      for (auto s: value_inputs) arr.push_back(std::stold(s));
      return std::make_unique<ArrayValue<long double>>(arr, shape, BaseValue::FLOAT_128);
    }
    default:
      std::ostringstream oss;
      for (auto s: value_inputs)
	oss << s;
      throw std::runtime_error("Value cannot be casted as "+dtype_raw[2]+" bit floating-point type from the given string: "+oss.str());
    }
  }
  
  void FloatNode::set_option(const std::string option_value, const std::string option_units, Environment& env) {
    // TODO: variable precision x should be implemented
    BaseValue::PointerType ovalue;
    switch (value_dtype) {
    case BaseValue::FLOAT_32:
      ovalue = std::make_unique<ScalarValue<float>>(std::stof(option_value), BaseValue::FLOAT_32);
      break;
    case BaseValue::FLOAT_64:
      ovalue = std::make_unique<ScalarValue<double>>(std::stod(option_value), BaseValue::FLOAT_64);
      break;
    case BaseValue::FLOAT_128:
      ovalue = std::make_unique<ScalarValue<long double>>(std::stold(option_value), BaseValue::FLOAT_128);
      break;
    default:
      throw std::runtime_error("Option value cannot be casted as "+dtype_raw[2]+" bit float type from the given string: "+option_value);
    }
    QuantityNode* qnode = dynamic_cast<QuantityNode*>(this);
    if (qnode and !option_units.empty()) {
      if (qnode->units==nullptr)
	throw std::runtime_error("Trying to convert '"+option_units+"' into a nondimensional quantity: "+line.code);
      else
	ovalue->convert_units(option_units, qnode->units);
    }
    options.push_back({std::move(ovalue), option_value, option_units});
  }
  
  BaseNode::PointerType FloatNode::clone(const std::string& nm) const {
    if (value==nullptr) 
      return std::make_shared<FloatNode>(nm, nullptr, value->dtype);
    else
      return std::make_shared<FloatNode>(nm, std::move(value->clone()), value->dtype);
  }
  
}
