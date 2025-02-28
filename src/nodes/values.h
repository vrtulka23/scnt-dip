#ifndef DIP_VALUES_H
#define DIP_VALUES_H

namespace dip {

  class BaseValue {
  public:
    enum DataType {
      BOOL, STRING,
      INT16, INT32, INT64,
      UINT16, UINT32, UINT64,
      FLOAT32, FLOAT64, FLOAT128
    };
    DataType dtype;
    BaseValue(DataType dt): dtype(dt) {};
    virtual ~BaseValue() = default;
    virtual void print() = 0;
    virtual std::string to_string(int precision=0) = 0;
  };

  // Scalar values
  
  template <typename T>
  class BaseScalarValue: public BaseValue {
  protected:
    T value;
  public:
    BaseScalarValue(const T& val, const DataType dt): value(val), BaseValue(dt) {};
    void print() {std::cout << value << std::endl;};
    T get_value() {return value;};
  protected:
    virtual void value_to_string(std::ostringstream& oss, int precision=0) = 0;
  public:
    std::string to_string(int precision=0) {
      std::ostringstream oss;
      value_to_string(oss, precision);
      return oss.str();
    };
  };
  
  template <typename T>
  class ScalarValue: public BaseScalarValue<T> {
  public:
    ScalarValue(const T& val, const BaseValue::DataType dt): BaseScalarValue<T>(val, dt) {};
  protected:
    void value_to_string(std::ostringstream& oss, int precision=0) {
      if (precision==0) precision=DISPLAY_FLOAT_PRECISION;
      int exponent = static_cast<int>(std::log10(std::fabs(this->value)));
      if (exponent > 3 || exponent < -3) {
        oss << std::scientific << std::setprecision(precision) << this->value;
      } else {
        oss << std::fixed << std::setprecision(precision-exponent) << this->value;
      }
    };
  };
  
  template <>
  class ScalarValue<std::string> : public BaseScalarValue<std::string> {
  public:
    ScalarValue(const std::string& val, const BaseValue::DataType dt) : BaseScalarValue(val,dt) {};
  private:
    void value_to_string(std::ostringstream& oss, int precision=0) {    
      if (precision==0)
	oss << value;
      else
	throw std::runtime_error("String value does not support precision parameter for to_string() method.");
    };
  };

  template <>
  class ScalarValue<bool> : public BaseScalarValue<bool> {
  public:
    ScalarValue(const bool& val, const BaseValue::DataType dt) : BaseScalarValue(val,dt) {};
  private:
    void value_to_string(std::ostringstream& oss, int precision=0) {    
      if (precision==0)
	if (value)
	  oss << KWD_TRUE;
	else
	  oss << KWD_FALSE;
      else
	throw std::runtime_error("Boolean value does not support precision parameter for to_string() method.");
    };
  };

  // Array values
  
  template <typename T>
  class BaseArrayValue: public BaseValue {
  protected:
    std::vector<T> value;
    std::vector<int> shape;
  public:
    BaseArrayValue(const T& val, const std::vector<int>& sh, const DataType dt): value({val}), shape(sh), BaseValue(dt) {};
    BaseArrayValue(const std::vector<T>&  arr, const std::vector<int>& sh, const DataType dt): value(arr), shape(sh), BaseValue(dt) {};
    void print() {std::cout << to_string() << std::endl;};
    std::vector<T> get_value() {return value;};
    std::vector<int> get_shape() {return shape;};
  protected:
    virtual void value_to_string(std::ostringstream& oss, size_t& offset, int precision=0) = 0;
    std::string to_string_dim(size_t& offset, const int& precision=0, int dim=0) {
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
    std::string to_string(int precision=0) {
      size_t offset = 0;
      if (precision==0) {
	return to_string_dim(offset);
      } else {
	return to_string_dim(offset, precision);
      }
    };
  };
  
  template <typename T>
  class ArrayValue: public BaseArrayValue<T> {
  public:
    ArrayValue(const T& val, const std::vector<int>& sh, const BaseValue::DataType dt): BaseArrayValue<T>(val,sh,dt) {};
    ArrayValue(const std::vector<T>&  arr, const std::vector<int>& sh, const BaseValue::DataType dt): BaseArrayValue<T>(arr,sh,dt) {};
  private:
    void value_to_string(std::ostringstream& oss, size_t& offset, int precision=0) {
      if (precision==0) precision=DISPLAY_FLOAT_PRECISION;
      int exponent = static_cast<int>(std::log10(std::fabs(this->value[offset])));
      if (exponent > 3 || exponent < -3) {
        oss << std::scientific << std::setprecision(precision) << this->value[offset];
      } else {
        oss << std::fixed << std::setprecision(precision-exponent) << this->value[offset];
      }
    }
  };
  
  template <>
  class ArrayValue<std::string>: public BaseArrayValue<std::string> {
  public:
    ArrayValue(const std::string& val, const std::vector<int>& sh, const BaseValue::DataType dt): BaseArrayValue<std::string>(val,sh,dt) {};
    ArrayValue(const std::vector<std::string>&  arr, const std::vector<int>& sh, const BaseValue::DataType dt): BaseArrayValue<std::string>(arr,sh,dt) {};
  private:
    void value_to_string(std::ostringstream& oss, size_t& offset, int precision=0) {
      oss << "'" << value[offset] << "'";
    }
  public:
    std::string to_string(int precision=0) {
      size_t offset = 0;
      if (precision==0) {
	return to_string_dim(offset);
      } else {
	throw std::runtime_error("String value does not support precision parameter for to_string() method.");
      }
    };
  };

  template <>
  class ArrayValue<bool>: public BaseArrayValue<bool> {
  public:
    ArrayValue(const bool& val, const std::vector<int>& sh, const BaseValue::DataType dt): BaseArrayValue<bool>(val,sh,dt) {};
    ArrayValue(const std::vector<bool>&  arr, const std::vector<int>& sh, const BaseValue::DataType dt): BaseArrayValue<bool>(arr,sh,dt) {};
  private:
    void value_to_string(std::ostringstream& oss, size_t& offset, int precision=0) {
      if (value[offset])
	oss << KWD_TRUE;
      else
	oss << KWD_FALSE;
    }
  public:
    std::string to_string(int precision=0) {
      size_t offset = 0;
      if (precision==0) {
	return to_string_dim(offset);
      } else {
	throw std::runtime_error("Boolean value does not support precision parameter for to_string() method.");
      }
    };
  };

}

#endif // DIP_VALUES_H
