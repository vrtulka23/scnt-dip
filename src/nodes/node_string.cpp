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
      parser.part_units();
      parser.part_comment();
      return std::make_shared<StringNode>(parser);
    }
    return nullptr;
  }

  std::shared_ptr<BaseNode> StringNode::create_scalar(const std::string& name, const std::string value) {
    std::unique_ptr<ScalarValue<std::string>> ptr_value = std::make_unique<ScalarValue<std::string>>(value);
    std::shared_ptr<dip::StringNode> ptr_node = std::make_shared<dip::StringNode>(name, std::move(ptr_value));
    return std::move(ptr_node);
  }
  
  std::shared_ptr<BaseNode> StringNode::create_array(const std::string& name, const std::vector<std::string>&  arr, std::vector<int> sh) {
    if (sh.empty())
      sh.push_back(arr.size());
    std::unique_ptr<BaseValue> ptr_value = ArrayValue<std::string>::create(arr,sh);
    std::shared_ptr<dip::StringNode> ptr_node = std::make_shared<dip::StringNode>(name, std::move(ptr_value));
    return ptr_node;
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
