#ifndef DIP_NODES_H
#define DIP_NODES_H

#include <vector>
#include <deque>
#include <iostream>
#include <sstream>
#include <iomanip>

#include "../settings.h"
#include "values.h"

namespace dip {
  
  class Node {
  public:
    typedef std::vector<std::tuple<int,int>> DimensionType;
    enum NodeDtype {
      NODE_NONE, NODE_EMPTY, NODE_GROUP, NODE_UNIT, NODE_SOURCE, NODE_CASE, NODE_MODIFICATION,
      NODE_BOOLEAN, NODE_STRING, NODE_INTEGER, NODE_FLOAT, NODE_TABLE,
      NODE_OPTIONS, NODE_CONSTANT, NODE_FORMAT, NODE_CONDITION, NODE_TAGS, NODE_DESCRIPTION
    };
    Line line;                             // source code line information; in Python this were 'code' & 'source' variables
    NodeDtype dtype;                       // data type of a node; in Python this was 'keyword' variable
    std::string dtype_raw;                 // data type string; in Python this was 'dtype' variable
    std::vector<std::string> dtype_prop;   // data type properties (e.g. unsigned/precision)
    int indent;                            // indent of a node
    std::string name;                      // node name
    std::vector<std::string> value_raw;    // raw value string(s)
    std::vector<int> value_shape;          // shape of an array value
    std::string value_ref;                 // reference string
    std::string value_func;                // function name; in Python this was 'value_fn' variable
    std::string value_expr;                // expression string
    //std::tuple<std::string,> value_slice;
    std::string units_raw;                 // raw units string
    bool declared;                         // is node a declaration?; in Python this was 'defined' variable
    DimensionType dimension;               // list of array dimensions
    Node(): indent(0), declared(false), dtype(NODE_NONE) {};
    Node(const Line& l): line(l), indent(0), declared(false), dtype(NODE_NONE) {};
    Node(const NodeDtype kwd): indent(0), declared(false), dtype(kwd) {};
    virtual ~Node() = default;
    bool has_dtype(NodeDtype kwd);
    std::string to_string();
  };
  
  class Parser: public Node {
  public:
    enum ParsingFlag {
      KWD_CASE, KWD_UNIT, KWD_SOURCE,
      KWD_OPTIONS, KWD_CONSTANT, KWD_FORMAT, KWD_TAGS, KWD_DESCRIPTION, KWD_CONDITION,
      PART_INDENT, PART_NAME, PART_TYPE, PART_DIMENSION, PART_EQUAL,
      PART_REFERENCE, PART_FUNCTION, PART_EXPRESSION, PART_ARRAY, PART_STRING, PART_VALUE,
      PART_UNITS, PART_COMMENT, PART_DELIMITER
    };
    std::string code;                      // in Python this was 'ccode', the original 'code' is now in the 'line' struct
    std::string comment;
    std::vector<ParsingFlag> parsed;
  private:
    void _strip(const std::string text, ParsingFlag flag);
  public:
    Parser(const Line& l): Node(l), code(l.code) {};
    static void encode_escape_symbols(std::string& str);
    static void decode_escape_symbols(std::string& str);
    bool do_continue();
    bool is_parsed(ParsingFlag flag);
    void kwd_case();
    void kwd_unit();
    void kwd_source();
    void kwd_options();
    void kwd_constant();
    void kwd_format();
    void kwd_tags();
    void kwd_description();
    void kwd_condition();
    void part_indent();
    void part_name(const bool path=true);
    void part_type();
    void part_dimension();
    void part_equal();
    void part_array();
    bool part_string();
    void part_value();
    void part_reference(const bool inject=false);
    void part_slice();
    void part_function();
    void part_expression();
    void part_units();
    void part_comment();
    bool part_delimiter(char symbol);
  };
  
