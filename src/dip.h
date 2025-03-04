#ifndef DIP_DIP_H
#define DIP_DIP_H

#include <iostream>
#include <queue>
#include <array>

#include "environment.h"

namespace dip {

  class DIP {
  private:
    static int num_instances;
    int name;
    Environment env;
    std::queue<Line> lines;

    Source source;

    static constexpr std::array<Node::NodeDtype,3> nodes_special = {
      BaseNode::NODE_EMPTY,BaseNode::NODE_UNIT,BaseNode::NODE_SOURCE
    };
    static constexpr std::array<Node::NodeDtype,6> nodes_properties = {
      BaseNode::NODE_OPTIONS,BaseNode::NODE_CONSTANT,BaseNode::NODE_FORMAT,
      BaseNode::NODE_CONDITION,BaseNode::NODE_TAGS,BaseNode::NODE_DESCRIPTION
    };
    static constexpr std::array<Node::NodeDtype,1> nodes_hierarchy  = {
      BaseNode::NODE_GROUP
    };
    std::vector<Node::NodeDtype> nodes_nohierarchy;
    std::vector<Node::NodeDtype> nodes_notypes;
    
    size_t num_files = 0;
    size_t num_strings = 0;
    
    NodeList _get_queue();
    std::shared_ptr<BaseNode> _determine_node(const Line& line);
  public:
    DIP(const Environment* e = NULL);
    void add_file(const std::string file_path, const std::string source_name, const bool absolute=true);
    void add_string(const std::string code);
    void add_source(const std::string name, const std::string path);
    void add_unit(const std::string name, const double value, const std::string unit="");    
    void add_function(const std::string name, int (*fn)(int));
    Environment parse();
    Environment parse_docs();
    std::string to_string();
  };

}

#endif // DIP_DIP_H
