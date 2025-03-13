#include "solvers.h"

namespace dip {

  BaseValue::PointerType FunctionSolver::solve_value(const std::string& func_name, const std::string& in_units) const {
    FunctionList::ValueFunctionType func = environment.functions.get_value(func_name);
    BaseValue::PointerType value = func(environment);
    if (value == nullptr)
      throw std::runtime_error("Value function returns an empty pointer.");
    return std::move(value);
  }
  
  BaseNode::NodeListType FunctionSolver::solve_table(const std::string& func_name, const std::string& in_units) const {
    FunctionList::TableFunctionType func = environment.functions.get_table(func_name);
    BaseNode::NodeListType nodes = func(environment);
    if (nodes.empty())
      throw std::runtime_error("Table function returns an empty node list.");
    return nodes;
  }
  
}
