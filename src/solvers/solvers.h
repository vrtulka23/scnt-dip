#ifndef H_SOLVERS
#define H_SOLVERS

#include "../environment.h"

namespace dip {

  class FunctionSolver {
    const Environment& environment;
  public:
    FunctionSolver(const Environment& env): environment(env) {};
    BaseNode::NodeListType solve(const std::string& func_name, const std::string& in_units="");
  };
  
}

#endif // H_SOLVERS
