#ifndef DIP_NODE_H
#define DIP_NODE_H

#include <vector>

#include "../settings.h"

namespace dip {

  class Node {
  public:
    enum NodeKeyword {
      UNKNOWN_NODE,
      NODE_EMPTY, NODE_GROUP,
      NODE_BOOLEAN, NODE_STRING, NODE_INTEGER, NODE_FLOAT, NODE_TABLE
    };
    Line line;   // in Python code/source
    int indent;
    std::string name;
    std::string value_raw;
    NodeKeyword keyword;
    std::string dtype;
    std::vector<std::string> dtype_prop;
    bool defined;
    std::vector<std::tuple<int,int>> dimension;
    Node(): indent(0), defined(false), keyword(UNKNOWN_NODE) {};
    bool has_keyword(NodeKeyword kwd);
    std::string to_string();
  };
  
  class Parser: public Node {
  public:
    enum ParsingFlag {
      PART_INDENT, PART_NAME, PART_TYPE, PART_DIMENSION, PART_EQUAL, PART_VALUE, PART_COMMENT
    };
    Line line;  // in Python code/source
    std::string code;
    std::string comment;
    std::vector<ParsingFlag> parsed;
  private:
    void _strip(const std::string text, ParsingFlag flag);
  public:
    Parser(const Line& l): line(l), code(l.code) {};
    bool do_continue();
    bool is_parsed(ParsingFlag flag);
    void part_indent();
    void part_name(const bool path=true);
    void part_type();
    void part_dimension();
    void part_equal();
    void part_value();
    void part_reference(const bool inject=false);
    void part_slice();
    void part_format();
    void part_function();
    void part_expression();
    void part_units();
    void part_comment();
  };

  class BaseNode: public Node {
  public:
    BaseNode();
    BaseNode(Parser& parser);
    BaseNode(Parser& parser, const NodeKeyword kwd): BaseNode(parser) {keyword = kwd;};
  };
  
  class EmptyNode: public BaseNode {
  public:
    static std::shared_ptr<BaseNode> is_node(Parser& parser);
    EmptyNode(Parser& parser): BaseNode(parser,Node::NODE_EMPTY) {};
  };

  class GroupNode: public BaseNode {
  public:
    static std::shared_ptr<BaseNode> is_node(Parser& parser);
    GroupNode(Parser& parser): BaseNode(parser,Node::NODE_GROUP) {};
  };
  
  class BooleanNode: public BaseNode {
  public:
    static std::shared_ptr<BaseNode> is_node(Parser& parser);
    BooleanNode(Parser& parser): BaseNode(parser,Node::NODE_BOOLEAN) {};
  };  
  
  class StringNode: public BaseNode {
  public:
    static std::shared_ptr<BaseNode> is_node(Parser& parser);
    StringNode(Parser& parser): BaseNode(parser,Node::NODE_STRING) {};
  };  
  
}
  
#endif // DIP_NODE_H
