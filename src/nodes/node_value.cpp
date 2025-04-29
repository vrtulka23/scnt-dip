#include "nodes.h"

#include <sstream>

namespace dip {

  ValueNode::ValueNode(const std::string& nm, BaseValue::PointerType val, const ValueDtype vdt): constant(false), value_dtype(vdt) {
    name=nm;
    Array::ShapeType dims = val->get_shape();
    if (val->get_size()>1) {
      dimension.clear();
      for (int dim: dims)
	dimension.push_back({dim, dim});
    }
    set_value(std::move(val));
  };

  BaseValue::PointerType ValueNode::cast_value() {
    return cast_value(value_raw, value_shape);
  }

  BaseValue::PointerType ValueNode::cast_value(Array::StringType& value_input, const Array::ShapeType& shape) {
    if (!dimension.empty()) {
      return cast_array_value(value_input, shape);
    } else if (value_input.size()>1) {
      throw std::runtime_error("Value size is an array but node is defined as scalar: "+line.code);
    } else {
      return cast_scalar_value(value_input.at(0));
    }
  }

  void ValueNode::set_value(BaseValue::PointerType value_input) {
    value = nullptr;
    if (value_input==nullptr and !value_raw.empty() and !value_raw.at(0).empty()) {
      value = cast_value();
    } else if (value_input!=nullptr) {
      value = std::move(value_input);
      if (value->dtype!=value_dtype) {
	std::string d1 = std::string(ValueDtypeNames[value_dtype]);
	std::string d2 = std::string(ValueDtypeNames[value->dtype]);
	throw std::runtime_error("Assigning '"+d2+"' value to the '"+d1+"' node: "+line.code);
      }
    }
    if (value!=nullptr) {
      if (!value_slice.empty())
	value = value->slice(value_slice);
      if (dimension.empty()) {
	if (value->get_size()>1)
	  throw std::runtime_error("Assigning array value to the scalar node: "+line.code);
      } else {
	validate_dimensions(); // check if value shape corresponds with dimension ranges
      }
    }
  }

  void ValueNode::modify_value(BaseNode::PointerType node, Environment& env) {
    if (node->dtype!=NodeDtype::Modification and node->dtype!=dtype)
      throw std::runtime_error("Node '"+name+"' with type '"+dtype_raw.at(1)+"' cannot modify node '"+node->name+"' with type '"+node->dtype_raw.at(1)+"'");
    BaseValue::PointerType value = cast_value(node->value_raw, node->value_shape);
    QuantityNode* qnode = dynamic_cast<QuantityNode*>(this);
    if (qnode and !node->units_raw.empty()) {
      if (qnode->units==nullptr)
	throw std::runtime_error("Trying to convert '"+node->units_raw+"' into a nondimensional quantity: "+line.code);
      else
	value->convert_units(node->units_raw, qnode->units);
    }
    value_raw = node->value_raw;
    set_value(std::move(value));
  }

  bool ValueNode::set_property(PropertyType property, Array::StringType& values, std::string& units) {
    switch (property) {
    case PropertyType::Options:
      for (auto value_option: values) {
	if (dtype==NodeDtype::Boolean)
	      throw std::runtime_error("Option property is not implemented for boolean nodes: "+line.code);
	// TODO: account for multidimensional arrays as individual options
	BaseValue::PointerType ovalue = cast_scalar_value(value_option);
	options.push_back({std::move(ovalue), value_option, units});
      }
      return true;
    case PropertyType::Constant:
      constant = true;
      return true;
    case PropertyType::Tags:
      tags = values;
      return true;
    case PropertyType::Description:
      description += values.at(0);
      return true;
    case PropertyType::Condition:
      condition = values.at(0);
      return true;
    default:
      return false;
    }
  }
  
  /*
   * Validation of node properties and values
   */
  
  void ValueNode::validate_constant() const {
    if (constant)
      throw std::runtime_error("Node '"+name+"' is constant and cannot be modified: "+line.code);
  }
  
  void ValueNode::validate_definition() const {
    if (value==nullptr) 
      throw std::runtime_error("Declared node has undefined value: "+line.code);
  }

  void ValueNode::validate_condition() const {
    if (!condition.empty()) {
      if (condition==KEYWORD_FALSE)
	throw std::runtime_error("Node does not satisfy the given condition: "+condition);
      else if (condition==KEYWORD_TRUE)
	return;
      // TODO: implement expression solver
    }
  }
  
  void ValueNode::validate_options() const {
    if (options.size()>0) {
      bool match = false;
      for (int i=0; i<options.size(); i++) {
	if (options[i].value and *value==options[i].value.get())
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

  void ValueNode::validate_format() const {
    if (format.size()>0)
      throw std::runtime_error("Format property can be used only with string nodes: "+line.code);
  }

  void ValueNode::validate_dimensions() const {
    Array::ShapeType vdim = value->get_shape();
    if (dimension.size()!=vdim.size()) 
      throw std::runtime_error("Number of value dimensions does not match that of node: "+std::to_string(vdim.size())+"!="+std::to_string(dimension.size()));
    for (size_t i=0; i<dimension.size(); i++) {
      int dmin = dimension[i].dmin; // must be int and not size_t because
      int dmax = dimension[i].dmax; // dimension ranges can be -1
      if (dmax==Array::max_range)
	dmax = vdim[i];
      if (vdim[i]<dmin or dmax<vdim[i]) {
	std::ostringstream nss, vss;
	for (size_t j=0; j<dimension.size(); j++) {
	  if (j>0) {
	    nss << ",";
	    vss << ",";
	  }
	  dmin = dimension[j].dmin;
	  dmax = dimension[j].dmax;
	  if (dmin==0 and dmax<0)
	    nss << SEPARATOR_SLICE;
	  else if (dmin==dmax)
	    nss << dmin;
	  else if (dmax<0)
	    nss << dmin << SEPARATOR_SLICE;
	  else if (dmin==0)
	    nss << SEPARATOR_SLICE << dmax;
	  else
	    nss << dmin << SEPARATOR_SLICE << dmax;
	  vss << vdim[j];
	}
	throw std::runtime_error("Value dimensions do not correspond to the node dimension ranges: ["+vss.str()+"] != ["+nss.str()+"]");
      }
    }
    //std::cout << "checking dimensions " << std::endl;
    //std::cout << dimension.size() << std::endl;
    //std::cout << value->dimension().size() << std::endl;
  }

}
