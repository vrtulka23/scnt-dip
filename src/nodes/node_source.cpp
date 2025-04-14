#include <array>
#include <fstream>

#include "nodes.h"
#include "../environment.h"
#include "../dip.h"
#include "../parsers.h"

namespace dip {

  BaseNode::PointerType SourceNode::is_node(Parser& parser) {
    if (parser.kwd_source()) {
      parser.part_keyword();
      parser.part_equal();
      parser.part_string();
      parser.part_comment();
      return std::make_shared<SourceNode>(parser);
    }
    return nullptr;
  }
  
  BaseNode::NodeListType SourceNode::parse(Environment& env) {
      // TODO: implement import of a source
      // TODO: implement injection of a source file
      // TODO: implement injection a text file
    EnvSource senv = parse_source(value_raw.at(0), value_raw.at(1), line.source);
    env.sources.append(value_raw.at(0), senv);
    return {};
  }  
 
}
