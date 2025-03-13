#include "nodes.h"
#include "../solvers/solvers.h"

namespace dip {

  std::shared_ptr<BaseNode> FloatNode::is_node(Parser& parser) {
    if (parser.has_dtype(Node::NODE_FLOAT)) {
      parser.part_dimension();
      parser.part_equal();
      if (parser.is_parsed(Parser::PART_EQUAL))
	parser.part_value();
      parser.part_units();
      parser.part_comment();
      return std::make_shared<FloatNode>(parser);
    }
    return nullptr;
  }


  FloatNode::FloatNode(Parser& parser): BaseNode(parser, Node::NODE_FLOAT) {
    if (dtype_prop[0]=="32") {
      value_dtype = BaseValue::VALUE_FLOAT32;
    } else if (dtype_prop[0]=="64" or dtype_prop[0]=="") {
      value_dtype = BaseValue::VALUE_FLOAT64;
    } else if (dtype_prop[0]=="128" and max_float_size==128) {
      value_dtype = BaseValue::VALUE_FLOAT128;
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
    case BaseValue::VALUE_FLOAT32:
      return std::make_unique<ScalarValue<float>>(std::stof(value_input), BaseValue::VALUE_FLOAT32);
    case BaseValue::VALUE_FLOAT64:
      return std::make_unique<ScalarValue<double>>(std::stod(value_input), BaseValue::VALUE_FLOAT64);
    case BaseValue::VALUE_FLOAT128:
      return std::make_unique<ScalarValue<long double>>(std::stold(value_input), BaseValue::VALUE_FLOAT128);
    default:
      throw std::runtime_error("Value cannot be casted as "+dtype_prop[0]+" bit floating-point type from the given string: "+value_input);
    }
  }
  
  BaseValue::PointerType FloatNode::cast_array_value(const std::vector<std::string>& value_inputs, const std::vector<int>& shape) {
    // TODO: variable precision x should be implemented
    switch (value_dtype) {
    case BaseValue::VALUE_FLOAT32: {
      std::vector<float> arr;
      for (auto s: value_inputs) arr.push_back(std::stof(s));
      return std::make_unique<ArrayValue<float>>(arr, shape, BaseValue::VALUE_FLOAT32);
    }
    case BaseValue::VALUE_FLOAT64: {
      std::vector<double> arr;
      for (auto s: value_inputs) arr.push_back(std::stod(s));
      return std::make_unique<ArrayValue<double>>(arr, shape, BaseValue::VALUE_FLOAT64);
    }
    case BaseValue::VALUE_FLOAT128: {
      std::vector<long double> arr;
      for (auto s: value_inputs) arr.push_back(std::stold(s));
      return std::make_unique<ArrayValue<long double>>(arr, shape, BaseValue::VALUE_FLOAT128);
    }
    default:
      std::ostringstream oss;
      for (auto s: value_inputs)
	oss << s;
      throw std::runtime_error("Value cannot be casted as "+dtype_prop[0]+" bit floating-point type from the given string: "+oss.str());
    }
  }
  
  void FloatNode::set_option(const std::string option_value, const std::string option_units, Environment& env) {
    // TODO: variable precision x should be implemented
    BaseValue::PointerType ovalue;
    switch (value_dtype) {
    case BaseValue::VALUE_FLOAT32:
      ovalue = std::make_unique<ScalarValue<float>>(std::stof(option_value), BaseValue::VALUE_FLOAT32);
      break;
    case BaseValue::VALUE_FLOAT64:
      ovalue = std::make_unique<ScalarValue<double>>(std::stod(option_value), BaseValue::VALUE_FLOAT64);
      break;
    case BaseValue::VALUE_FLOAT128:
      ovalue = std::make_unique<ScalarValue<long double>>(std::stold(option_value), BaseValue::VALUE_FLOAT128);
      break;
    default:
      throw std::runtime_error("Option value cannot be casted as "+dtype_prop[0]+" bit float type from the given string: "+option_value);
    }
    // TODO: cast option value into the units of the node
    options.push_back({std::move(ovalue), option_value, option_units});
  }
  
}
