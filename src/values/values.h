#ifndef DIP_VALUES_H
#define DIP_VALUES_H

#include <typeinfo>

#include "../settings.h"

namespace dip {

  enum class ValueDtype { // TODO: INTX and FLOATX should support arbitrary precision types
    Boolean,     String,
    Integer16,   Integer32,   Integer64,    IntegerX,   
    Integer16_U, Integer32_U, Integer64_U,  
    Float32,     Float64,     Float128,     FloatX
  };

  extern std::unordered_map<ValueDtype, std::string> ValueDtypeNames;

  template <typename T>
  class ArrayValue;
  
  class BaseValue {
  public:
    typedef std::unique_ptr<BaseValue> PointerType;
    ValueDtype dtype;
    BaseValue(ValueDtype dt): dtype(dt) {};
    virtual ~BaseValue() = default;
    virtual void print() = 0;
    virtual std::string to_string(const int precision=0) const = 0;
    virtual Array::ShapeType get_shape() const = 0;
    virtual size_t get_size() const = 0;
    virtual BaseValue::PointerType clone() const = 0;
    virtual BaseValue::PointerType slice(const Array::RangeType& slice) = 0;
    virtual void convert_units(const std::string& from_units, const Quantity::PointerType& to_quantity) = 0;
    virtual void convert_units(const Quantity::PointerType& from_quantity, const std::string& to_units) = 0;
    virtual bool operator==(const BaseValue* other) const = 0;
    virtual bool operator<(const BaseValue* other) const = 0;
    virtual explicit operator bool() const = 0;
    virtual explicit operator short() const = 0;
    virtual explicit operator unsigned short() const = 0;
    virtual explicit operator int() const = 0;
    virtual explicit operator unsigned int() const = 0;
    virtual explicit operator long long() const = 0;
    virtual explicit operator unsigned long long() const = 0;
    virtual explicit operator float() const = 0;
    virtual explicit operator double() const = 0;
    virtual explicit operator long double() const = 0;
    virtual explicit operator std::string() const = 0;
  };

}

#include "values_scalar.h"
#include "values_array.h"

namespace dip {
  
  // helper function that create a scalar value pointer from a C++ data type
  template <typename T>
  BaseValue::PointerType create_scalar_value(T value) {
    if constexpr (std::is_same_v<T, bool>)
      return std::make_unique<ScalarValue<bool>>(ScalarValue<bool>(value));
    else if constexpr (std::is_same_v<T, short>)
      return std::make_unique<ScalarValue<short>>(ScalarValue<short>(value, ValueDtype::Integer16));
    else if constexpr (std::is_same_v<T, unsigned short>)
      return std::make_unique<ScalarValue<unsigned short>>(ScalarValue<unsigned short>(value, ValueDtype::Integer16_U));
    else if constexpr (std::is_same_v<T, int>)
      return std::make_unique<ScalarValue<int>>(ScalarValue<int>(value, ValueDtype::Integer32));
    else if constexpr (std::is_same_v<T, unsigned int>)
      return std::make_unique<ScalarValue<unsigned int>>(ScalarValue<unsigned int>(value, ValueDtype::Integer32_U));
    else if constexpr (std::is_same_v<T, long long>)
      return std::make_unique<ScalarValue<long long>>(ScalarValue<long long>(value, ValueDtype::Integer64));
    else if constexpr (std::is_same_v<T, unsigned long long>)
      return std::make_unique<ScalarValue<unsigned long long>>(ScalarValue<unsigned long long>(value, ValueDtype::Integer64_U));
    else if constexpr (std::is_same_v<T, float>)
      return std::make_unique<ScalarValue<float>>(ScalarValue<float>(value, ValueDtype::Float32));
    else if constexpr (std::is_same_v<T, double>)
      return std::make_unique<ScalarValue<double>>(ScalarValue<double>(value, ValueDtype::Float64));
    else if constexpr (std::is_same_v<T, long double>)
      return std::make_unique<ScalarValue<long double>>(ScalarValue<long double>(value, ValueDtype::Float128));
    else if constexpr (std::is_same_v<T, std::string>)
      return std::make_unique<ScalarValue<std::string>>(ScalarValue<std::string>(value));
    else
      static_assert(std::is_integral_v<T>, "Given data type is not associated with any scalar value");
  };

  // helper function that create a array value pointer from a C++ data type
  template <typename T>
  BaseValue::PointerType create_array_value(const std::vector<T>&  arr, Array::ShapeType sh={}) {
    if (sh.empty())
      sh.push_back(arr.size());
    if constexpr (std::is_same_v<T, bool>)
      return std::make_unique<ArrayValue<bool>>(ArrayValue<bool>(arr, sh));
    else if constexpr (std::is_same_v<T, short>)
      return std::make_unique<ArrayValue<short>>(ArrayValue<short>(arr, sh, ValueDtype::Integer16));
    else if constexpr (std::is_same_v<T, unsigned short>)
      return std::make_unique<ArrayValue<unsigned short>>(ArrayValue<unsigned short>(arr, sh, ValueDtype::Integer16_U));
    else if constexpr (std::is_same_v<T, int>)
      return std::make_unique<ArrayValue<int>>(ArrayValue<int>(arr, sh, ValueDtype::Integer32));
    else if constexpr (std::is_same_v<T, unsigned int>)
      return std::make_unique<ArrayValue<unsigned int>>(ArrayValue<unsigned int>(arr, sh, ValueDtype::Integer32_U));
    else if constexpr (std::is_same_v<T, long long>)
      return std::make_unique<ArrayValue<long long>>(ArrayValue<long long>(arr, sh, ValueDtype::Integer64));
    else if constexpr (std::is_same_v<T, unsigned long long>)
      return std::make_unique<ArrayValue<unsigned long long>>(ArrayValue<unsigned long long>(arr, sh, ValueDtype::Integer64_U));
    else if constexpr (std::is_same_v<T, float>)
      return std::make_unique<ArrayValue<float>>(ArrayValue<float>(arr, sh, ValueDtype::Float32));
    else if constexpr (std::is_same_v<T, double>)
      return std::make_unique<ArrayValue<double>>(ArrayValue<double>(arr, sh, ValueDtype::Float64));
    else if constexpr (std::is_same_v<T, long double>)
      return std::make_unique<ArrayValue<long double>>(ArrayValue<long double>(arr, sh, ValueDtype::Float128));
    else if constexpr (std::is_same_v<T, std::string>)
      return std::make_unique<ArrayValue<std::string>>(ArrayValue<std::string>(arr, sh));
    else 
      static_assert(std::is_integral_v<T>, "Given data type is not associated with any array value");
  };
  
}

#endif // DIP_VALUES_H
