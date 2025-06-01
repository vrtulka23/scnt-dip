#ifndef DIP_VALUES_SCALAR_H
#define DIP_VALUES_SCALAR_H

#include <typeinfo>

#include "../settings.h"

namespace dip {

  // Scalar values
  
  template <typename T>
  class BaseScalarValue: public BaseValue {
  protected:
    T value;
  public:
    BaseScalarValue(const T& val, const ValueDtype dt): value(val), BaseValue(dt) {};
    void print() override {std::cout << value << std::endl;};
    T get_value() {return value;};
    Array::ShapeType get_shape() const override { return {1};};
    size_t get_size() const override {return 1;};
  protected:
    virtual void value_to_string(std::ostringstream& oss, int precision=0) const = 0;
  public:
    std::string to_string(const int precision=0) const override {
      std::ostringstream oss;
      value_to_string(oss, precision);
      return oss.str();
    };
    bool operator==(const BaseValue* other) const override {
      // TODO: this need to be modified to also handle array conversions
      if (dtype==other->dtype) {
	T other_value = static_cast<T>(*other);
	return value==other_value ? true : false;
      } else {
	throw std::runtime_error("Cannot compare equality of types '"+std::string(ValueDtypeNames[dtype])+"' and '"+std::string(ValueDtypeNames[other->dtype])+"'");
      }
    };
    bool operator<(const BaseValue* other) const override {
      if (dtype==other->dtype) {
	T other_value = static_cast<T>(*other);
	return (value<other_value) ? true : false;
      } else {
	throw std::runtime_error("Cannot compare equality of types '"+std::string(ValueDtypeNames[dtype])+"' and '"+std::string(ValueDtypeNames[other->dtype])+"'");
      }
    };
    BaseValue::PointerType slice(const Array::RangeType& slice) override {
      throw std::runtime_error("Scalar value cannot be sliced");
      return nullptr;
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
    explicit operator bool() const override {
      return static_cast<bool>(this->value);
    };
    explicit operator short() const override {
      return static_cast<short>(this->value);
    };
    explicit operator unsigned short() const override {
      return static_cast<unsigned short>(this->value);
    };
    explicit operator int() const override {
      return static_cast<int>(this->value);
    };
    explicit operator unsigned int() const override {
      return static_cast<unsigned int>(this->value);
    };
    explicit operator long long() const override {
      return static_cast<long long>(this->value);
    };
    explicit operator unsigned long long() const override {
      return static_cast<unsigned long long>(this->value);
    };
    explicit operator float() const override {
      return static_cast<float>(this->value);
    };
    explicit operator double() const override {
      return static_cast<double>(this->value);
    };
    explicit operator long double() const override {
      return static_cast<long double>(this->value);
    };
    explicit operator std::string() const override {
      return std::to_string(this->value);
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
    explicit operator bool() const override {
      return static_cast<bool>(value.size());
    }
    explicit operator short() const override {
      // TODO: this is a quick and dirty implementation, needs to be improved
      try {
	return std::stoi(value);
      } catch (const std::exception& e) {
	throw std::runtime_error("Cannot convert string to short: '" + value + "': " + e.what());
      }
    };
    explicit operator unsigned short() const override {
      // TODO: this is a quick and dirty implementation, needs to be improved
      try {
	return std::stoul(value);
      } catch (const std::exception& e) {
	throw std::runtime_error("Cannot convert string to unsigned short: '" + value + "': " + e.what());
      }
    };
    explicit operator int() const override {
      try {
	return std::stoi(value);
      } catch (const std::exception& e) {
	throw std::runtime_error("Cannot convert string to integer: '" + value + "': " + e.what());
      }
    };
    explicit operator unsigned int() const override {
      try {
	// TODO: this should be changed to std::from_chars
	return std::stoul(value);
      } catch (const std::exception& e) {
	throw std::runtime_error("Cannot convert string to unsigned integer: '" + value + "': " + e.what());
      }
    };
    explicit operator long long() const override {
      try {
	return std::stoll(value);
      } catch (const std::exception& e) {
	throw std::runtime_error("Cannot convert string to long long: '" + value + "': " + e.what());
      }
    };
    explicit operator unsigned long long() const override {
      try {
	return std::stoull(value);
      } catch (const std::exception& e) {
	throw std::runtime_error("Cannot convert string to unsigned long long: '" + value + "': " + e.what());
      }
    };
    explicit operator float() const override {
      try {
	return std::stof(value);
      } catch (const std::exception& e) {
	throw std::runtime_error("Cannot convert string to float: '" + value + "': " + e.what());
      }
    };
    explicit operator double() const override {
      try {
	return std::stod(value);
      } catch (const std::exception& e) {
	throw std::runtime_error("Cannot convert string to double: '" + value + "': " + e.what());
      }
    };
    explicit operator long double() const override {
      try {
	return std::stold(value);
      } catch (const std::exception& e) {
	throw std::runtime_error("Cannot convert string to long double: '" + value + "': " + e.what());
      }
    };
    explicit operator std::string() const override {
      return value;
    };
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
    explicit operator bool() const override {
      return value;
    }
    explicit operator short() const override {
      return static_cast<short>(value);
    };
    explicit operator unsigned short() const override {
      return static_cast<unsigned short>(value);
    };
    explicit operator int() const override {
      return static_cast<int>(value);
    };
    explicit operator unsigned int() const override {
      return static_cast<unsigned int>(value);
    };
    explicit operator long long() const override {
      return static_cast<long long>(value);
    };
    explicit operator unsigned long long() const override {
      return static_cast<unsigned long long>(value);
    };
    explicit operator float() const override {
      return static_cast<float>(value);
    };
    explicit operator double() const override {
      return static_cast<double>(value);
    };
    explicit operator long double() const override {
      return static_cast<long double>(value);
    };
    explicit operator std::string() const override {
      if (value)
	return std::string(KEYWORD_TRUE);
      else
	return std::string(KEYWORD_FALSE);
    };
  };

}

#endif // DIP_VALUES_SCALAR_H
