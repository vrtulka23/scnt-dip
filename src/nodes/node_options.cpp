#include <array>

#include "nodes.h"
#include "../environment.h"

namespace dip {

  BaseNode::PointerType OptionsNode::is_node(Parser& parser) {
    if (parser.part_equal(false)) {
      parser.part_value();
      parser.part_units();
      parser.part_comment();
      return std::make_shared<OptionsNode>(parser);
    }
    if (parser.kwd_options()) {
      parser.part_value();
      parser.part_units();
      parser.part_comment();
      return std::make_shared<OptionsNode>(parser);
    }
    return nullptr;
  }

  void OptionsNode::set_property(Environment& env, ValueNode::PointerType vnode) {
    if (dimension.empty()) {
      vnode->set_option(value_raw.at(0), units_raw, env);
    } else {
      for (auto value_option: value_raw)
	vnode->set_option(value_option, units_raw, env); 
    }
  }

}
