#ifndef H_SOLVERS
#define H_SOLVERS

#include <scnt-exs/exs.h>

#include "../environment.h"
#include "../values/values.h"

namespace dip {

  struct LogicalSettings {
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
    
    static BaseValue::PointerType from_string(std::string s, LogicalSettings* settings);
    std::string to_string();
    void comparison_equal(LogicalAtom *other);
    void comparison_not_equal(LogicalAtom *other);
    void comparison_lower_equal(LogicalAtom *other);
    void comparison_greater_equal(LogicalAtom *other);
    void comparison_lower(LogicalAtom *other);
    void comparison_greater(LogicalAtom *other);
    void logical_not();
    void logical_and(LogicalAtom* other);
    void logical_or(LogicalAtom* other);
  };

  class LogicalSolver {
  public:
    std::unique_ptr<exs::Solver<LogicalAtom, LogicalSettings>> solver;
    LogicalSolver(Environment& env);
    LogicalAtom eval(const std::string& expression);
  };
  
  class NumericalSolver {
    
  };
  
  class TemplateSolver {
    
  };
  
}

#endif // H_SOLVERS
