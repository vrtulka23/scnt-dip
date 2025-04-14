#ifndef H_SOLVERS
#define H_SOLVERS

#include <scnt-exs/exs.h>

#include "../environment.h"
#include "../nodes/values.h"

namespace dip {

  struct Settings {
    Environment* env;
  };
  
  class LogicalAtom: public exs::AtomBase<BaseValue::PointerType> {
  public:
    // Constructor from unique_ptr
    LogicalAtom(BaseValue::PointerType b): AtomBase(std::move(b)) {};
    // Deep copy constructor
    LogicalAtom(const LogicalAtom& a);
    LogicalAtom& operator=(const LogicalAtom& a);
    // Move constructor
    LogicalAtom(LogicalAtom&& a) noexcept = default;
    LogicalAtom& operator=(LogicalAtom&& a) noexcept = default;
    
    static BaseValue::PointerType from_string(std::string s, Settings* settings);
    std::string to_string();
    void logical_and(LogicalAtom* other);
    void logical_or(LogicalAtom* other);
  };

  class LogicalSolver {
  public:
    std::unique_ptr<exs::Solver<LogicalAtom, Settings>> solver;
    LogicalSolver(Environment& env);
    LogicalAtom solve(const std::string& expression);
  };
  
  class NumericalSolver {
    
  };
  
  class TemplateSolver {
    
  };
  
}

#endif // H_SOLVERS
