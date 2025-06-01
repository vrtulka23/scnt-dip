#ifndef DIP_NODES_H
#define DIP_NODES_H

#include <vector>
#include <deque>
#include <iostream>
#include <sstream>
#include <iomanip>

#include "../settings.h"
#include "../values/values.h"

namespace dip {

  enum class ValueOrigin {
    Keyword, String, Reference, ReferenceRaw, Function, Expression
  };

  enum class NodeDtype {
    None, Empty,                                           // empty
    Unit, Source,                                          // environment
    Group, Case, Import,                                   // node structure
    Boolean, Integer, Float, String, Table, Modification,  // data handling
    Property,                                              // properties
  };
  
  enum class PropertyType {
    None,                                                  // not a property
    Constant, Condition, Tags, Description,                // global properties
    Format, Options, Delimiter                             // specific properties
  };

  class Node {
  public:
    Line line;                             // source code line information; in Python this were 'code' & 'source' variables
    size_t indent;                         // indent of a node
    std::string name;                      // node name
    std::array<std::string,3> dtype_raw;   // data type properties (unsigned/type/precision)
    Array::StringType value_raw;           // raw value string(s)
    Array::ShapeType value_shape;          // shape of an array value
    ValueOrigin value_origin;              // origin of the value; in Python there were separate variables: value_ref, value_expr, value_func
    Array::RangeType value_slice;          // slice of an injected node value
    std::string units_raw;                 // raw units string
    Array::RangeType dimension;            // list of array dimensions
    Node(): indent(0), value_origin(ValueOrigin::String) {};
    Node(const Line& l): line(l), indent(0), value_origin(ValueOrigin::String) {};
    Node(const std::string& nm): name(nm), indent(0), value_origin(ValueOrigin::String) {};
    virtual ~Node() = default;
    std::string to_string();
  };
  
  class Parser: public Node {
  private:
    void strip(const std::string& text); 
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
    bool kwd_property(PropertyType& ptype);
    bool part_space(const bool required=true);
    bool part_indent();
    bool part_name(const bool required=true);
    bool part_type(const bool required=true);
    bool part_literal();
    bool part_dimension();
    bool part_equal(const bool required=true);
    bool part_reference();
    bool part_expression();
    bool part_function();
    bool part_array();
    bool part_string();
    bool part_keyword(const bool required=true);
    bool part_value();
    bool part_slice();
    bool part_units();
    bool part_comment();
    bool part_delimiter(const char symbol, const bool required=true);
  };

  class BaseNode: virtual public Node {
  public:
    NodeDtype dtype;                       // data type of a node; in Python this was 'keyword' variable in Node class
    size_t branch_id;
    size_t case_id;
    typedef std::shared_ptr<BaseNode> PointerType;
    typedef std::deque<BaseNode::PointerType> NodeListType;
    BaseNode(): dtype(NodeDtype::None) {};
    BaseNode(const NodeDtype dt): dtype(dt) {};
    BaseNode(Parser& parser, const NodeDtype dt);
    virtual ~BaseNode() = default;
    virtual NodeListType parse(Environment& env);
    virtual bool set_property(PropertyType property, Array::StringType& values, std::string& units);
  };

  class EmptyNode: public BaseNode {
  public:
    static BaseNode::PointerType is_node(Parser& parser);
    EmptyNode(Parser& parser): BaseNode(parser, NodeDtype::Empty) {};
  };

  class ImportNode: public BaseNode {
  public:
    static BaseNode::PointerType is_node(Parser& parser);
    ImportNode(Parser& parser): BaseNode(parser, NodeDtype::Import) {};
    BaseNode::NodeListType parse(Environment& env) override;
  };

  class UnitNode: public BaseNode {
  public:
    static BaseNode::PointerType is_node(Parser& parser);
    UnitNode(Parser& parser): BaseNode(parser, NodeDtype::Unit) {};
    BaseNode::NodeListType parse(Environment& env) override;
  };
  
  class SourceNode: public BaseNode {
  public:
    static BaseNode::PointerType is_node(Parser& parser);
    SourceNode(Parser& parser): BaseNode(parser, NodeDtype::Source) {};
    BaseNode::NodeListType parse(Environment& env) override;
  };
  
  class CaseNode: public BaseNode {
  public:
    bool value;             // case value
    size_t case_id;         // ID of a case
    CaseType case_type;     // type of a case
    static BaseNode::PointerType is_node(Parser& parser);
    CaseNode(Parser& parser): BaseNode(parser, NodeDtype::Case), case_id(0), value(false) {};
    BaseNode::NodeListType parse(Environment& env) override;
  };
  
  class GroupNode: public BaseNode {
  public:
    static BaseNode::PointerType is_node(Parser& parser);
    GroupNode(Parser& parser): BaseNode(parser, NodeDtype::Group) {};
  };
    
  class ModificationNode: public BaseNode {
  public:
    static BaseNode::PointerType is_node(Parser& parser);
    ModificationNode(Parser& parser): BaseNode(parser, NodeDtype::Modification) {};
  };
  
