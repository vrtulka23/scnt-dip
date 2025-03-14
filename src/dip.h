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

    static constexpr std::array<BaseNode::NodeDtype,3> nodes_special = {
      BaseNode::EMPTY,BaseNode::UNIT,BaseNode::SOURCE
    };
    static constexpr std::array<BaseNode::NodeDtype,6> nodes_properties = {
      BaseNode::OPTIONS,BaseNode::CONSTANT,BaseNode::FORMAT,
      BaseNode::CONDITION,BaseNode::TAGS,BaseNode::DESCRIPTION
    };
    static constexpr std::array<BaseNode::NodeDtype,1> nodes_hierarchy  = {
      BaseNode::GROUP
    };
    std::vector<BaseNode::NodeDtype> nodes_nohierarchy;
    std::vector<BaseNode::NodeDtype> nodes_notypes;
    
    size_t num_files = 0;
    size_t num_strings = 0;
    
    NodeList _get_queue();
    BaseNode::PointerType _determine_node(Line& line);
  public:
    DIP(const Environment* e = NULL);
    void add_string(const std::string code);
    void add_file(const std::string file_path, const std::string source_name, const bool absolute=true);
    void add_source(const std::string name, const std::string path);
    void add_unit(const std::string name, const double value, const std::string unit="");    
    void add_value_function(const std::string name, FunctionList::ValueFunctionType func);
    void add_table_function(const std::string name, FunctionList::TableFunctionType func);
    Environment parse();
    Environment parse_docs();
    std::string to_string();
  };

}

#endif // DIP_DIP_H
