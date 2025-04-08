#ifndef DIP_PARSERS_H
#define DIP_PARSERS_H

#include "lists/lists.h"

namespace dip {

  EnvSource parse_source(const std::string& source_name, const std::string& source_file, const Source& parent);
  std::queue<Line> parse_lines(std::queue<Line>& lines, const std::string& source_code, const std::string& source_name);
  BaseNode::NodeListType parse_code_nodes(std::queue<Line>& lines);
  BaseNode::NodeListType parse_table_nodes(std::queue<Line>& lines, const char delimiter);
  std::string parse_array(const std::string& value_string, Array::StringType& value_raw, Array::ShapeType& value_shape);
  void parse_value(std::string value_string, Array::StringType& value_raw, Array::ShapeType& value_shape);
  void parse_slices(std::string& value_string, Array::RangeType& dimension);
  
}

#endif // DIP_PARSERS_H
