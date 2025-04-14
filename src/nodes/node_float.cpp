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

  FloatNode::FloatNode(Parser& parser): BaseNode(parser, NodeDtype::Float) {
    if (dtype_raw[2]=="32") {
      value_dtype = ValueDtype::Float32;
    } else if (dtype_raw[2]=="64" or dtype_raw[2]=="") {
      value_dtype = ValueDtype::Float64;
    } else if (dtype_raw[2]=="128" and max_float_size==128) {
      value_dtype = ValueDtype::Float128;
    } else {
      throw std::runtime_error("Value data type cannot be determined from the node settings");
    }
  };
  
  BaseNode::NodeListType FloatNode::parse(Environment& env) {
    switch (value_origin) {
    case ValueOrigin::Function:
      set_value(env.request_value(value_raw.at(0), RequestType::Function, units_raw));
      break;
    case ValueOrigin::Reference:
      set_value(env.request_value(value_raw.at(0), RequestType::Reference, units_raw));
      break;
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
  
  BaseValue::PointerType FloatNode::cast_scalar_value(const std::string& value_input) const {
    // TODO: variable precision x should be implemented
    switch (value_dtype) {
    case ValueDtype::Float32:
      return std::make_unique<ScalarValue<float>>(std::stof(value_input), ValueDtype::Float32);
    case ValueDtype::Float64:
      return std::make_unique<ScalarValue<double>>(std::stod(value_input), ValueDtype::Float64);
    case ValueDtype::Float128:
      return std::make_unique<ScalarValue<long double>>(std::stold(value_input), ValueDtype::Float128);
    default:
      throw std::runtime_error("Value cannot be casted as "+dtype_raw[2]+" bit floating-point type from the given string: "+value_input);
    }
  }
  
  BaseValue::PointerType FloatNode::cast_array_value(const Array::StringType& value_inputs, const Array::ShapeType& shape) const {
    // TODO: variable precision x should be implemented
    switch (value_dtype) {
    case ValueDtype::Float32: {
      std::vector<float> arr;
      arr.reserve(value_inputs.size());
      for (auto s: value_inputs) arr.push_back(std::stof(s));
      return std::make_unique<ArrayValue<float>>(arr, shape, ValueDtype::Float32);
    }
    case ValueDtype::Float64: {
      std::vector<double> arr;
      arr.reserve(value_inputs.size());
      for (auto s: value_inputs) arr.push_back(std::stod(s));
      return std::make_unique<ArrayValue<double>>(arr, shape, ValueDtype::Float64);
    }
    case ValueDtype::Float128: {
      std::vector<long double> arr;
      arr.reserve(value_inputs.size());
      for (auto s: value_inputs) arr.push_back(std::stold(s));
      return std::make_unique<ArrayValue<long double>>(arr, shape, ValueDtype::Float128);
    }
    default:
      std::ostringstream oss;
      for (auto s: value_inputs)
	oss << s;
      throw std::runtime_error("Value cannot be casted as "+dtype_raw[2]+" bit floating-point type from the given string: "+oss.str());
    }
  }
  
  BaseNode::PointerType FloatNode::clone(const std::string& nm) const {
    if (value==nullptr) 
      return std::make_shared<FloatNode>(nm, nullptr, value->dtype);
    else
      return std::make_shared<FloatNode>(nm, std::move(value->clone()), value->dtype);
  }
  
}
