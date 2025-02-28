#include "nodes.h"

namespace dip {

  std::shared_ptr<BaseNode> IntegerNode::is_node(Parser& parser) {
    if (parser.has_dtype(Node::NODE_INTEGER)) {
      parser.part_dimension();
      parser.part_equal();
      if (parser.is_parsed(Parser::PART_EQUAL))
	parser.part_value();
      else 
	parser.declared = true;
      parser.part_units();
      parser.part_comment();
      return std::make_shared<IntegerNode>(parser);
    }
    return nullptr;
  }

  // TODO: set_value
  
  BaseNode::NodeListType IntegerNode::parse(Environment& env) {
    // TODO: process function
    // TODO: process expression
    // TODO: process units
    return {};
  }  
  
  std::unique_ptr<BaseValue> IntegerNode::cast_scalar_value(const std::string value_input) {    
    // TODO: variable precision x should be implemented
    if (dtype_prop[1]=="16") {
      if (dtype_prop[0]=="u")
	return std::make_unique<ScalarValue<unsigned short>>((unsigned short)std::stoi(value_input), BaseValue::VALUE_UINT16);
      else
	return std::make_unique<ScalarValue<short>>((short)std::stoi(value_input), BaseValue::VALUE_INT16);
    } else if (dtype_prop[1]=="32" or dtype_prop[1]=="") {
      if (dtype_prop[0]=="u")
	return std::make_unique<ScalarValue<unsigned int>>(std::stoi(value_input), BaseValue::VALUE_UINT32);
      else
	return std::make_unique<ScalarValue<int>>(std::stoi(value_input), BaseValue::VALUE_INT32);
    } else if (dtype_prop[1]=="64") {
      if (dtype_prop[0]=="u")
	return std::make_unique<ScalarValue<unsigned long long>>(std::stoull(value_input), BaseValue::VALUE_UINT64);
      else
	return std::make_unique<ScalarValue<long long>>(std::stoll(value_input), BaseValue::VALUE_INT64);
    } else {
      if (dtype_prop[0]=="u")
	throw std::runtime_error("Value cannot be casted as unsigned "+dtype_prop[0]+" bit integer type from the given string: "+value_input);
      else
	throw std::runtime_error("Value cannot be casted as "+dtype_prop[0]+" bit integer type from the given string: "+value_input);
    }
  }

  std::unique_ptr<BaseValue> IntegerNode::cast_array_value(const std::vector<std::string>& value_inputs, const std::vector<int>& shape) {
    // TODO: variable precision x should be implemented
    if (dtype_prop[1]=="16") {
      if (dtype_prop[0]=="u") {
	std::vector<unsigned short> arr;
	for (auto s: value_inputs) arr.push_back((unsigned short)std::stoi(s));
	return std::make_unique<ArrayValue<unsigned short>>(arr, shape, BaseValue::VALUE_UINT16);
      } else {
	std::vector<short> arr;
	for (auto s: value_inputs) arr.push_back((short)std::stoi(s));
	return std::make_unique<ArrayValue<short>>(arr, shape, BaseValue::VALUE_INT16);
      }
    } else if (dtype_prop[1]=="32" or dtype_prop[1]=="") {
      if (dtype_prop[0]=="u") {
	std::vector<unsigned int> arr;
	for (auto s: value_inputs) arr.push_back(std::stoul(s));
	return std::make_unique<ArrayValue<unsigned int>>(arr, shape, BaseValue::VALUE_UINT32);
      } else {
	std::vector<int> arr;
	for (auto s: value_inputs) arr.push_back(std::stoi(s));
	return std::make_unique<ArrayValue<int>>(arr, shape, BaseValue::VALUE_INT32);
      }
    } else if (dtype_prop[1]=="64") {
      if (dtype_prop[0]=="u") {
	std::vector<unsigned long long> arr;
	for (auto s: value_inputs) arr.push_back(std::stoull(s));
	return std::make_unique<ArrayValue<unsigned long long>>(arr, shape, BaseValue::VALUE_UINT64);
      } else {
	std::vector<long long> arr;
	for (auto s: value_inputs) arr.push_back(std::stoll(s));
	return std::make_unique<ArrayValue<long long>>(arr, shape, BaseValue::VALUE_INT64);
      }
    } else {
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
  
}
