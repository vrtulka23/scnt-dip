#ifndef DIP_DIP_H
#define DIP_DIP_H

#include <iostream>

#include "environment.h"

namespace dip {

  class DIP {
  private:
    static int num_instances;
    int name;
    Environment env;
    std::vector<Line> lines;

    Source source;
    
    size_t num_files = 0;
    size_t num_strings = 0;
    
    Environment _get_queue();
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
