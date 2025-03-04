#include "nodes.h"

#include <sstream>

namespace dip {

  void ValueNode::tokenize_array_values(const std::string& str, std::vector<std::string>& value_inputs, std::vector<int>& shape) {
    std::stringstream ss(str);
    char ch;
    
    std::string value;
    bool inString = false;
    int  dim = 0, item = 0;
    while (ss >> ch) {
      if (ch == '[') {
	dim++;
	if (shape.size()<dim)
	  shape.push_back(0);
	value.clear();
      } else if (ch == ',') {
	if (!value.empty()) {
	  value_inputs.push_back(value);
	  value.clear();
	}
	shape[dim-1]++;
      } else if (ch == ']') {
	if (!value.empty()) {
	  value_inputs.push_back(value);
	  value.clear();
	}
	shape[dim-1]++;
	dim--;
      } else if (ch == '"') {
	value.clear();
	while (ss >> ch && ch != '"') {
	  value += ch;
	}
      } else if (ch == '\'') {
	value.clear();
	while (ss >> ch && ch != '\'') {
	  value += ch;
	}
      } else if (ch == ' ') {
	continue;
      } else {
	value += ch;
      }
    }

    // Check if all nested arrays are closed
    if (dim!=0)
      throw std::runtime_error("Definition of an array has some unclosed brackets or quotes: "+str);

    // Normalize shape and check coherence of nested arrays
    int coef = 1;
    for (int d=1; d<shape.size(); d++) {
      coef *= shape[d-1];
      if (shape[d]%coef != 0) 
	throw std::runtime_error("Items in dimension "+std::to_string(d+1)+" do not have the same shape: "+str);
      shape[d] /= coef;
    }
  }

  std::unique_ptr<BaseValue> ValueNode::cast_value() {
    return cast_value(value_raw);
  }

  std::unique_ptr<BaseValue> ValueNode::cast_value(std::string value_input) {
    if (dimension.size()>0) {
      std::vector<std::string> value_inputs;
      std::vector<int> shape;
      ValueNode::tokenize_array_values(value_input, value_inputs, shape);
      return cast_array_value(value_inputs, shape);
    } else {
      return cast_scalar_value(value_input);
    }

  }

  void ValueNode::set_value(std::unique_ptr<BaseValue> value_input) {
    if (value_input==nullptr and value_raw.length()>0)
      value = cast_value();
    else if (value_input!=nullptr)
      value = std::move(value_input);
    else
      value = nullptr;
  }

  void ValueNode::modify_value(std::shared_ptr<BaseNode> node, Environment& env) {
    if (node->dtype!=Node::NODE_MODIFICATION and node->dtype!=dtype)
	throw std::runtime_error("Node '"+name+"' with type '"+dtype_raw+"' cannot modify node '"+node->name+"' with type '"+node->dtype_raw+"'");
    std::unique_ptr<BaseValue> value = cast_value(node->value_raw);
    // TODO: add conversion to original units
    set_value(std::move(value));
  }

  void ValueNode::validate_constant() {
    if (constant)
      throw std::runtime_error("Node '"+name+"' is constant and cannot be modified: "+line.to_string());
  }
  
  void ValueNode::validate_definition() {
    if (declared and value==nullptr) 
      throw std::runtime_error("Declared node has undefined value: "+line.to_string());
  }
  
  void ValueNode::validate_options() {
    if (options.size()>0) {
      bool match = false;
      for (int i=0; i<options.size(); i++) {
	if (options[i].value and value->equals_to(options[i].value.get()))
	  match = true;
      }
      if (!match) {
	std::ostringstream oss;
	for (int i=0; i<options.size(); i++) {
	  if (i>0) oss << ", ";
	  oss << options[i].value->to_string();
	}
	throw std::runtime_error("Value '"+value->to_string()+"' of node '"+name+"' doesn't match with any option: "+oss.str());
      }
    }
  }

  void ValueNode::validate_format() {
    if (format.size()>0)
      throw std::runtime_error("Format property can be used only with string nodes: "+line.to_string());
  }

}
