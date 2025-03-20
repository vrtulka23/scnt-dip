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
    enum ValueOrigin {FROM_STRING, FROM_REFERENCE, FROM_FUNCTION, FROM_EXPRESSION};
    Line line;                             // source code line information; in Python this were 'code' & 'source' variables
    int indent;                            // indent of a node
    std::string name;                      // node name
    std::vector<std::string> dtype_raw;    // data type properties (unsigned/type/precision)
    std::vector<std::string> value_raw;    // raw value string(s)
    BaseValue::ShapeType value_shape;      // shape of an array value
    ValueOrigin value_origin;              // origin of the value
    std::string value_ref;                 // reference string
    std::string value_func;                // function name; in Python this was 'value_fn' variable
    std::string value_expr;                // expression string
    //std::tuple<std::string,> value_slice;
    std::string units_raw;                 // raw units string
    DimensionType dimension;               // list of array dimensions
    Node(): indent(0), value_origin(FROM_STRING) {};
    Node(const Line& l): line(l), indent(0), value_origin(FROM_STRING) {};
    Node(const std::string& nm): name(nm), indent(0), value_origin(FROM_STRING) {};
    virtual ~Node() = default;
    std::string to_string();
  };
  
  class Parser: public Node {
  private:
    void strip(const std::string text); 
    static const std::array<std::string, 3> ESCAPE_SYMBOLS;
  public:
    std::string code;                      // in Python this was 'ccode', the original 'code' is now in the 'line' struct
    std::string comment;
    Parser(const Line& l): Node(l), code(l.code) {};
    static void encode_escape_symbols(std::string& str);
    static void decode_escape_symbols(std::string& str);
    bool do_continue();
    bool kwd_case();
    bool kwd_unit();
    bool kwd_source();
    bool kwd_options();
    bool kwd_constant();
    bool kwd_format();
    bool kwd_tags();
    bool kwd_description();
    bool kwd_condition();
    bool part_indent();
    bool part_name(bool required=true);
    bool part_key(bool required=true);
    bool part_type(bool required=true);
    bool part_dimension();
    bool part_equal(bool required=true);
    bool part_reference(const bool inject=false);
    bool part_expression();
    bool part_function();
    bool part_array();
    bool part_string();
    bool part_value();
    bool part_slice();
    bool part_units();
    bool part_comment();
    bool part_delimiter(char symbol, bool required=true);
  };
  
  class BaseNode: virtual public Node {
  public:
    enum NodeDtype {
      NONE, EMPTY, GROUP, UNIT, SOURCE, CASE, MODIFICATION, IMPORT,
      BOOLEAN, STRING, INTEGER, FLOAT, TABLE,
      OPTIONS, CONSTANT, FORMAT, CONDITION, TAGS, DESCRIPTION
    };
    NodeDtype dtype;                       // data type of a node; in Python this was 'keyword' variable in Node class
    std::string branch_id;
    std::string case_id;
    typedef std::shared_ptr<BaseNode> PointerType;
    typedef std::deque<BaseNode::PointerType> NodeListType;
    BaseNode(): dtype(NONE) {};
    BaseNode(const NodeDtype dt): dtype(dt) {};
    BaseNode(Parser& parser, const NodeDtype dt);
    virtual ~BaseNode() = default;
    virtual NodeListType parse(Environment& env);
  };

  class EmptyNode: public BaseNode {
  public:
    static BaseNode::PointerType is_node(Parser& parser);
    EmptyNode(Parser& parser): BaseNode(parser, BaseNode::EMPTY) {};
  };

  class ImportNode: public BaseNode {
  public:
    static BaseNode::PointerType is_node(Parser& parser);
    ImportNode(Parser& parser): BaseNode(parser, BaseNode::IMPORT) {};
    BaseNode::NodeListType parse(Environment& env) override;
  };

  /*
  class UnitNode: public BaseNode {
  public:
  };
  */
  
  class SourceNode: public BaseNode {
  public:
    static BaseNode::PointerType is_node(Parser& parser);
    SourceNode(Parser& parser): BaseNode(parser, BaseNode::SOURCE) {};
    BaseNode::NodeListType parse(Environment& env) override;
  };
  
  class CaseNode: public BaseNode {
  public:
    bool value;             // case value
    int case_id;            // ID of a case
    std::string case_type;  // type of a case
    static BaseNode::PointerType is_node(Parser& parser);
    CaseNode(Parser& parser): BaseNode(parser, BaseNode::CASE), case_id(0), value(false) {};
    BaseNode::NodeListType parse(Environment& env) override;
  };
  
  class OptionsNode: public BaseNode {
  public:
    static BaseNode::PointerType is_node(Parser& parser);
    OptionsNode(Parser& parser): BaseNode(parser, BaseNode::OPTIONS) {};
    BaseNode::NodeListType parse(Environment& env) override;
  };
  
  class ConstantNode: public BaseNode {
  public:
    static BaseNode::PointerType is_node(Parser& parser);
    ConstantNode(Parser& parser): BaseNode(parser, BaseNode::CONSTANT) {};
    BaseNode::NodeListType parse(Environment& env) override;
  };
  
  class FormatNode: public BaseNode {
  public:
    static BaseNode::PointerType is_node(Parser& parser);
    FormatNode(Parser& parser): BaseNode(parser, BaseNode::FORMAT) {};
    BaseNode::NodeListType parse(Environment& env) override;
  };

  class TagsNode: public BaseNode {
  public:
    static BaseNode::PointerType is_node(Parser& parser);
    TagsNode(Parser& parser): BaseNode(parser, BaseNode::TAGS) {};
    BaseNode::NodeListType parse(Environment& env) override;
  };
  
  class DescriptionNode: public BaseNode {
  public:
    static BaseNode::PointerType is_node(Parser& parser);
    DescriptionNode(Parser& parser): BaseNode(parser, BaseNode::CONSTANT) {};
    BaseNode::NodeListType parse(Environment& env) override;
  };
  
  /*
  class ConditionNode: public BaseNode {
  public:
  };
  */
  
  class GroupNode: public BaseNode {
  public:
    static BaseNode::PointerType is_node(Parser& parser);
    GroupNode(Parser& parser): BaseNode(parser, BaseNode::GROUP) {};
  };
    
  class ModificationNode: public BaseNode {
  public:
    static BaseNode::PointerType is_node(Parser& parser);
    ModificationNode(Parser& parser): BaseNode(parser, BaseNode::MODIFICATION) {};
  };
  
  class TableNode: public BaseNode {
  public:
    static BaseNode::PointerType is_node(Parser& parser);
    TableNode(Parser& parser): BaseNode(parser, BaseNode::TABLE) {};
    BaseNode::NodeListType parse(Environment& env) override;
  };

  class ValueNode: virtual public BaseNode {
    virtual BaseValue::PointerType cast_scalar_value(const std::string value_input) const = 0;
    virtual BaseValue::PointerType cast_array_value(const std::vector<std::string>& value_inputs, const BaseValue::ShapeType& shape) const = 0;
  protected:
    struct OptionStruct {
      BaseValue::PointerType value;
      std::string value_raw;
      std::string units_raw;
    };
    BaseValue::ValueDtype value_dtype;
  public:
    typedef std::shared_ptr<ValueNode> PointerType;
    BaseValue::PointerType value;
    std::vector<std::string> tags;
    bool constant;
    std::string description;
    std::vector<OptionStruct> options;
    std::string format;
    ValueNode(): constant(false) {};
    ValueNode(const BaseValue::ValueDtype vdt): constant(false), value_dtype(vdt) {};
    ValueNode(const std::string& nm, BaseValue::PointerType val, const BaseValue::ValueDtype vdt);
    virtual ~ValueNode() = default;
    BaseValue::PointerType cast_value();
    BaseValue::PointerType cast_value(std::vector<std::string> value_input);
    void set_value(BaseValue::PointerType value_input=nullptr);
    void modify_value(BaseNode::PointerType node, Environment& env);
    virtual void set_option(const std::string option_value, const std::string option_units, Environment& env) = 0;
    virtual BaseNode::PointerType clone(const std::string& nm) const = 0;
    void validate_constant() const;
    void validate_definition() const;
    virtual void validate_options() const;
    virtual void validate_format() const;
  private:
    void validate_dimensions() const;
  };
  
  class BooleanNode: public ValueNode {
    BaseValue::PointerType cast_scalar_value(const std::string value_input) const override;
    BaseValue::PointerType cast_array_value(const std::vector<std::string>& value_inputs, const BaseValue::ShapeType& shape) const override;
  public:
    static BaseNode::PointerType is_node(Parser& parser);
    BooleanNode(const std::string& nm, BaseValue::PointerType val): BaseNode(BaseNode::BOOLEAN), ValueNode(nm, std::move(val), BaseValue::BOOLEAN) {};
    BooleanNode(Parser& parser): BaseNode(parser, BaseNode::BOOLEAN), ValueNode(BaseValue::BOOLEAN) {};
    BaseNode::NodeListType parse(Environment& env) override;
    void set_option(const std::string option_value, const std::string option_units, Environment& env) override;
    BaseNode::PointerType clone(const std::string& nm) const override;
    void validate_options() const override;
  };  
  
  class IntegerNode: public ValueNode {
    BaseValue::PointerType cast_scalar_value(const std::string value_input) const override;
    BaseValue::PointerType cast_array_value(const std::vector<std::string>& value_inputs, const BaseValue::ShapeType& shape) const override;
  public:
    static constexpr size_t max_int_size = sizeof(long long) * CHAR_BIT;
    static BaseNode::PointerType is_node(Parser& parser);
    IntegerNode(const std::string& nm, BaseValue::PointerType val, const BaseValue::ValueDtype vdt): BaseNode(BaseNode::BOOLEAN), ValueNode(nm, std::move(val), vdt) {};
    IntegerNode(Parser& parser);
    BaseNode::NodeListType parse(Environment& env) override;
    void set_option(const std::string option_value, const std::string option_units, Environment& env) override;
    BaseNode::PointerType clone(const std::string& nm) const override;
  };  
  
  class FloatNode: public ValueNode {
    BaseValue::PointerType cast_scalar_value(const std::string value_input) const override;
    BaseValue::PointerType cast_array_value(const std::vector<std::string>& value_inputs, const BaseValue::ShapeType& shape) const override;
  public:
    static constexpr size_t max_float_size = sizeof(long double) * CHAR_BIT;
    static BaseNode::PointerType is_node(Parser& parser);
    FloatNode(const std::string& nm, BaseValue::PointerType val, const BaseValue::ValueDtype vdt): BaseNode(BaseNode::FLOAT), ValueNode(nm, std::move(val), vdt) {};
    FloatNode(Parser& parser);
    BaseNode::NodeListType parse(Environment& env) override;
    void set_option(const std::string option_value, const std::string option_units, Environment& env) override;
    BaseNode::PointerType clone(const std::string& nm) const override;
  };  
  
  class StringNode: public ValueNode {
    BaseValue::PointerType cast_scalar_value(const std::string value_input) const override;
    BaseValue::PointerType cast_array_value(const std::vector<std::string>& value_inputs, const BaseValue::ShapeType& shape) const override;
  public:
    static BaseNode::PointerType is_node(Parser& parser);
    StringNode(const std::string& nm, BaseValue::PointerType val): BaseNode(BaseNode::STRING), ValueNode(nm, std::move(val), BaseValue::STRING) {};
    StringNode(Parser& parser): BaseNode(parser, BaseNode::STRING), ValueNode(BaseValue::STRING) {};
    BaseNode::NodeListType parse(Environment& env) override;
    void set_option(const std::string option_value, const std::string option_units, Environment& env) override;
    BaseNode::PointerType clone(const std::string& nm) const override;
    void validate_format() const override;
  };
  
  // helper function that create a scalar value node pointer from a C++ data type
  template <typename T>
  BaseNode::PointerType create_scalar_node(const std::string& name, const T value) {
    BaseValue::PointerType ptr_value = create_scalar_value<T>(value);
    if constexpr (std::is_same_v<T, bool>)
      return std::make_shared<BooleanNode>(name, std::move(ptr_value));
    else if constexpr (std::is_same_v<T, short>)
      return std::make_shared<IntegerNode>(name, std::move(ptr_value), BaseValue::INTEGER_16);
    else if constexpr (std::is_same_v<T, unsigned short>)
      return std::make_shared<IntegerNode>(name, std::move(ptr_value), BaseValue::INTEGER_16_U);
    else if constexpr (std::is_same_v<T, int>)
      return std::make_shared<IntegerNode>(name, std::move(ptr_value), BaseValue::INTEGER_32);
    else if constexpr (std::is_same_v<T, unsigned int>)
      return std::make_shared<IntegerNode>(name, std::move(ptr_value), BaseValue::INTEGER_32_U);
    else if constexpr (std::is_same_v<T, long long>)
      return std::make_shared<IntegerNode>(name, std::move(ptr_value), BaseValue::INTEGER_64);
    else if constexpr (std::is_same_v<T, unsigned long long>)
      return std::make_shared<IntegerNode>(name, std::move(ptr_value), BaseValue::INTEGER_64_U);
    else if constexpr (std::is_same_v<T, float>)
      return std::make_shared<FloatNode>(name, std::move(ptr_value), BaseValue::FLOAT_32);
    else if constexpr (std::is_same_v<T, double>)
      return std::make_shared<FloatNode>(name, std::move(ptr_value), BaseValue::FLOAT_64);
    else if constexpr (std::is_same_v<T, long double>)
      return std::make_shared<FloatNode>(name, std::move(ptr_value), BaseValue::FLOAT_128);
    else if constexpr (std::is_same_v<T, std::string>)      
      return std::make_shared<StringNode>(name, std::move(ptr_value));
    else 
      static_assert(std::is_integral_v<T>, "Given data type is not associated with any scalar value node");
  }

  // helper function that create a array value node pointer from a C++ data type
  template <typename T>
  BaseNode::PointerType create_array_node(const std::string& name, const std::vector<T>&  arr, BaseValue::ShapeType sh={}) {
    if (sh.empty())
      sh.push_back(arr.size());
    BaseValue::PointerType ptr_value = create_array_value<T>(arr, sh);
    if constexpr (std::is_same_v<T, bool>)
      return std::make_shared<BooleanNode>(name, std::move(ptr_value));
    else if constexpr (std::is_same_v<T, short>)
      return std::make_shared<IntegerNode>(name, std::move(ptr_value), BaseValue::INTEGER_16);
    else if constexpr (std::is_same_v<T, unsigned short>)
      return std::make_shared<IntegerNode>(name, std::move(ptr_value), BaseValue::INTEGER_16_U);
    else if constexpr (std::is_same_v<T, int>)
      return std::make_shared<IntegerNode>(name, std::move(ptr_value), BaseValue::INTEGER_32);
    else if constexpr (std::is_same_v<T, unsigned int>)
      return std::make_shared<IntegerNode>(name, std::move(ptr_value), BaseValue::INTEGER_32_U);
    else if constexpr (std::is_same_v<T, long long>)
      return std::make_shared<IntegerNode>(name, std::move(ptr_value), BaseValue::INTEGER_64);
    else if constexpr (std::is_same_v<T, unsigned long long>)
      return std::make_shared<IntegerNode>(name, std::move(ptr_value), BaseValue::INTEGER_64_U);
    else if constexpr (std::is_same_v<T, float>)
      return std::make_shared<FloatNode>(name, std::move(ptr_value), BaseValue::FLOAT_32);
    else if constexpr (std::is_same_v<T, double>)
      return std::make_shared<FloatNode>(name, std::move(ptr_value), BaseValue::FLOAT_64);
    else if constexpr (std::is_same_v<T, long double>)
      return std::make_shared<FloatNode>(name, std::move(ptr_value), BaseValue::FLOAT_128);
    else if constexpr (std::is_same_v<T, std::string>)
      return std::make_shared<StringNode>(name, std::move(ptr_value));
    else
      static_assert(std::is_integral_v<T>, "Given data type is not associated with any array value node");
  };
  
}
  
#endif // DIP_NODES_H
