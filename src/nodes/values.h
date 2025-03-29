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

  class BaseValue {
  public:
    typedef std::unique_ptr<BaseValue> PointerType;
    typedef std::vector<int> ShapeType;
    ValueDtype dtype;
    BaseValue(ValueDtype dt): dtype(dt) {};
    virtual ~BaseValue() = default;
    virtual void print() = 0;
    virtual std::string to_string(const int precision=0) const = 0;
    virtual bool equals_to(const BaseValue* other) const = 0;
    virtual BaseValue::ShapeType get_shape() const = 0;
    virtual size_t get_size() const = 0;
    virtual BaseValue::PointerType clone() const = 0;
    virtual void convert_units(const std::string& from_units, const Quantity::PointerType& to_quantity) = 0;
    virtual void convert_units(const Quantity::PointerType& from_quantity, const std::string& to_units) = 0;
  };

  
  // Scalar values
  
  template <typename T>
  class BaseScalarValue: public BaseValue {
  protected:
    T value;
  public:
    BaseScalarValue(const T& val, const ValueDtype dt): value(val), BaseValue(dt) {};
    void print() override {std::cout << value << std::endl;};
    T get_value() {return value;};
    BaseValue::ShapeType get_shape() const override { return {1};};
    size_t get_size() const override {return 1;};
  protected:
    virtual void value_to_string(std::ostringstream& oss, int precision=0) const = 0;
  public:
    std::string to_string(const int precision=0) const override {
      std::ostringstream oss;
      value_to_string(oss, precision);
      return oss.str();
    };
    bool equals_to(const BaseValue* other) const override {
      const BaseScalarValue<T>* otherT = dynamic_cast<const BaseScalarValue<T>*>(other);
      if (otherT)
	return (value==otherT->value) ? true : false;
      else
	throw std::runtime_error("Could not convert BaseValue into the BaseScalarValue.");
    };
    void convert_units(const std::string& from_units, const Quantity::PointerType& to_quantity) override {
      throw std::runtime_error("Scalar value of type '"+std::string(ValueDtypeNames[dtype])+"' does not support unit conversion.");
    };
    void convert_units(const Quantity::PointerType& from_quantity, const std::string& to_units) override {
      throw std::runtime_error("Scalar value of type '"+std::string(ValueDtypeNames[dtype])+"' does not support unit conversion.");
    };
  };
  
  template <typename T>
  class ScalarValue: public BaseScalarValue<T> {
  public:
    ScalarValue(const T& val, const ValueDtype dt): BaseScalarValue<T>(val, dt) {};
  protected:
    void value_to_string(std::ostringstream& oss, int precision=0) const override {
      if (precision==0) precision=DISPLAY_FLOAT_PRECISION;
      int exponent = static_cast<int>(std::log10(std::fabs(this->value)));
      if (exponent > 3 || exponent < -3) {
        oss << std::scientific << std::setprecision(precision) << this->value;
      } else {
        oss << std::fixed << std::setprecision(precision-exponent) << this->value;
      }
    };
    BaseValue::PointerType clone() const override {
      return std::make_unique<ScalarValue<T>>(this->value, this->dtype);
    }
    void convert_units(const std::string& from_units, const Quantity::PointerType& to_quantity) override {
      // TODO: use the same BaseValue pointers in the puq to allow variable precision
      puq::Quantity quantity(this->value, from_units);
      quantity = quantity.convert(*to_quantity);
      this->value = quantity.value.magnitude.value.value.at(0);
    };
    void convert_units(const Quantity::PointerType& from_quantity, const std::string& to_units) override {
      // TODO: use the same BaseValue pointers in the puq to allow variable precision
      puq::Quantity quantity = this->value * (*from_quantity);
      quantity = quantity.convert(to_units);
      this->value = quantity.value.magnitude.value.value.at(0);
    };
  };
  
  template <>
  class ScalarValue<std::string> : public BaseScalarValue<std::string> {
  public:
    ScalarValue(const std::string& val, const ValueDtype dt) : BaseScalarValue(val,dt) {};
    ScalarValue(const std::string& val): ScalarValue(val,ValueDtype::String) {};
  private:
    void value_to_string(std::ostringstream& oss, int precision=0) const override {    
      if (precision==0)
	oss << value;
      else
	throw std::runtime_error("String value does not support precision parameter for to_string() method.");
    };
    BaseValue::PointerType clone() const override {
      return std::make_unique<ScalarValue<std::string>>(this->value, this->dtype);
    }
  };

  template <>
  class ScalarValue<bool> : public BaseScalarValue<bool> {
  public:
    ScalarValue(const bool& val, const ValueDtype dt) : BaseScalarValue(val,dt) {};
    ScalarValue(const bool& val): ScalarValue(val,ValueDtype::Boolean) {};
  private:
    void value_to_string(std::ostringstream& oss, int precision=0) const override {    
      if (precision==0)
	if (value)
	  oss << KEYWORD_TRUE;
	else
	  oss << KEYWORD_FALSE;
      else
	throw std::runtime_error("Boolean value does not support precision parameter for to_string() method.");
    };
    BaseValue::PointerType clone() const override {
      return std::make_unique<ScalarValue<bool>>(this->value, this->dtype);
    }
  };

  // Array values
  
  template <typename T>
  class BaseArrayValue: public BaseValue {
  protected:
    std::vector<T> value;
    BaseValue::ShapeType shape;
  public:
    BaseArrayValue(const T& val, const BaseValue::ShapeType& sh, const ValueDtype dt): value({val}), shape(sh), BaseValue(dt) {};
    BaseArrayValue(const std::vector<T>&  arr, const BaseValue::ShapeType& sh, const ValueDtype dt): value(arr), shape(sh), BaseValue(dt) {};
    void print() override {std::cout << to_string() << std::endl;};
    std::vector<T> get_value() {return value;};
    BaseValue::ShapeType get_shape() const override {return shape;};
    size_t get_size() const override {return value.size();};
  protected:
    virtual void value_to_string(std::ostringstream& oss, size_t& offset, int precision=0) const = 0;
    std::string to_string_dim(size_t& offset, const int& precision=0, int dim=0) const {
      std::ostringstream oss;
      oss << "[";
      for (int i=0; i<shape[dim];i++) {
	if (i>0) oss << ", ";
	if (dim+1<shape.size()) {
	  oss << to_string_dim(offset, precision, dim+1);
	} else {
	  value_to_string(oss, offset, precision);
	  offset++;
	}
      }
      oss << "]";
      return oss.str();
    }
  public:
    std::string to_string(const int precision=0) const override {
      size_t offset = 0;
      if (precision==0) {
	return to_string_dim(offset);
      } else {
	return to_string_dim(offset, precision);
      }
    };
    bool equals_to(const BaseValue* other) const override {
      const BaseArrayValue<T>* otherT = dynamic_cast<const BaseArrayValue<T>*>(other);
      if (otherT) {
	bool is_equal = true;
	for (int i=0; i<value.size();i++)
	  if (value[i]!=otherT->value[i])
	    is_equal = false;
	for (int i=0; i<shape.size();i++)
	  if (shape[i]!=otherT->shape[i])
	    is_equal = false;	
	return is_equal;
      } else {
	throw std::runtime_error("Could not convert BaseValue into a BaseArrayValue");
      }
    };
    void convert_units(const std::string& from_units, const Quantity::PointerType& to_quantity) override {
      throw std::runtime_error("Array value of type '"+std::string(ValueDtypeNames[dtype])+"' does not support unit conversion.");
    };
    void convert_units(const Quantity::PointerType& from_quantity, const std::string& to_units) override {
      throw std::runtime_error("Array value of type '"+std::string(ValueDtypeNames[dtype])+"' does not support unit conversion.");
    };
  };
  
  template <typename T>
  class ArrayValue: public BaseArrayValue<T> {
  public:
    ArrayValue(const T& val, const BaseValue::ShapeType& sh, const ValueDtype dt): BaseArrayValue<T>(val,sh,dt) {};
    ArrayValue(const std::vector<T>&  arr, const BaseValue::ShapeType& sh, const ValueDtype dt): BaseArrayValue<T>(arr,sh,dt) {};
  private:
    void value_to_string(std::ostringstream& oss, size_t& offset, int precision=0) const override {
      if (precision==0) precision=DISPLAY_FLOAT_PRECISION;
      int exponent = static_cast<int>(std::log10(std::fabs(this->value[offset])));
      if (exponent > 3 || exponent < -3) {
        oss << std::scientific << std::setprecision(precision) << this->value[offset];
      } else {
        oss << std::fixed << std::setprecision(precision-exponent) << this->value[offset];
      }
    };
    BaseValue::PointerType clone() const override {
      return std::make_unique<ArrayValue<T>>(this->value, this->shape, this->dtype);
    };
    void convert_units(const std::string& from_units, const Quantity::PointerType& to_quantity) override {
      // TODO: use the same BaseValue pointers in the puq to allow variable precision
      std::vector<double> input(this->value.begin(), this->value.end());
      puq::Quantity quantity = puq::Quantity(input, from_units);
      quantity = quantity.convert(*to_quantity);
      std::vector<double> output = quantity.value.magnitude.value.value;
      std::copy(output.begin(), output.end(), this->value.begin());
    };
    void convert_units(const Quantity::PointerType& from_quantity, const std::string& to_units) override {
      // TODO: use the same BaseValue pointers in the puq to allow variable precision
      std::vector<double> input(this->value.begin(), this->value.end());
      puq::Quantity quantity = input * (*from_quantity);
      quantity = quantity.convert(to_units);
      std::vector<double> output = quantity.value.magnitude.value.value;
      std::copy(output.begin(), output.end(), this->value.begin());
    };
  };
  
  template <>
  class ArrayValue<std::string>: public BaseArrayValue<std::string> {
  public:
    ArrayValue(const std::string& val, const BaseValue::ShapeType& sh, const ValueDtype dt): BaseArrayValue<std::string>(val,sh,dt) {};
    ArrayValue(const std::vector<std::string>&  arr, const BaseValue::ShapeType& sh, const ValueDtype dt): BaseArrayValue<std::string>(arr,sh,dt) {};
    ArrayValue(const std::string& val, const BaseValue::ShapeType& sh): ArrayValue(val,sh,ValueDtype::String) {};
    ArrayValue(const std::vector<std::string>&  arr, const BaseValue::ShapeType& sh): ArrayValue(arr,sh,ValueDtype::String) {};
  private:
    void value_to_string(std::ostringstream& oss, size_t& offset, int precision=0) const override {
      oss << "'" << value[offset] << "'";
    }
  public:
    std::string to_string(int precision=0) const override {
      size_t offset = 0;
      if (precision==0) {
	return to_string_dim(offset);
      } else {
	throw std::runtime_error("String value does not support precision parameter for to_string() method.");
      }
    };
    BaseValue::PointerType clone() const override {
      return std::make_unique<ArrayValue<std::string>>(this->value, this->shape, this->dtype);
    };
  };

  template <>
  class ArrayValue<bool>: public BaseArrayValue<bool> {
  public:
    ArrayValue(const bool& val, const BaseValue::ShapeType& sh, const ValueDtype dt): BaseArrayValue<bool>(val,sh,dt) {};
    ArrayValue(const std::vector<bool>&  arr, const BaseValue::ShapeType& sh, const ValueDtype dt): BaseArrayValue<bool>(arr,sh,dt) {};
    ArrayValue(const bool& val, const BaseValue::ShapeType& sh): ArrayValue(val,sh,ValueDtype::Boolean) {};
    ArrayValue(const std::vector<bool>&  arr, const BaseValue::ShapeType& sh): ArrayValue(arr,sh,ValueDtype::Boolean) {};
  private:
    void value_to_string(std::ostringstream& oss, size_t& offset, int precision=0) const override {
      if (value[offset])
	oss << KEYWORD_TRUE;
      else
	oss << KEYWORD_FALSE;
    }
  public:
    std::string to_string(int precision=0) const override {
      size_t offset = 0;
      if (precision==0) {
	return to_string_dim(offset);
      } else {
	throw std::runtime_error("Boolean value does not support precision parameter for to_string() method.");
      }
    };
    BaseValue::PointerType clone() const override {
      return std::make_unique<ArrayValue<bool>>(this->value, this->shape, this->dtype);
    };
  };

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
  BaseValue::PointerType create_array_value(const std::vector<T>&  arr, BaseValue::ShapeType sh={}) {
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
