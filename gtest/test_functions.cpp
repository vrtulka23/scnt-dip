#include <gtest/gtest.h>

#include "../src/dip.h"
#include "../src/environment.h"
#include "../src/nodes/nodes.h"

dip::FunctionList::FunctionReturnType foo_boolean(const dip::Environment& env) {
  dip::ScalarValue<bool> value(false);
  return std::make_unique<dip::ScalarValue<bool>>(value);
}

dip::FunctionList::FunctionReturnType foo_integer(const dip::Environment& env) {
  dip::ScalarValue<int> value(1, dip::BaseValue::VALUE_INT32);
  return std::make_unique<dip::ScalarValue<int>>(value);
}

dip::FunctionList::FunctionReturnType foo_double(const dip::Environment& env) {
  dip::ScalarValue<double> value(1.234, dip::BaseValue::VALUE_FLOAT64);
  return std::make_unique<dip::ScalarValue<double>>(value);
}

dip::FunctionList::FunctionReturnType foo_string(const dip::Environment& env) {
  dip::ScalarValue<std::string> value("string");
  return std::make_unique<dip::ScalarValue<std::string>>(value);
}

TEST(Functions, BooleanValues) {

  dip::DIP d;
  d.add_function("foo", foo_boolean);
  d.add_string("bar bool = (foo)");
  dip::Environment env = d.parse();
  EXPECT_EQ(env.nodes.size(), 1);
  
  std::shared_ptr<dip::BaseNode> node = env.nodes[0];
  EXPECT_EQ(node->name, "bar");
  
  std::shared_ptr<dip::ValueNode> vnode = std::dynamic_pointer_cast<dip::ValueNode>(env.nodes[0]);
  EXPECT_TRUE(vnode);
  EXPECT_EQ(vnode->value->to_string(), "false");

  // returning incompatible value
  d = dip::DIP();
  d.add_function("foo", foo_string);
  d.add_string("bar bool = (foo)");
  try {
    d.parse();
    FAIL() << "Expected std::runtime_error";
  } catch (const std::runtime_error& e) {
    EXPECT_STREQ(e.what(), "Value set to a boolean node must have data type ID=0. Current data type ID is: 1");
  } catch (...) {
    FAIL() << "Expected std::runtime_error";
  }

}

TEST(Functions, StringValues) {

  dip::DIP d;
  d.add_function("foo", foo_string);
  d.add_string("bar str = (foo)");
  dip::Environment env = d.parse();
  EXPECT_EQ(env.nodes.size(), 1);
  
  std::shared_ptr<dip::BaseNode> node = env.nodes[0];
  EXPECT_EQ(node->name, "bar");
  
  std::shared_ptr<dip::ValueNode> vnode = std::dynamic_pointer_cast<dip::ValueNode>(env.nodes[0]);
  EXPECT_TRUE(vnode);
  EXPECT_EQ(vnode->value->to_string(), "string");

  // returning incompatible value
  d = dip::DIP();
  d.add_function("foo", foo_boolean);
  d.add_string("bar str = (foo)");
  try {
    d.parse();
    FAIL() << "Expected std::runtime_error";
  } catch (const std::runtime_error& e) {
    EXPECT_STREQ(e.what(), "Value set to a string node must have data type ID=1. Current data type ID is: 0");
  } catch (...) {
    FAIL() << "Expected std::runtime_error";
  }

}
