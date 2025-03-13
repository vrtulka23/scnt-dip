#ifndef H_SOLVERS
#define H_SOLVERS

#include "../environment.h"

namespace dip {

  class FunctionSolver {
    const Environment& environment;
  public:
    FunctionSolver(const Environment& env): environment(env) {};
    BaseValue::PointerType solve_value(const std::string& func_name, const std::string& in_units="") const;
    BaseNode::NodeListType solve_table(const std::string& func_name, const std::string& in_units="") const;
  };
  
}

#endif // H_SOLVERS
