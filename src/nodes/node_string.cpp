#include <regex>

#include "nodes.h"
#include "../solvers/solvers.h"

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
    if (!units_raw.empty())
      throw std::runtime_error("String data type does not support units: "+line.code);
    if (!value_func.empty()) {
      FunctionSolver solver(env);
      FunctionList::FunctionReturnType return_value;
      return_value = solver.solve(value_func);
      if (std::holds_alternative<std::unique_ptr<BaseValue>>(return_value)) {
	std::unique_ptr<BaseValue>& ptr = std::get<std::unique_ptr<BaseValue>>(return_value);
	set_value(std::move(ptr));
      } else {
	throw std::runtime_error("Function solver can return only a single value pointer to a string node");
      }
    }
    // TODO: process expression
    return {};
  }

  std::unique_ptr<BaseValue> StringNode::cast_scalar_value(const std::string value_input) {
    return std::make_unique<ScalarValue<std::string>>(value_input, value_dtype);
  }

  std::unique_ptr<BaseValue> StringNode::cast_array_value(const std::vector<std::string>& value_inputs, const std::vector<int>& shape) {      
    return std::make_unique<ArrayValue<std::string>>(value_inputs, shape, value_dtype);
  }
  
  void StringNode::set_option(const std::string option_value, const std::string option_units, Environment& env) {
    std::unique_ptr<BaseValue> ovalue = std::make_unique<ScalarValue<std::string>>(option_value, value_dtype);
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

  void StringNode::validate_datatype() {
    if (value->dtype!=value_dtype)
      throw std::runtime_error("Value set to a string node must have data type ID="+std::to_string(value_dtype)+". Current data type ID is: "+std::to_string(value->dtype));
  }  
  
}
