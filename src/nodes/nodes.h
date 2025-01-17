#ifndef DIP_NODE_H
#define DIP_NODE_H

#include <vector>
#include <deque>

#include "../settings.h"

namespace dip {
  
  class Node {
  public:
    typedef std::vector<std::tuple<int,int>> DimensionType;
    enum NodeKeyword {
      NODE_NONE, NODE_EMPTY, NODE_GROUP, NODE_UNIT, NODE_SOURCE,
      NODE_BOOLEAN, NODE_STRING, NODE_INTEGER, NODE_FLOAT, NODE_TABLE,
      NODE_OPTION, NODE_CONSTANT, NODE_FORMAT, NODE_CONDITION, NODE_TAGS, NODE_DESCRIPTION
    };
    Line line;   // in Python code/source
    int indent;
    std::string name;
    std::string value_raw;
    NodeKeyword keyword;
    std::string dtype;
    std::vector<std::string> dtype_prop;
    bool defined;
    DimensionType dimension;
    Node(): indent(0), defined(false), keyword(NODE_NONE) {};
    bool has_keyword(NodeKeyword kwd);
    std::string to_string();
  };
  
  class Parser: public Node {
  public:
    enum ParsingFlag {
      PART_INDENT, PART_NAME, PART_TYPE, PART_DIMENSION, PART_EQUAL, PART_VALUE, PART_COMMENT,
      PART_REFERENCE, PART_FUNCTION, PART_EXPRESSION
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
    typedef std::deque<std::shared_ptr<BaseNode>> NodeListType;
    BaseNode();
    BaseNode(Parser& parser);
    BaseNode(Parser& parser, const NodeKeyword kwd): BaseNode(parser) {keyword = kwd;};
    NodeListType parse(const Environment& env);
  };
  
  class EmptyNode: public BaseNode {
  public:
    static std::shared_ptr<BaseNode> is_node(Parser& parser);
    EmptyNode(Parser& parser): BaseNode(parser, Node::NODE_EMPTY) {};
  };

  class GroupNode: public BaseNode {
  public:
    static std::shared_ptr<BaseNode> is_node(Parser& parser);
    GroupNode(Parser& parser): BaseNode(parser, Node::NODE_GROUP) {};
  };
  
  class BooleanNode: public BaseNode {
  public:
    static std::shared_ptr<BaseNode> is_node(Parser& parser);
    BooleanNode(Parser& parser): BaseNode(parser, Node::NODE_BOOLEAN) {};
    NodeListType parse(Environment& env);
  };  
  
  class IntegerNode: public BaseNode {
  public:
    static std::shared_ptr<BaseNode> is_node(Parser& parser);
    IntegerNode(Parser& parser): BaseNode(parser, Node::NODE_INTEGER) {};
    NodeListType parse(Environment& env);
  };  
  
  class FloatNode: public BaseNode {
  public:
    static std::shared_ptr<BaseNode> is_node(Parser& parser);
    FloatNode(Parser& parser): BaseNode(parser, Node::NODE_FLOAT) {};
    NodeListType parse(Environment& env);
  };  
  
  class StringNode: public BaseNode {
  public:
    static std::shared_ptr<BaseNode> is_node(Parser& parser);
    StringNode(Parser& parser): BaseNode(parser, Node::NODE_STRING) {};
    NodeListType parse(Environment& env);
  };  
  
}
  
#endif // DIP_NODE_H