  class BaseNode: virtual public Node {
  public:
    std::string branch_id;
    std::string case_id;
    typedef std::deque<std::shared_ptr<BaseNode>> NodeListType;
    BaseNode() {};
    BaseNode(Parser& parser);
    BaseNode(Parser& parser, const NodeDtype kwd);
    virtual ~BaseNode() = default;
    virtual NodeListType parse(Environment& env);
  };

  class EmptyNode: public BaseNode {
  public:
    static std::shared_ptr<BaseNode> is_node(Parser& parser);
    EmptyNode(Parser& parser): BaseNode(parser, Node::NODE_EMPTY) {};
  };

  /*
  class ImportNode: public BaseNode {
  public:
  };
  */
  
  /*
  class UnitNode: public BaseNode {
  public:
  };
  */
  
  /*
  class SourceNode: public BaseNode {
  public:
  };
  */
  
  class CaseNode: public BaseNode {
  public:
    bool value;             // case value
    int case_id;            // ID of a case
    std::string case_type;  // type of a case
    static std::shared_ptr<BaseNode> is_node(Parser& parser);
    CaseNode(Parser& parser): BaseNode(parser, Node::NODE_CASE), case_id(0), value(false) {};
    BaseNode::NodeListType parse(Environment& env) override;
  };
  
  class OptionsNode: public BaseNode {
  public:
    static std::shared_ptr<BaseNode> is_node(Parser& parser);
    OptionsNode(Parser& parser): BaseNode(parser, Node::NODE_OPTIONS) {};
    BaseNode::NodeListType parse(Environment& env) override;
  };
  
  class ConstantNode: public BaseNode {
  public:
    static std::shared_ptr<BaseNode> is_node(Parser& parser);
    ConstantNode(Parser& parser): BaseNode(parser, Node::NODE_CONSTANT) {};
    BaseNode::NodeListType parse(Environment& env) override;
  };
  
  class FormatNode: public BaseNode {
  public:
    static std::shared_ptr<BaseNode> is_node(Parser& parser);
    FormatNode(Parser& parser): BaseNode(parser, Node::NODE_FORMAT) {};
    BaseNode::NodeListType parse(Environment& env) override;
  };

  class TagsNode: public BaseNode {
  public:
    static std::shared_ptr<BaseNode> is_node(Parser& parser);
    TagsNode(Parser& parser): BaseNode(parser, Node::NODE_TAGS) {};
    BaseNode::NodeListType parse(Environment& env) override;
  };
  
  class DescriptionNode: public BaseNode {
  public:
    static std::shared_ptr<BaseNode> is_node(Parser& parser);
    DescriptionNode(Parser& parser): BaseNode(parser, Node::NODE_CONSTANT) {};
    BaseNode::NodeListType parse(Environment& env) override;
  };
  
  /*
  class ConditionNode: public BaseNode {
  public:
  };
  */
  
  class GroupNode: public BaseNode {
  public:
    static std::shared_ptr<BaseNode> is_node(Parser& parser);
    GroupNode(Parser& parser): BaseNode(parser, Node::NODE_GROUP) {};
  };
    
  class ModificationNode: public BaseNode {
  public:
    static std::shared_ptr<BaseNode> is_node(Parser& parser);
    ModificationNode(Parser& parser): BaseNode(parser, Node::NODE_MODIFICATION) {};
  };
  
  class TableNode: public BaseNode {
  public:
    static std::shared_ptr<BaseNode> is_node(Parser& parser);
    TableNode(Parser& parser): BaseNode(parser, Node::NODE_TABLE) {};
    BaseNode::NodeListType parse(Environment& env) override;
  };
  
