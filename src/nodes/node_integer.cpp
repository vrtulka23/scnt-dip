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
  
  IntegerNode::IntegerNode(Parser& parser): BaseNode(parser, NodeDtype::Integer) {
    if (dtype_raw[2]=="16") {
      value_dtype = (dtype_raw[0]=="u") ? ValueDtype::Integer16_U : ValueDtype::Integer16;
    } else if (dtype_raw[2]=="32" or dtype_raw[2]=="") {
      value_dtype = (dtype_raw[0]=="u") ? ValueDtype::Integer32_U : ValueDtype::Integer32;
    } else if (dtype_raw[2]=="64") {
      value_dtype = (dtype_raw[0]=="u") ? ValueDtype::Integer64_U : ValueDtype::Integer64;
    } else {
      throw std::runtime_error("Value data type cannot be determined from the node settings");
    }
  };
  
  BaseNode::NodeListType IntegerNode::parse(Environment& env) {
    switch (value_origin) {
    case ValueOrigin::Function:
      set_value(env.request_value(value_raw.at(0), RequestType::Function, units_raw));
      break;
    case ValueOrigin::Reference: {
      set_value(env.request_value(value_raw.at(0), RequestType::Reference, units_raw));
      break;
    }
    case ValueOrigin::ReferenceRaw: {
      std::string source_code = env.request_code(value_raw.at(0));
      Array::StringType source_value_raw;
      Array::ShapeType source_value_shape;
      parse_value(source_code, source_value_raw, source_value_shape);
      set_value(cast_value(source_value_raw, source_value_shape));
      break;
    }
    case ValueOrigin::Expression: {
      // TODO: implement numerical expression
      break;
    }
    default:
      break;
    }
    // TODO: process expression
    return {};
  }  
    
  BaseValue::PointerType IntegerNode::cast_scalar_value(const std::string& value_input) const {
    // TODO: variable precision x should be implemented
    switch (value_dtype) {
    case ValueDtype::Integer16_U:
      return std::make_unique<ScalarValue<unsigned short>>((unsigned short)std::stoi(value_input), ValueDtype::Integer16_U);
      break;
    case ValueDtype::Integer16:
      return std::make_unique<ScalarValue<short>>((short)std::stoi(value_input), ValueDtype::Integer16);
      break;
    case ValueDtype::Integer32_U:
      return std::make_unique<ScalarValue<unsigned int>>(std::stoi(value_input), ValueDtype::Integer32_U);
      break;
    case ValueDtype::Integer32:
      return std::make_unique<ScalarValue<int>>(std::stoi(value_input), ValueDtype::Integer32);
      break;
    case ValueDtype::Integer64_U:
      return std::make_unique<ScalarValue<unsigned long long>>(std::stoull(value_input), ValueDtype::Integer64_U);
      break;
    case ValueDtype::Integer64:
      return std::make_unique<ScalarValue<long long>>(std::stoll(value_input), ValueDtype::Integer64);
      break;
    default:
      if (dtype_raw[0]=="u")
	throw std::runtime_error("Value cannot be casted as unsigned "+dtype_raw[0]+" bit integer type from the given string: "+value_input);
      else
	throw std::runtime_error("Value cannot be casted as "+dtype_raw[0]+" bit integer type from the given string: "+value_input);
    }
  }

  BaseValue::PointerType IntegerNode::cast_array_value(const Array::StringType& value_inputs, const Array::ShapeType& shape) const {
    // TODO: variable precision x should be implemented
    switch (value_dtype) {
    case ValueDtype::Integer16_U: {
      std::vector<unsigned short> arr;
      arr.reserve(value_inputs.size());
      for (auto s: value_inputs) arr.push_back((unsigned short)std::stoul(s));
      return std::make_unique<ArrayValue<unsigned short>>(arr, shape, ValueDtype::Integer16_U);
    }
    case ValueDtype::Integer16: {
      std::vector<short> arr;
      arr.reserve(value_inputs.size());
      for (auto s: value_inputs) arr.push_back((short)std::stoi(s));
      return std::make_unique<ArrayValue<short>>(arr, shape, ValueDtype::Integer16);
    }
    case ValueDtype::Integer32_U: {
      std::vector<unsigned int> arr;
      arr.reserve(value_inputs.size());
      for (auto s: value_inputs) arr.push_back(std::stoul(s));
      return std::make_unique<ArrayValue<unsigned int>>(arr, shape, ValueDtype::Integer32_U);
    }
    case ValueDtype::Integer32: {
      std::vector<int> arr;
      arr.reserve(value_inputs.size());
      for (auto s: value_inputs) arr.push_back(std::stoi(s));
      return std::make_unique<ArrayValue<int>>(arr, shape, ValueDtype::Integer32);
    }
    case ValueDtype::Integer64_U: {
      std::vector<unsigned long long> arr;
      arr.reserve(value_inputs.size());
      for (auto s: value_inputs) arr.push_back(std::stoull(s));
      return std::make_unique<ArrayValue<unsigned long long>>(arr, shape, ValueDtype::Integer64_U);
    }
    case ValueDtype::Integer64: {
      std::vector<long long> arr;
      arr.reserve(value_inputs.size());
      for (auto s: value_inputs) arr.push_back(std::stoll(s));
      return std::make_unique<ArrayValue<long long>>(arr, shape, ValueDtype::Integer64);
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

  BaseNode::PointerType IntegerNode::clone(const std::string& nm) const {
    if (value==nullptr) 
      return std::make_shared<IntegerNode>(nm, nullptr, value->dtype);
    else
      return std::make_shared<IntegerNode>(nm, std::move(value->clone()), value->dtype);
  }
  
}
