#include <map>

#include "values.h"

namespace dip {
  
  std::unordered_map<ValueDtype, std::string> ValueDtypeNames = {
    {ValueDtype::Boolean,     "bool"},
    {ValueDtype::String,      "str"},
    {ValueDtype::Integer16,   "int16"},
    {ValueDtype::Integer32,   "int32"},
    {ValueDtype::Integer64,   "int64"},
    {ValueDtype::IntegerX,    "intx"},
    {ValueDtype::Integer16_U, "uint16"},
    {ValueDtype::Integer32_U, "uint32"},
    {ValueDtype::Integer64_U, "uint64"},
    {ValueDtype::Float32,     "float32"},
    {ValueDtype::Float64,     "float64"},
    {ValueDtype::Float128,    "float128"},
    {ValueDtype::FloatX,      "floatx"}
  };

}
