#include <regex>

#include "nodes.h"
#include "../parsers.h"
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
    switch (value_origin) {
    case ValueOrigin::Function:
      set_value(env.request_value(value_raw.at(0), RequestType::Function));
      break;
    case ValueOrigin::Reference:
      set_value(env.request_value(value_raw.at(0), RequestType::Reference));
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
      // TODO: implement template expression
      break;
    }
    default:
      break;
    }
    // TODO: process expression
    return {};
  }

  BaseValue::PointerType StringNode::cast_scalar_value(const std::string& value_input) const {
    return std::make_unique<ScalarValue<std::string>>(value_input, value_dtype);
  }

  BaseValue::PointerType StringNode::cast_array_value(const Array::StringType& value_inputs, const Array::ShapeType& shape) const {      
    return std::make_unique<ArrayValue<std::string>>(value_inputs, shape, value_dtype);
  }
  
  BaseNode::PointerType StringNode::clone(const std::string& nm) const {
    if (value==nullptr) 
      return std::make_shared<StringNode>(nm, nullptr);
    else
      return std::make_shared<StringNode>(nm, std::move(value->clone()));
  }

  bool StringNode::set_property(PropertyType property, Array::StringType& values, std::string& units) {
    if (ValueNode::set_property(property, values, units)) {
      return true;
    } else if (property==PropertyType::Format) {
      this->format = values.at(0);
      return true;
    } else {
      return false;
    }
  }
  
  void StringNode::validate_format() const {
    if (format.size()>0) {
      std::regex pattern(format);
      if (!std::regex_match(value->to_string(), pattern)) {
	throw std::runtime_error("Node value '"+value->to_string()+"' does not match with expected format '"+format+"'");
      }
    }
  }
  
}