  class ValueNode: virtual public BaseNode {
    virtual std::unique_ptr<BaseValue> cast_scalar_value(const std::string value_input) = 0;
    virtual std::unique_ptr<BaseValue> cast_array_value(const std::vector<std::string>& value_inputs, const std::vector<int>& shape) = 0;
  protected:
    struct OptionStruct {
      std::unique_ptr<BaseValue> value;
      std::string value_raw;
      std::string units_raw;
    };
    BaseValue::ValueDtype value_dtype;
  public:
    std::unique_ptr<BaseValue> value;
    std::vector<std::string> tags;
    bool constant;
    std::string description;
    std::vector<OptionStruct> options;
    std::string format;
    ValueNode(): constant(false) {};
    ValueNode(const BaseValue::ValueDtype vdt): constant(false), value_dtype(vdt) {};
    virtual ~ValueNode() = default;
    std::unique_ptr<BaseValue> cast_value();
    std::unique_ptr<BaseValue> cast_value(std::vector<std::string> value_input);
    void set_value(std::unique_ptr<BaseValue> value_input=nullptr);
    void modify_value(std::shared_ptr<BaseNode> node, Environment& env);
    virtual void set_option(const std::string option_value, const std::string option_units, Environment& env) = 0;
    void validate_constant();
    void validate_definition();
    virtual void validate_options();
    virtual void validate_format();
    virtual void validate_datatype() = 0;
  private:
    void validate_dimensions();
  };
  
  class BooleanNode: public ValueNode {
    std::unique_ptr<BaseValue> cast_scalar_value(const std::string value_input) override;
    std::unique_ptr<BaseValue> cast_array_value(const std::vector<std::string>& value_inputs, const std::vector<int>& shape) override;
  public:
    static std::shared_ptr<BaseNode> is_node(Parser& parser);
    BooleanNode(Parser& parser): BaseNode(parser, Node::NODE_BOOLEAN), ValueNode(BaseValue::VALUE_BOOL) {};
    BaseNode::NodeListType parse(Environment& env) override;
    void set_option(const std::string option_value, const std::string option_units, Environment& env) override;
    void validate_options() override;
    void validate_datatype() override;
  };  
  
  class IntegerNode: public ValueNode {
    std::unique_ptr<BaseValue> cast_scalar_value(const std::string value_input) override;
    std::unique_ptr<BaseValue> cast_array_value(const std::vector<std::string>& value_inputs, const std::vector<int>& shape) override;
  public:
    static constexpr size_t max_int_size = sizeof(long long) * CHAR_BIT;
    static std::shared_ptr<BaseNode> is_node(Parser& parser);
    IntegerNode(Parser& parser);
    BaseNode::NodeListType parse(Environment& env) override;
    void set_option(const std::string option_value, const std::string option_units, Environment& env) override;
    void validate_datatype() override;
  };  
  
  class FloatNode: public ValueNode {
    std::unique_ptr<BaseValue> cast_scalar_value(const std::string value_input) override;
    std::unique_ptr<BaseValue> cast_array_value(const std::vector<std::string>& value_inputs, const std::vector<int>& shape) override;
  public:
    static constexpr size_t max_float_size = sizeof(long double) * CHAR_BIT;
    static std::shared_ptr<BaseNode> is_node(Parser& parser);
    FloatNode(Parser& parser): BaseNode(parser, Node::NODE_FLOAT) {};
    BaseNode::NodeListType parse(Environment& env) override;
    void set_option(const std::string option_value, const std::string option_units, Environment& env) override;
    void validate_datatype() override;
  };  
  
  class StringNode: public ValueNode {
    std::unique_ptr<BaseValue> cast_scalar_value(const std::string value_input) override;
    std::unique_ptr<BaseValue> cast_array_value(const std::vector<std::string>& value_inputs, const std::vector<int>& shape) override;
  public:
    static std::shared_ptr<BaseNode> is_node(Parser& parser);
    StringNode(Parser& parser): BaseNode(parser, Node::NODE_STRING), ValueNode(BaseValue::VALUE_STRING) {};
    BaseNode::NodeListType parse(Environment& env) override;
    void set_option(const std::string option_value, const std::string option_units, Environment& env) override;
    void validate_format() override;
    void validate_datatype() override;
  };  
  
}
  
#endif // DIP_NODES_H
