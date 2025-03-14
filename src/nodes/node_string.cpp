#include <regex>

#include "nodes.h"
#include "../solvers/solvers.h"

namespace dip {

  BaseNode::PointerType StringNode::is_node(Parser& parser) {
    if (parser.dtype_raw[1]=="str") {
      parser.part_dimension();
      if (parser.part_equal(false))
	parser.part_value();
      parser.part_units();
      parser.part_comment();
      return std::make_shared<StringNode>(parser);
    }
    return nullptr;
  }
  
  BaseNode::NodeListType StringNode::parse(Environment& env) {
    if (!units_raw.empty())
      throw std::runtime_error("String data type does not support units: "+line.code);
    if (!value_func.empty()) {
      FunctionSolver solver(env);
      set_value(solver.solve_value(value_func));
    }
    // TODO: process expression
    return {};
  }

  BaseValue::PointerType StringNode::cast_scalar_value(const std::string value_input) {
    return std::make_unique<ScalarValue<std::string>>(value_input, value_dtype);
  }

  BaseValue::PointerType StringNode::cast_array_value(const std::vector<std::string>& value_inputs, const std::vector<int>& shape) {      
    return std::make_unique<ArrayValue<std::string>>(value_inputs, shape, value_dtype);
  }
  
  void StringNode::set_option(const std::string option_value, const std::string option_units, Environment& env) {
    BaseValue::PointerType ovalue = std::make_unique<ScalarValue<std::string>>(option_value, value_dtype);
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
