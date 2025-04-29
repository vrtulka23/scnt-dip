#include "solvers.h"
#include "../nodes/nodes.h"

namespace dip {

  LogicalAtom::LogicalAtom(const LogicalAtom& a): AtomBase(a.value->clone()) {
  }

  LogicalAtom& LogicalAtom::operator=(const LogicalAtom& a) {
    if (this != &a)
      value = a.value->clone();
    return *this;
  }
  
  BaseValue::PointerType LogicalAtom::from_string(std::string s, LogicalSettings* settings) {
    Parser parser({s, {"LOGICAL_ATOM",0}});
    if (parser.part_reference()) {
      BaseValue::PointerType value = settings->env->request_value(parser.value_raw.at(0), RequestType::Reference);
      return std::move(value);
    } else if (parser.part_literal()) {
      BaseNode::PointerType node = nullptr;
      if (node==nullptr) node = BooleanNode::is_node(parser);
      if (node==nullptr) node = IntegerNode::is_node(parser);
      if (node==nullptr) node = FloatNode::is_node(parser);
      if (node==nullptr) node = StringNode::is_node(parser);
      if (node==nullptr)
	throw std::runtime_error("Value could not be determined from : "+s);
      ValueNode::PointerType vnode = std::dynamic_pointer_cast<ValueNode>(node);
      vnode->set_value();
      return std::move(vnode->value);
    } else {
      throw std::runtime_error("Invalid atom value: "+s);
    }
  }
  
  std::string LogicalAtom::to_string() {
    return value->to_string();
  }

  // Comparison operations
  void LogicalAtom::comparison_equal(LogicalAtom *other) {
    value = create_scalar_value<bool>(*value==other->value.get());
  }
  void LogicalAtom::comparison_not_equal(LogicalAtom *other) {
    value = create_scalar_value<bool>(!(*value==other->value.get()));
  }
  void LogicalAtom::comparison_lower_equal(LogicalAtom *other) {
    value = create_scalar_value<bool>(!(*other->value<value.get()));
  }
  void LogicalAtom::comparison_greater_equal(LogicalAtom *other) {
    value = create_scalar_value<bool>(!(*value<other->value.get()));
  }
  void LogicalAtom::comparison_lower(LogicalAtom *other) {
    value = create_scalar_value<bool>(*value<other->value.get());
  }
  void LogicalAtom::comparison_greater(LogicalAtom *other) {
    value = create_scalar_value<bool>(*other->value<value.get());
  }

    // Logical operations
  void LogicalAtom::logical_not() {
    value = create_scalar_value<bool>(!*value);
  }
  void LogicalAtom::logical_and(LogicalAtom* other) {
    value = create_scalar_value<bool>(*value && *(other->value));
  }
  void LogicalAtom::logical_or(LogicalAtom* other) {
    value = create_scalar_value<bool>(*value || *(other->value));
  }
  
  LogicalSolver::LogicalSolver(Environment& env) {
    
    LogicalSettings settings = {&env};
	
    exs::OperatorList<LogicalAtom, LogicalSettings> operators;
    operators.append(exs::PARENTHESES_OPERATOR,    std::make_shared<exs::OperatorParentheses<LogicalAtom, LogicalSettings>>("( "," )"));
    operators.append(exs::EQUAL_OPERATOR,          std::make_shared<exs::OperatorEqual<LogicalAtom, LogicalSettings>>(" == "));
    operators.append(exs::NOT_EQUAL_OPERATOR,      std::make_shared<exs::OperatorNotEqual<LogicalAtom, LogicalSettings>>(" != "));
    operators.append(exs::NOT_OPERATOR,            std::make_shared<exs::OperatorNot<LogicalAtom, LogicalSettings>>()); // needs to be after NOT_EQUAL
    operators.append(exs::LOWER_EQUAL_OPERATOR,    std::make_shared<exs::OperatorLowerEqual<LogicalAtom, LogicalSettings>>(" <= "));
    operators.append(exs::GREATER_EQUAL_OPERATOR,  std::make_shared<exs::OperatorGreaterEqual<LogicalAtom, LogicalSettings>>(" >= "));
    operators.append(exs::LOWER_OPERATOR,          std::make_shared<exs::OperatorLower<LogicalAtom, LogicalSettings>>(" < "));
    operators.append(exs::GREATER_OPERATOR,        std::make_shared<exs::OperatorGreater<LogicalAtom, LogicalSettings>>(" > "));
    operators.append(exs::AND_OPERATOR,            std::make_shared<exs::OperatorAnd<LogicalAtom, LogicalSettings>>(" && "));
    operators.append(exs::OR_OPERATOR,             std::make_shared<exs::OperatorOr<LogicalAtom, LogicalSettings>>(" || "));
    
    exs::StepList steps;
    steps.append(exs::GROUP_OPERATION,  {exs::PARENTHESES_OPERATOR});
    steps.append(exs::BINARY_OPERATION,    {
	exs::LOWER_EQUAL_OPERATOR,exs::GREATER_EQUAL_OPERATOR,
	exs::LOWER_OPERATOR,exs::GREATER_OPERATOR
      });
    steps.append(exs::BINARY_OPERATION, {exs::EQUAL_OPERATOR,exs::NOT_EQUAL_OPERATOR});
    steps.append(exs::UNARY_OPERATION,  {exs::NOT_OPERATOR});
    steps.append(exs::BINARY_OPERATION, {exs::AND_OPERATOR});
    steps.append(exs::BINARY_OPERATION, {exs::OR_OPERATOR});
    
    solver = std::make_unique<exs::Solver<LogicalAtom, LogicalSettings>>(operators, steps, settings);
    
  }
  
  LogicalAtom LogicalSolver::eval(const std::string& expression) {
    if (expression.empty())
      throw std::runtime_error("Logical expression cannot be empty");
    LogicalAtom ua = solver->solve(expression);
    return ua;
  }

}
