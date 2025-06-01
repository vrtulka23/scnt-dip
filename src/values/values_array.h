#ifndef DIP_VALUES_ARRAY_H
#define DIP_VALUES_ARRAY_H

#include <typeinfo>

#include "../settings.h"

namespace dip {
  
  // Array values

  template <typename T> class ArrayValue; // here we need a forward declaration
  
  template <typename T>
  class BaseArrayValue: public BaseValue {
  protected:
    std::vector<T> value;
    Array::ShapeType shape;
  public:
    BaseArrayValue(const T& val, const Array::ShapeType& sh, const ValueDtype dt): value({val}), shape(sh), BaseValue(dt) {};
    BaseArrayValue(const std::vector<T>&  arr, const Array::ShapeType& sh, const ValueDtype dt): value(arr), shape(sh), BaseValue(dt) {};
    void print() override {std::cout << to_string() << std::endl;};
    std::vector<T> get_value() {return value;};
    Array::ShapeType get_shape() const override {return shape;};
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
    bool operator==(const BaseValue* other) const override {
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
    bool operator<(const BaseValue* other) const override {
      const BaseArrayValue<T>* otherT = dynamic_cast<const BaseArrayValue<T>*>(other);
      if (otherT) {
	bool is_equal = true;
	for (int i=0; i<value.size();i++)
	  if (value[i]>=otherT->value[i])
	    is_equal = false;
	for (int i=0; i<shape.size();i++)
	  if (shape[i]>=otherT->shape[i])
	    is_equal = false;	
	return is_equal;
      } else {
	throw std::runtime_error("Could not convert BaseValue into a BaseArrayValue");
      }
    };
    BaseValue::PointerType slice_value(const Array::RangeType& slice) {
      if (slice.size()!=this->shape.size())
	throw std::runtime_error("Array slice size does not correspond with array shape: "+std::to_string(slice.size())+"!="+std::to_string(this->shape.size()));
      // calculate new shape and size
      Array::ShapeType new_shape;
      size_t new_size = 0;
      for (size_t i=0; i<this->shape.size(); i++) {
	int dmin = slice[i].dmin;
	int dmax = (slice[i].dmax==Array::max_range) ? this->shape[i]-1 : slice[i].dmax;
	int new_dim = dmax + 1 - dmin;
	if (new_dim>1) {
	  new_shape.push_back(new_dim);
	  new_size += new_dim;
	}
      }
      // allocate variables
      std::vector<T> new_value;
      new_value.reserve(new_size);
      Array::ShapeType coord(this->shape.size(),0);
      // list through all values and copy sliced
      for (size_t i=0; i<this->value.size(); i++) {
	// copy only values within the given ranges
	bool push = true;
	for (size_t dim = 0; dim<this->shape.size(); dim++) {
	  int dmin = slice[dim].dmin;
	  int dmax = (slice[dim].dmax==Array::max_range) ? this->shape[dim]-1 : slice[dim].dmax;
	  if (coord[dim]<dmin or dmax<coord[dim])
	    push = false;
	}
	if (push)
	  new_value.push_back(this->value[i]);
	// increase the coodrinates
	for (size_t dim = this->shape.size(); dim-->0;) {
	  if (++coord[dim] < this->shape[dim]) break;
	  coord[dim] = 0;  // carry over
	}
      }
      if (new_size>1)
	return std::make_unique<ArrayValue<T>>(new_value, new_shape, this->dtype);
      else
	return std::make_unique<ScalarValue<T>>(new_value[0], this->dtype);
    };
    void convert_units(const std::string& from_units, const Quantity::PointerType& to_quantity) override {
      throw std::runtime_error("Array value of type '"+std::string(ValueDtypeNames[dtype])+"' does not support unit conversion.");
    };
    void convert_units(const Quantity::PointerType& from_quantity, const std::string& to_units) override {
      throw std::runtime_error("Array value of type '"+std::string(ValueDtypeNames[dtype])+"' does not support unit conversion.");
    };
    explicit operator bool() const override {
      // TODO: Implement bool conversion of arrays
      throw std::runtime_error("Bool conversion of arrays is not implemented!!!");
    };
    explicit operator short() const override {
      throw std::runtime_error("Short conversion of arrays is not implemented!!!");
    };
    explicit operator unsigned short() const override {
      throw std::runtime_error("Unsigned short conversion of arrays is not implemented!!!");
    };
    explicit operator int() const override {
      throw std::runtime_error("Integer conversion of arrays is not implemented!!!");
    };
    explicit operator unsigned int() const override {
      throw std::runtime_error("Unsigned integer conversion of arrays is not implemented!!!");
    };
    explicit operator long long() const override {
      throw std::runtime_error("Long long conversion of arrays is not implemented!!!");
    };
    explicit operator unsigned long long() const override {
      throw std::runtime_error("Unsigned long long conversion of arrays is not implemented!!!");
    };
    explicit operator float() const override {
      throw std::runtime_error("Float conversion of arrays is not implemented!!!");
    };
    explicit operator double() const override {
      throw std::runtime_error("Double conversion of arrays is not implemented!!!");
    };
    explicit operator long double() const override {
      throw std::runtime_error("Double conversion of arrays is not implemented!!!");
    };
    explicit operator std::string() const override {
      throw std::runtime_error("Double conversion of arrays is not implemented!!!");
    };
  };
  
  template <typename T>
  class ArrayValue: public BaseArrayValue<T> {
  public:
    ArrayValue(const T& val, const Array::ShapeType& sh, const ValueDtype dt): BaseArrayValue<T>(val,sh,dt) {};
    ArrayValue(const std::vector<T>&  arr, const Array::ShapeType& sh, const ValueDtype dt): BaseArrayValue<T>(arr,sh,dt) {};
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
    BaseValue::PointerType slice(const Array::RangeType& slice) override {
      return this->slice_value(slice);
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
    ArrayValue(const std::string& val, const Array::ShapeType& sh, const ValueDtype dt): BaseArrayValue<std::string>(val,sh,dt) {};
    ArrayValue(const Array::StringType&  arr, const Array::ShapeType& sh, const ValueDtype dt): BaseArrayValue<std::string>(arr,sh,dt) {};
    ArrayValue(const std::string& val, const Array::ShapeType& sh): ArrayValue(val,sh,ValueDtype::String) {};
    ArrayValue(const Array::StringType&  arr, const Array::ShapeType& sh): ArrayValue(arr,sh,ValueDtype::String) {};
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
    BaseValue::PointerType slice(const Array::RangeType& slice) override {
      return this->slice_value(slice);
    };
  };

  template <>
  class ArrayValue<bool>: public BaseArrayValue<bool> {
  public:
    ArrayValue(const bool& val, const Array::ShapeType& sh, const ValueDtype dt): BaseArrayValue<bool>(val,sh,dt) {};
    ArrayValue(const std::vector<bool>&  arr, const Array::ShapeType& sh, const ValueDtype dt): BaseArrayValue<bool>(arr,sh,dt) {};
    ArrayValue(const bool& val, const Array::ShapeType& sh): ArrayValue(val,sh,ValueDtype::Boolean) {};
    ArrayValue(const std::vector<bool>&  arr, const Array::ShapeType& sh): ArrayValue(arr,sh,ValueDtype::Boolean) {};
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
    BaseValue::PointerType slice(const Array::RangeType& slice) override {
      return this->slice_value(slice);
    };
  };

}

#endif // DIP_VALUES_ARRAY_H
