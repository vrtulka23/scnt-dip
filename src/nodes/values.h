#ifndef DIP_VALUES_H
#define DIP_VALUES_H

namespace dip {

  class BaseValue {
  public:
    typedef std::unique_ptr<BaseValue> PointerType;
    enum ValueDtype { // TODO: INTX and FLOATX should support arbitrary precision types
      VALUE_BOOL,    VALUE_STRING,
      VALUE_INT16,   VALUE_INT32,   VALUE_INT64,    VALUE_INTX,   
      VALUE_UINT16,  VALUE_UINT32,  VALUE_UINT64,  
      VALUE_FLOAT32, VALUE_FLOAT64, VALUE_FLOAT128, VALUE_FLOATX
    };
    ValueDtype dtype;
    BaseValue(ValueDtype dt): dtype(dt) {};
    virtual ~BaseValue() = default;
    virtual void print() = 0;
    virtual std::string to_string(const int precision=0) = 0;
    virtual bool equals_to(const BaseValue* other) const = 0;
    virtual std::vector<int> dimension() const = 0;
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
  protected:
    virtual void value_to_string(std::ostringstream& oss, int precision=0) = 0;
  public:
    std::string to_string(const int precision=0) override {
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
    virtual std::vector<int> dimension() const override {
      return {1};
    };
  };
  
