#include "environment.h"

namespace dip {

  inline std::tuple<std::string, std::string> parse_request(const std::string& request) {
    size_t pos = request.find(SIGN_QUERY);
    if (pos == std::string::npos)
      throw std::runtime_error("Environment request must contain a question mark symbol: "+request);
    else
      return {request.substr(0, pos), request.substr(pos + 1)};
  }
  
  Environment::Environment() {
    
  }

  BaseValue::PointerType Environment::request_value(const std::string& request, const RequestType rtype, const std::string& in_units) const {
    BaseValue::PointerType new_value = nullptr;
    switch (rtype) {
    case Environment::FUNCTION: {
      FunctionList::ValueFunctionType func = functions.get_value(request);
      new_value = func(*this);
      break;
    }
    case Environment::REFERENCE: {
      auto [source, path] = parse_request(request);
      const NodeList& node_pool = (source.empty()) ? nodes : sources[source].nodes;
      for (size_t i=0; i<node_pool.size(); i++) {
	ValueNode::PointerType vnode = std::dynamic_pointer_cast<ValueNode>(node_pool[i]);
	if (vnode and vnode->name==path)
	  new_value = vnode->value->clone();
      }
      break;
    }
    default:
      throw std::runtime_error("Unrecognized environment request type: "+std::to_string(rtype));
    }
    if (new_value == nullptr)
      throw std::runtime_error("Value environment request returns an empty pointer: "+request);
    return std::move(new_value);
  }
  
  BaseNode::NodeListType Environment::request_nodes(const std::string& request, const RequestType rtype, const std::string& in_units) const {
    BaseNode::NodeListType new_nodes;
    switch (rtype) {
    case Environment::FUNCTION: {
      FunctionList::TableFunctionType func = functions.get_table(request);
      new_nodes = func(*this);
      break;
    }
    case Environment::REFERENCE: {
      auto [source, path] = parse_request(request);
      if (!path.empty())
	path += std::string(1,SIGN_SEPARATOR);
      const NodeList& node_pool = (source.empty()) ? nodes : sources[source].nodes;
      for (size_t i=0; i<node_pool.size(); i++) {
	ValueNode::PointerType vnode = std::dynamic_pointer_cast<ValueNode>(node_pool[i]);
	if (vnode and vnode->name.rfind(path, 0) == 0 and vnode->name.size()>path.size()) {
	  std::string new_name = vnode->name.substr(path.size(), vnode->name.size());
	  new_nodes.push_back(vnode->clone(new_name));
	}
      }
      break;
    }
    default:
      throw std::runtime_error("Unrecognized environment request type: "+std::to_string(rtype));
    }      
    if (new_nodes.empty())
      throw std::runtime_error("Node environment request returns an empty node list: "+request);
    return new_nodes;
  }
  
}
