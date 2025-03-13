#include "nodes.h"
#include "../solvers/solvers.h"

namespace dip {

  std::shared_ptr<BaseNode> IntegerNode::is_node(Parser& parser) {
    if (parser.has_dtype(Node::NODE_INTEGER)) {
      parser.part_dimension();
      parser.part_equal();
      if (parser.is_parsed(Parser::PART_EQUAL))
	parser.part_value();
      parser.part_units();
      parser.part_comment();
      return std::make_shared<IntegerNode>(parser);
    }
    return nullptr;
  }
  
  IntegerNode::IntegerNode(Parser& parser): BaseNode(parser, Node::NODE_INTEGER) {
    if (dtype_prop[1]=="16") {
      value_dtype = (dtype_prop[0]=="u") ? BaseValue::VALUE_UINT16 : BaseValue::VALUE_INT16;
    } else if (dtype_prop[1]=="32" or dtype_prop[1]=="") {
      value_dtype = (dtype_prop[0]=="u") ? BaseValue::VALUE_UINT32 : BaseValue::VALUE_INT32;
    } else if (dtype_prop[1]=="64") {
      value_dtype = (dtype_prop[0]=="u") ? BaseValue::VALUE_UINT64 : BaseValue::VALUE_INT64;
    } else {
      throw std::runtime_error("Value data type cannot be determined from the node settings");
    }
  };
  
  BaseNode::NodeListType IntegerNode::parse(Environment& env) {
    if (!value_func.empty()) {
      FunctionSolver solver(env);
      set_value(solver.solve_value(value_func));
    }
    // TODO: process expression
    // TODO: process units
    return {};
  }  
    
  BaseValue::PointerType IntegerNode::cast_scalar_value(const std::string value_input) {    
    // TODO: variable precision x should be implemented
    switch (value_dtype) {
    case BaseValue::VALUE_UINT16:
      return std::make_unique<ScalarValue<unsigned short>>((unsigned short)std::stoi(value_input), BaseValue::VALUE_UINT16);
      break;
    case BaseValue::VALUE_INT16:
      return std::make_unique<ScalarValue<short>>((short)std::stoi(value_input), BaseValue::VALUE_INT16);
      break;
    case BaseValue::VALUE_UINT32:
      return std::make_unique<ScalarValue<unsigned int>>(std::stoi(value_input), BaseValue::VALUE_UINT32);
      break;
    case BaseValue::VALUE_INT32:
      return std::make_unique<ScalarValue<int>>(std::stoi(value_input), BaseValue::VALUE_INT32);
      break;
    case BaseValue::VALUE_UINT64:
      return std::make_unique<ScalarValue<unsigned long long>>(std::stoull(value_input), BaseValue::VALUE_UINT64);
      break;
    case BaseValue::VALUE_INT64:
      return std::make_unique<ScalarValue<long long>>(std::stoll(value_input), BaseValue::VALUE_INT64);
      break;
    default:
      if (dtype_prop[0]=="u")
	throw std::runtime_error("Value cannot be casted as unsigned "+dtype_prop[0]+" bit integer type from the given string: "+value_input);
      else
	throw std::runtime_error("Value cannot be casted as "+dtype_prop[0]+" bit integer type from the given string: "+value_input);
    }
  }

  BaseValue::PointerType IntegerNode::cast_array_value(const std::vector<std::string>& value_inputs, const std::vector<int>& shape) {
    // TODO: variable precision x should be implemented
    switch (value_dtype) {
    case BaseValue::VALUE_UINT16: {
	std::vector<unsigned short> arr;
	for (auto s: value_inputs) arr.push_back((unsigned short)std::stoul(s));
	return std::make_unique<ArrayValue<unsigned short>>(arr, shape, BaseValue::VALUE_UINT16);
    }
    case BaseValue::VALUE_INT16: {
	std::vector<short> arr;
	for (auto s: value_inputs) arr.push_back((short)std::stoi(s));
	return std::make_unique<ArrayValue<short>>(arr, shape, BaseValue::VALUE_INT16);
    }
    case BaseValue::VALUE_UINT32: {
	std::vector<unsigned int> arr;
	for (auto s: value_inputs) arr.push_back(std::stoul(s));
	return std::make_unique<ArrayValue<unsigned int>>(arr, shape, BaseValue::VALUE_UINT32);
    }
    case BaseValue::VALUE_INT32: {
	std::vector<int> arr;
	for (auto s: value_inputs) arr.push_back(std::stoi(s));
	return std::make_unique<ArrayValue<int>>(arr, shape, BaseValue::VALUE_INT32);
    }
    case BaseValue::VALUE_UINT64: {
	std::vector<unsigned long long> arr;
	for (auto s: value_inputs) arr.push_back(std::stoull(s));
	return std::make_unique<ArrayValue<unsigned long long>>(arr, shape, BaseValue::VALUE_UINT64);
    }
    case BaseValue::VALUE_INT64: {
	std::vector<long long> arr;
	for (auto s: value_inputs) arr.push_back(std::stoll(s));
	return std::make_unique<ArrayValue<long long>>(arr, shape, BaseValue::VALUE_INT64);
    }
    default:
      std::ostringstream oss;
      for (auto s: value_inputs)
	oss << s;
      if (dtype_prop[0]=="u") {
	throw std::runtime_error("Value cannot be casted as unsigned "+dtype_prop[0]+" bit integer type from the given string: "+oss.str());
      } else {
	throw std::runtime_error("Value cannot be casted as "+dtype_prop[0]+" bit integer type from the given string: "+oss.str());
      }
    }    
  }

  void IntegerNode::set_option(const std::string option_value, const std::string option_units, Environment& env) {
    // TODO: variable precision x should be implemented
    BaseValue::PointerType ovalue;
    switch (value_dtype) {
    case BaseValue::VALUE_UINT16:
	ovalue = std::make_unique<ScalarValue<unsigned short>>((unsigned short)std::stoul(option_value), BaseValue::VALUE_UINT16);
	break;
    case BaseValue::VALUE_INT16:
	ovalue = std::make_unique<ScalarValue<short>>((short)std::stoi(option_value), BaseValue::VALUE_INT16);
	break;
    case BaseValue::VALUE_UINT32:
	ovalue = std::make_unique<ScalarValue<unsigned int>>(std::stoul(option_value), BaseValue::VALUE_UINT32);
	break;
    case BaseValue::VALUE_INT32:
	ovalue = std::make_unique<ScalarValue<int>>(std::stoi(option_value), BaseValue::VALUE_INT32);
	break;
    case BaseValue::VALUE_UINT64:
	ovalue = std::make_unique<ScalarValue<unsigned long long>>(std::stoull(option_value), BaseValue::VALUE_UINT64);
	break;
    case BaseValue::VALUE_INT64:
	ovalue = std::make_unique<ScalarValue<long long>>(std::stoll(option_value), BaseValue::VALUE_INT64);
	break;
    default:
      if (dtype_prop[0]=="u")
	throw std::runtime_error("Option value cannot be casted as unsigned "+dtype_prop[0]+" bit integer type from the given string: "+option_value);
      else
	throw std::runtime_error("Option value cannot be casted as "+dtype_prop[0]+" bit integer type from the given string: "+option_value);
    }
    // TODO: cast option value into the units of the node
    options.push_back({std::move(ovalue), option_value, option_units});
  }
  
}