  template <typename T>
  class ScalarValue: public BaseScalarValue<T> {
  public:
    static BaseValue::PointerType create(T value) {
      if constexpr (std::is_same_v<T, short>)
	return std::make_unique<ScalarValue<short>>(ScalarValue<short>(value, BaseValue::VALUE_INT16));
      else if constexpr (std::is_same_v<T, unsigned short>)
	return std::make_unique<ScalarValue<unsigned short>>(ScalarValue<unsigned short>(value, BaseValue::VALUE_UINT16));
      else if constexpr (std::is_same_v<T, int>)
	return std::make_unique<ScalarValue<int>>(ScalarValue<int>(value, BaseValue::VALUE_INT32));
      else if constexpr (std::is_same_v<T, unsigned int>)
	return std::make_unique<ScalarValue<unsigned int>>(ScalarValue<unsigned int>(value, BaseValue::VALUE_UINT32));
      else if constexpr (std::is_same_v<T, long long>)
	return std::make_unique<ScalarValue<long long>>(ScalarValue<long long>(value, BaseValue::VALUE_INT64));
      else if constexpr (std::is_same_v<T, unsigned long long>)
	return std::make_unique<ScalarValue<unsigned long long>>(ScalarValue<unsigned long long>(value, BaseValue::VALUE_UINT64));
      else if constexpr (std::is_same_v<T, float>)
	return std::make_unique<ScalarValue<float>>(ScalarValue<float>(value, BaseValue::VALUE_FLOAT32));
      else if constexpr (std::is_same_v<T, double>)
	return std::make_unique<ScalarValue<double>>(ScalarValue<double>(value, BaseValue::VALUE_FLOAT64));
      else if constexpr (std::is_same_v<T, long double>)
	return std::make_unique<ScalarValue<long double>>(ScalarValue<long double>(value, BaseValue::VALUE_FLOAT128));
      else {
	static_assert(std::is_integral_v<T>, "Unsupported ScalarValue data type");
        return nullptr;
      }
    };
    ScalarValue(const T& val, const BaseValue::ValueDtype dt): BaseScalarValue<T>(val, dt) {};
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
    static BaseValue::PointerType create(std::string value) {
      return std::make_unique<ScalarValue<std::string>>(ScalarValue<std::string>(value));
    };
    ScalarValue(const std::string& val, const BaseValue::ValueDtype dt) : BaseScalarValue(val,dt) {};
    ScalarValue(const std::string& val): ScalarValue(val,BaseValue::VALUE_STRING) {};
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
    static BaseValue::PointerType create(bool value) {
      return std::make_unique<ScalarValue<bool>>(ScalarValue<bool>(value));
    };
    ScalarValue(const bool& val, const BaseValue::ValueDtype dt) : BaseScalarValue(val,dt) {};
    ScalarValue(const bool& val): ScalarValue(val,BaseValue::VALUE_BOOL) {};
  private:
    void value_to_string(std::ostringstream& oss, int precision=0) {    
      if (precision==0)
	if (value)
	  oss << KEYWORD_TRUE;
	else
	  oss << KEYWORD_FALSE;
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
    BaseArrayValue(const T& val, const std::vector<int>& sh, const ValueDtype dt): value({val}), shape(sh), BaseValue(dt) {};
    BaseArrayValue(const std::vector<T>&  arr, const std::vector<int>& sh, const ValueDtype dt): value(arr), shape(sh), BaseValue(dt) {};
    void print() override {std::cout << to_string() << std::endl;};
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
    std::string to_string(const int precision=0) override {
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
    virtual std::vector<int> dimension() const override {
      return shape;
    };
  };
  
  template <typename T>
  class ArrayValue: public BaseArrayValue<T> {
  public:
    static BaseValue::PointerType create(const std::vector<T>&  arr, std::vector<int> sh={}) {
      if (sh.empty())
	sh.push_back(arr.size());
      if constexpr (std::is_same_v<T, short>)
	return std::make_unique<ArrayValue<short>>(ArrayValue<short>(arr, sh, BaseValue::VALUE_INT16));
      else if constexpr (std::is_same_v<T, unsigned short>)
	return std::make_unique<ArrayValue<unsigned short>>(ArrayValue<unsigned short>(arr, sh, BaseValue::VALUE_UINT16));
      else if constexpr (std::is_same_v<T, int>)
	return std::make_unique<ArrayValue<int>>(ArrayValue<int>(arr, sh, BaseValue::VALUE_INT32));
      else if constexpr (std::is_same_v<T, unsigned int>)
	return std::make_unique<ArrayValue<unsigned int>>(ArrayValue<unsigned int>(arr, sh, BaseValue::VALUE_UINT32));
      else if constexpr (std::is_same_v<T, long long>)
	return std::make_unique<ArrayValue<long long>>(ArrayValue<long long>(arr, sh, BaseValue::VALUE_INT64));
      else if constexpr (std::is_same_v<T, unsigned long long>)
	return std::make_unique<ArrayValue<unsigned long long>>(ArrayValue<unsigned long long>(arr, sh, BaseValue::VALUE_UINT64));
      else if constexpr (std::is_same_v<T, float>)
	return std::make_unique<ArrayValue<float>>(ArrayValue<float>(arr, sh, BaseValue::VALUE_FLOAT32));
      else if constexpr (std::is_same_v<T, double>)
	return std::make_unique<ArrayValue<double>>(ArrayValue<double>(arr, sh, BaseValue::VALUE_FLOAT64));
      else if constexpr (std::is_same_v<T, long double>)
	return std::make_unique<ArrayValue<long double>>(ArrayValue<long double>(arr, sh, BaseValue::VALUE_FLOAT128));
      else {
	static_assert(std::is_integral_v<T>, "Unsupported ArrayValue data type");
        return nullptr;
      }
    };
    ArrayValue(const T& val, const std::vector<int>& sh, const BaseValue::ValueDtype dt): BaseArrayValue<T>(val,sh,dt) {};
    ArrayValue(const std::vector<T>&  arr, const std::vector<int>& sh, const BaseValue::ValueDtype dt): BaseArrayValue<T>(arr,sh,dt) {};
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
    static BaseValue::PointerType create(const std::vector<std::string>&  arr, std::vector<int> sh={}) {
      if (sh.empty())
	sh.push_back(arr.size());
      return std::make_unique<ArrayValue<std::string>>(ArrayValue<std::string>(arr, sh));
    };
    ArrayValue(const std::string& val, const std::vector<int>& sh, const BaseValue::ValueDtype dt): BaseArrayValue<std::string>(val,sh,dt) {};
    ArrayValue(const std::vector<std::string>&  arr, const std::vector<int>& sh, const BaseValue::ValueDtype dt): BaseArrayValue<std::string>(arr,sh,dt) {};
    ArrayValue(const std::string& val, const std::vector<int>& sh): ArrayValue(val,sh,BaseValue::VALUE_STRING) {};
    ArrayValue(const std::vector<std::string>&  arr, const std::vector<int>& sh): ArrayValue(arr,sh,BaseValue::VALUE_STRING) {};
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
    static BaseValue::PointerType create(const std::vector<bool>&  arr, std::vector<int> sh={}) {
      if (sh.empty())
	sh.push_back(arr.size());
      return std::make_unique<ArrayValue<bool>>(ArrayValue<bool>(arr, sh));
    };
    ArrayValue(const bool& val, const std::vector<int>& sh, const BaseValue::ValueDtype dt): BaseArrayValue<bool>(val,sh,dt) {};
    ArrayValue(const std::vector<bool>&  arr, const std::vector<int>& sh, const BaseValue::ValueDtype dt): BaseArrayValue<bool>(arr,sh,dt) {};
    ArrayValue(const bool& val, const std::vector<int>& sh): ArrayValue(val,sh,BaseValue::VALUE_BOOL) {};
    ArrayValue(const std::vector<bool>&  arr, const std::vector<int>& sh): ArrayValue(arr,sh,BaseValue::VALUE_BOOL) {};
  private:
    void value_to_string(std::ostringstream& oss, size_t& offset, int precision=0) {
      if (value[offset])
	oss << KEYWORD_TRUE;
      else
	oss << KEYWORD_FALSE;
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
