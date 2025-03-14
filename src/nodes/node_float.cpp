#include "nodes.h"
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
    if (!value_func.empty()) {
      FunctionSolver solver(env);
      set_value(solver.solve_value(value_func));
    }
    // TODO: process expression
    // TODO: process units
    return {};
  }  
  
  BaseValue::PointerType FloatNode::cast_scalar_value(const std::string value_input) {
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
  
  BaseValue::PointerType FloatNode::cast_array_value(const std::vector<std::string>& value_inputs, const std::vector<int>& shape) {
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
    // TODO: cast option value into the units of the node
    options.push_back({std::move(ovalue), option_value, option_units});
  }
  
}
