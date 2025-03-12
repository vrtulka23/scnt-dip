#include "nodes.h"

#include <sstream>

namespace dip {

  std::unique_ptr<BaseValue> ValueNode::cast_value() {
    return cast_value(value_raw);
  }

  std::unique_ptr<BaseValue> ValueNode::cast_value(std::vector<std::string> value_input) {
    if (dimension.size()>0) {
      return cast_array_value(value_input, value_shape);
    } else {
      return cast_scalar_value(value_input[0]);
    }

  }

  void ValueNode::set_value(std::unique_ptr<BaseValue> value_input) {
    value = nullptr;
    if (value_input==nullptr and !value_raw.empty() and !value_raw[0].empty()) {
      value = cast_value();
    } else if (value_input!=nullptr) {
      value = std::move(value_input);
      if (value->dtype!=value_dtype)
	throw std::runtime_error("Value data type has ID="+std::to_string(value_dtype)+", but node data type has ID="+std::to_string(value->dtype));
    }
    if (value!=nullptr and !dimension.empty())
      validate_dimensions(); // check if value shape corresponds with dimension ranges
  }

  void ValueNode::modify_value(std::shared_ptr<BaseNode> node, Environment& env) {
    if (node->dtype!=Node::NODE_MODIFICATION and node->dtype!=dtype)
	throw std::runtime_error("Node '"+name+"' with type '"+dtype_raw+"' cannot modify node '"+node->name+"' with type '"+node->dtype_raw+"'");
    std::unique_ptr<BaseValue> value = cast_value(node->value_raw);
    // TODO: add conversion to original units
    value_raw = node->value_raw;
    set_value(std::move(value));
  }

  /*
   * Validation of node properties and values
   */
  
  void ValueNode::validate_constant() {
    if (constant)
      throw std::runtime_error("Node '"+name+"' is constant and cannot be modified: "+line.code);
  }
  
  void ValueNode::validate_definition() {
    if (declared and value==nullptr) 
      throw std::runtime_error("Declared node has undefined value: "+line.code);
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
      throw std::runtime_error("Format property can be used only with string nodes: "+line.code);
  }

  void ValueNode::validate_dimensions() {
    std::vector<int> vdim = value->dimension();
    if (dimension.size()!=vdim.size())
      throw std::runtime_error("Number of value dimensions does not match that of node: "+std::to_string(vdim.size())+"!="+std::to_string(dimension.size()));
    for (int i=0; i<dimension.size(); i++) {
      int dmin = std::get<0>(dimension[i]);
      int dmax = std::get<1>(dimension[i]);
      if (dmax<0)
	dmax = vdim[i];
      if (vdim[i]<dmin or dmax<vdim[i]) {
	std::ostringstream nss, vss;
	for (int j=0; j<dimension.size(); j++) {
	  if (j>0) {
	    nss << ",";
	    vss << ",";
	  }
	  dmin = std::get<0>(dimension[j]);
	  dmax = std::get<1>(dimension[j]);
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
