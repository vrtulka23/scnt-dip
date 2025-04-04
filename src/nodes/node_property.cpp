#include "nodes.h"
#include "../environment.h"

namespace dip {

  BaseNode::PointerType PropertyNode::is_node(Parser& parser) {
    PropertyType ptype = PropertyType::None;
    // determine property type
    if (parser.part_equal(false))
      ptype = PropertyType::Options;
    else
      parser.kwd_property(ptype);
    // parse rest of the parts
    if (ptype!=PropertyType::None) {
      parser.part_value();
      parser.part_units();
      parser.part_comment();
      return std::make_shared<PropertyNode>(parser, ptype);      
    }
    return nullptr;
  }
  
  // Set nodes that can preceeding an option
  static constexpr std::array<NodeDtype,13> preceeding_nodes = {
    NodeDtype::Boolean,NodeDtype::Integer,NodeDtype::Float,NodeDtype::String,NodeDtype::Table, NodeDtype::Property
  };
  
  BaseNode::NodeListType PropertyNode::parse(Environment& env) {
    if (std::find(preceeding_nodes.begin(), preceeding_nodes.end(), env.previous_node) == preceeding_nodes.end())
      throw std::runtime_error("Only value nodes (bool, int, float and str) can have properties: "+line.code);
    BaseNode::PointerType node = env.nodes.at(env.nodes.size()-1);
    if (node->indent>=indent)
      throw std::runtime_error("The indent '"+std::to_string(indent)+"' of a property is not higher than the indent '"+std::to_string(node->indent)+"' of a preceding node: "+line.code);
    if (!node->set_property(ptype, value_raw, units_raw, env))
      throw std::runtime_error("Property could not be set: "+line.code);
    return {};
  }  
    
}
