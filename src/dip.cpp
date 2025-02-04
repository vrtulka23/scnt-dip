#include <sstream>
#include <string>
#include <iostream>

#include "dip.h"
#include "settings.h"

namespace dip {

  int DIP::num_instances = 0;
  
  DIP::DIP(const Environment* e) {
    // Initial settings
    name = DIP::num_instances++;
    source.name = "DIP"+std::to_string(name)+"_"+std::string(ROOT_SOURCE);
    source.line_number = 0;
    env.sources.append(source.name,"","",{"",0});
    
    // Populate node lists
    nodes_nohierarchy.insert(nodes_nohierarchy.end(), nodes_special.begin(), nodes_special.end());
    nodes_nohierarchy.insert(nodes_nohierarchy.end(), nodes_properties.begin(), nodes_properties.end());
    nodes_notypes.insert(nodes_notypes.end(), nodes_special.begin(), nodes_special.end());
    nodes_notypes.insert(nodes_notypes.end(), nodes_properties.begin(), nodes_properties.end());
    nodes_notypes.insert(nodes_notypes.end(), nodes_hierarchy.begin(), nodes_hierarchy.end());
  }
  
  void DIP::add_string(const std::string code) {
    std::istringstream ss(code);
    std::string token;
    num_strings += 1;
    std::string source_name = "DIP"+std::to_string(name)+"_"+std::string(STRING_SOURCE)+std::to_string(num_strings);
    int line_number = 0;
    while (getline(ss, token, SEPARATOR_NEWLINE)) {
      lines.push({token,{source_name,line_number}});
      line_number += 1;
    }
    int lineno = source.line_number;
    std::string path = env.sources[source.name].path;
    env.sources.append(source_name,path,code,{source.name, lineno});
  }
  
  std::string DIP::to_string() {
    return "DIP";
  }

  NodeList DIP::_get_queue() {
    NodeList queue;
    while (lines.size()>0) {
      Line line = lines.front();
      lines.pop();
      // TODO: Group block structures """text"""
      std::shared_ptr<BaseNode> node = _determine_node(line);
      queue.push_back(node);
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
    if (node==nullptr) node = IntegerNode::is_node(parser);
    if (node==nullptr) node = FloatNode::is_node(parser);
    if (node==nullptr) node = StringNode::is_node(parser);
    // TODO: decode symbols
    return node;
  }
  
  Environment DIP::parse() {
    NodeList queue = _get_queue();
    Environment target = env;
    while (queue.size()>0) {
      std::shared_ptr<BaseNode> node = queue.pop_front();
      // TODO: value injecting
      // Perform specific node parsing only outside of case or inside of valid case
      BaseNode::NodeListType parsed = node->parse(target);
      if (parsed.size()>0) {
	while (parsed.size()>0) {
	  queue.push_front(parsed.back());
	  parsed.pop_back();
	}
	continue;
      }
      //Create hierarchical name
      target.hierarchy.record(node, nodes_nohierarchy);
      
      // TODO: rest of the parsing
      target.nodes.push_back(node);
    }
    // TODO: node validation
    return target;
  }

}