  class TableNode: public BaseNode {
  public:
    char delimiter;
    static BaseNode::PointerType is_node(Parser& parser);
    TableNode(Parser& parser): BaseNode(parser, NodeDtype::Table), delimiter(SEPARATOR_TABLE_COLUMNS) {};
    BaseNode::NodeListType parse(Environment& env) override;
    bool set_property(PropertyType property, Array::StringType& values, std::string& units) override;
  };

  /*
   *  Value nodes
   */
  
  class ValueNode: virtual public BaseNode {
    virtual BaseValue::PointerType cast_scalar_value(const std::string& value_input) const = 0;
    virtual BaseValue::PointerType cast_array_value(const Array::StringType& value_inputs, const Array::ShapeType& shape) const = 0;
  protected:
    struct OptionStruct {
      BaseValue::PointerType value;
      std::string value_raw;
      std::string units_raw;
    };
    ValueDtype value_dtype;
  public:
    typedef std::shared_ptr<ValueNode> PointerType;
    BaseValue::PointerType value;
    Array::StringType tags;
    bool constant;
    std::string description;
    std::string condition;
    std::vector<OptionStruct> options;
    std::string format;
    ValueNode(): constant(false) {};
    ValueNode(const ValueDtype vdt): constant(false), value_dtype(vdt) {};
    ValueNode(const std::string& nm, BaseValue::PointerType val, const ValueDtype vdt);
    virtual ~ValueNode() = default;
    BaseValue::PointerType cast_value();
    BaseValue::PointerType cast_value(Array::StringType& value_input, const Array::ShapeType& shape);
    void set_value(BaseValue::PointerType value_input=nullptr);
    void modify_value(BaseNode::PointerType node, Environment& env);
    virtual BaseNode::PointerType clone(const std::string& nm) const = 0;
    virtual bool set_property(PropertyType property, Array::StringType& values, std::string& units) override;
    void validate_constant() const;
    void validate_definition() const;
    void validate_condition() const;
    virtual void validate_options() const;
    virtual void validate_format() const;
  private:
    void validate_dimensions() const;
  };
  
  class BooleanNode: public ValueNode {
    BaseValue::PointerType cast_scalar_value(const std::string& value_input) const override;
    BaseValue::PointerType cast_array_value(const Array::StringType& value_inputs, const Array::ShapeType& shape) const override;
  public:
    static BaseNode::PointerType is_node(Parser& parser);
    BooleanNode(const std::string& nm, BaseValue::PointerType val): BaseNode(NodeDtype::Boolean), ValueNode(nm, std::move(val), ValueDtype::Boolean) {};
    BooleanNode(Parser& parser): BaseNode(parser, NodeDtype::Boolean), ValueNode(ValueDtype::Boolean) {};
    BaseNode::NodeListType parse(Environment& env) override;
    BaseNode::PointerType clone(const std::string& nm) const override;
    void validate_options() const override;
  };  
  
  class StringNode: public ValueNode {
    BaseValue::PointerType cast_scalar_value(const std::string& value_input) const override;
    BaseValue::PointerType cast_array_value(const Array::StringType& value_inputs, const Array::ShapeType& shape) const override;
  public:
    static BaseNode::PointerType is_node(Parser& parser);
    StringNode(const std::string& nm, BaseValue::PointerType val): BaseNode(NodeDtype::String), ValueNode(nm, std::move(val), ValueDtype::String) {};
    StringNode(Parser& parser): BaseNode(parser, NodeDtype::String), ValueNode(ValueDtype::String) {};
    BaseNode::NodeListType parse(Environment& env) override;
    BaseNode::PointerType clone(const std::string& nm) const override;
    bool set_property(PropertyType property, Array::StringType& values, std::string& units) override;
    void validate_format() const override;
  };

  /*
   *  Quantity nodes
   */

  class QuantityNode: virtual public ValueNode {
  public:
    typedef std::shared_ptr<QuantityNode> PointerType;
    Quantity::PointerType units;
    void set_units(Quantity::PointerType units_input=nullptr);
    virtual ~QuantityNode() = default;
  };
  
  class IntegerNode: public QuantityNode {
    BaseValue::PointerType cast_scalar_value(const std::string& value_input) const override;
    BaseValue::PointerType cast_array_value(const Array::StringType& value_inputs, const Array::ShapeType& shape) const override;
  public:
    static constexpr size_t max_int_size = sizeof(long long) * CHAR_BIT;
    static BaseNode::PointerType is_node(Parser& parser);
    IntegerNode(const std::string& nm, BaseValue::PointerType val, const ValueDtype vdt): BaseNode(NodeDtype::Boolean), ValueNode(nm, std::move(val), vdt) {};
    IntegerNode(Parser& parser);
    BaseNode::NodeListType parse(Environment& env) override;
    BaseNode::PointerType clone(const std::string& nm) const override;
  };  
  
