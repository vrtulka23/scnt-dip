#include <sstream>
#include <string>
#include <iostream>

#include "dip.h"
#include "settings.h"

namespace dip {

  int DIP::num_instances = 0;
  
  DIP::DIP(const Environment* e) {
    name = DIP::num_instances++;
    source.name = "DIP"+std::to_string(name)+"_"+std::string(ROOT_SOURCE);
    source.line_number = 0;
    env.sources.append(source.name,"","",{"",0});
  }
  
  void DIP::add_string(const std::string code) {
    std::istringstream ss(code);
    std::string token;
    num_strings += 1;
    std::string source_name = "DIP"+std::to_string(name)+"_"+std::string(STRING_SOURCE)+std::to_string(num_strings);
    int line_number = 0;
    while (getline(ss, token, SEPARATOR_NEWLINE)) {
      lines.push_back({token,{source_name,line_number}});
      line_number += 1;
    }
    int lineno = source.line_number;
    std::string path = env.sources[source.name].path;
    env.sources.append(source_name,path,code,{source.name, lineno});
  }
  
  std::string DIP::to_string() {
    return "DIP";
  }

  Environment DIP::_get_queue() {
    Environment queue;
    while (lines.size()>0) {
      Line line = lines.back();
      lines.pop_back();
      std::cout << line.source.line_number << " " << line.code << std::endl;
      // TODO: Group block structures """text"""
      std::shared_ptr<BaseNode> node = _determine_node(line);
      queue.nodes.append(node);
    }
    return queue;
  }

  std::shared_ptr<BaseNode> DIP::_determine_node(const Line& line) {
    // TODO: encode symbols \' \" and \n
    Parser parser(line);
    //parser.do_continue()
    std::shared_ptr<BaseNode> node(nullptr);
    node = EmptyNode::is_node(parser);
    if (node==nullptr) parser.part_indent();
    if (node==nullptr) parser.part_name();
    if (node==nullptr) node = GroupNode::is_node(parser);
    if (node==nullptr) parser.part_type();
    if (node==nullptr) node = BooleanNode::is_node(parser);
    if (node==nullptr) node = StringNode::is_node(parser);
    if (node==nullptr)
      std::cout << "parser: " << parser.to_string() << std::endl;
    else
      std::cout << "node:   " << node->to_string() << std::endl;
    // TODO: decode symbols
    return node;
  }
  
  Environment DIP::parse() {
    Environment queue = _get_queue();
    Environment target = env;
    return target;
  }

}
