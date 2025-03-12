#include "solvers.h"

namespace dip {

  FunctionList::FunctionReturnType FunctionSolver::solve(const std::string& func_name, const std::string& in_units) {
    FunctionList::FunctionType func = environment.functions[func_name];
    return func(environment);
  }
  
}