  class FloatNode: public QuantityNode {
    BaseValue::PointerType cast_scalar_value(const std::string& value_input) const override;
    BaseValue::PointerType cast_array_value(const Array::StringType& value_inputs, const Array::ShapeType& shape) const override;
  public:
    static constexpr size_t max_float_size = sizeof(long double) * CHAR_BIT;
    static BaseNode::PointerType is_node(Parser& parser);
    FloatNode(const std::string& nm, BaseValue::PointerType val, const ValueDtype vdt): BaseNode(NodeDtype::Float), ValueNode(nm, std::move(val), vdt) {};
    FloatNode(Parser& parser);
    BaseNode::NodeListType parse(Environment& env) override;
    BaseNode::PointerType clone(const std::string& nm) const override;
  };  
  
  /*
   *  Property nodes
   */

  class PropertyNode: public virtual BaseNode {
  public:
    typedef std::shared_ptr<PropertyNode> PointerType;
    PropertyType ptype; 
    static BaseNode::PointerType is_node(Parser& parser);
    PropertyNode(Parser& parser, PropertyType pt): BaseNode(parser, NodeDtype::Property), ptype(pt) {};
  };
  
  // helper function that create a scalar value node pointer from a C++ data type
  template <typename T>
  BaseNode::PointerType create_scalar_node(const std::string& name, const T value) {
    BaseValue::PointerType ptr_value = create_scalar_value<T>(value);
    if constexpr (std::is_same_v<T, bool>)
      return std::make_shared<BooleanNode>(name, std::move(ptr_value));
    else if constexpr (std::is_same_v<T, short>)
      return std::make_shared<IntegerNode>(name, std::move(ptr_value), ValueDtype::Integer16);
    else if constexpr (std::is_same_v<T, unsigned short>)
      return std::make_shared<IntegerNode>(name, std::move(ptr_value), ValueDtype::Integer16_U);
    else if constexpr (std::is_same_v<T, int>)
      return std::make_shared<IntegerNode>(name, std::move(ptr_value), ValueDtype::Integer32);
    else if constexpr (std::is_same_v<T, unsigned int>)
      return std::make_shared<IntegerNode>(name, std::move(ptr_value), ValueDtype::Integer32_U);
    else if constexpr (std::is_same_v<T, long long>)
      return std::make_shared<IntegerNode>(name, std::move(ptr_value), ValueDtype::Integer64);
    else if constexpr (std::is_same_v<T, unsigned long long>)
      return std::make_shared<IntegerNode>(name, std::move(ptr_value), ValueDtype::Integer64_U);
    else if constexpr (std::is_same_v<T, float>)
      return std::make_shared<FloatNode>(name, std::move(ptr_value), ValueDtype::Float32);
    else if constexpr (std::is_same_v<T, double>)
      return std::make_shared<FloatNode>(name, std::move(ptr_value), ValueDtype::Float64);
    else if constexpr (std::is_same_v<T, long double>)
      return std::make_shared<FloatNode>(name, std::move(ptr_value), ValueDtype::Float128);
    else if constexpr (std::is_same_v<T, std::string>)      
      return std::make_shared<StringNode>(name, std::move(ptr_value));
    else 
      static_assert(std::is_integral_v<T>, "Given data type is not associated with any scalar value node");
  }

  // helper function that create a array value node pointer from a C++ data type
  template <typename T>
  BaseNode::PointerType create_array_node(const std::string& name, const std::vector<T>&  arr, Array::ShapeType sh={}) {
    if (sh.empty())
      sh.push_back(arr.size());
    BaseValue::PointerType ptr_value = create_array_value<T>(arr, sh);
    if constexpr (std::is_same_v<T, bool>)
      return std::make_shared<BooleanNode>(name, std::move(ptr_value));
    else if constexpr (std::is_same_v<T, short>)
      return std::make_shared<IntegerNode>(name, std::move(ptr_value), ValueDtype::Integer16);
    else if constexpr (std::is_same_v<T, unsigned short>)
      return std::make_shared<IntegerNode>(name, std::move(ptr_value), ValueDtype::Integer16_U);
    else if constexpr (std::is_same_v<T, int>)
      return std::make_shared<IntegerNode>(name, std::move(ptr_value), ValueDtype::Integer32);
    else if constexpr (std::is_same_v<T, unsigned int>)
      return std::make_shared<IntegerNode>(name, std::move(ptr_value), ValueDtype::Integer32_U);
    else if constexpr (std::is_same_v<T, long long>)
      return std::make_shared<IntegerNode>(name, std::move(ptr_value), ValueDtype::Integer64);
    else if constexpr (std::is_same_v<T, unsigned long long>)
      return std::make_shared<IntegerNode>(name, std::move(ptr_value), ValueDtype::Integer64_U);
    else if constexpr (std::is_same_v<T, float>)
      return std::make_shared<FloatNode>(name, std::move(ptr_value), ValueDtype::Float32);
    else if constexpr (std::is_same_v<T, double>)
      return std::make_shared<FloatNode>(name, std::move(ptr_value), ValueDtype::Float64);
    else if constexpr (std::is_same_v<T, long double>)
      return std::make_shared<FloatNode>(name, std::move(ptr_value), ValueDtype::Float128);
    else if constexpr (std::is_same_v<T, std::string>)
      return std::make_shared<StringNode>(name, std::move(ptr_value));
    else
      static_assert(std::is_integral_v<T>, "Given data type is not associated with any array value node");
  };
  
}
  
#endif // DIP_NODES_H
