#include <array>
#include <fstream>

#include "nodes.h"
#include "../environment.h"
#include "../dip.h"
#include "../parsers.h"

namespace dip {

  BaseNode::PointerType SourceNode::is_node(Parser& parser) {
    if (parser.kwd_source()) {
      // TODO: implement import of a source
      // TODO: implement injection of a source file
      // TODO: implement injection a text file
      parser.part_key();
      parser.part_equal();
      parser.part_string();
      parser.part_comment();
      return std::make_shared<SourceNode>(parser);
    }
    return nullptr;
  }
  
  BaseNode::NodeListType SourceNode::parse(Environment& env) {
    EnvSource senv = parse_source(value_raw[0], value_raw[1], line.source);
    env.sources.append(value_raw[0], senv);
    return {};
  }  
 
}
