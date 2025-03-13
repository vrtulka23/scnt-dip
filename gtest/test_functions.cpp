#include <gtest/gtest.h>

#include "../src/dip.h"
#include "../src/environment.h"
#include "../src/nodes/nodes.h"

dip::BaseValue::PointerType get_scalar_boolean(const dip::Environment& env) {
  return dip::ScalarValue<bool>::create(false);
}
dip::BaseValue::PointerType get_scalar_integer(const dip::Environment& env) {
  return dip::ScalarValue<int>::create(2);
}
dip::BaseValue::PointerType get_scalar_double(const dip::Environment& env) {
  return dip::ScalarValue<double>::create(2.34e5);
}
dip::BaseValue::PointerType get_scalar_string(const dip::Environment& env) {
  return dip::ScalarValue<std::string>::create("string");
}

dip::BaseValue::PointerType get_array_boolean(const dip::Environment& env) {
  return dip::ArrayValue<bool>::create({false,true,false});
}
dip::BaseValue::PointerType get_array_integer(const dip::Environment& env) {
  return dip::ArrayValue<int>::create({2,3,4,5},{2,2});
}
dip::BaseValue::PointerType get_array_double(const dip::Environment& env) {
  return dip::ArrayValue<double>::create({2.34e5,3.45e6,4.56e7});
}
dip::BaseValue::PointerType get_array_string(const dip::Environment& env) {
  return dip::ArrayValue<std::string>::create({"foo","bar","baz"});
}

TEST(Functions, BooleanValues) {

  // define scalar
  dip::DIP d;
  d.add_value_function("scalar_value", get_scalar_boolean);
  d.add_value_function("array_value", get_array_boolean);
  d.add_string("foo bool = (scalar_value)");
  d.add_string("bar bool[3] = (array_value)");
  dip::Environment env = d.parse();
  EXPECT_EQ(env.nodes.size(), 2);
  
  std::shared_ptr<dip::BaseNode> node = env.nodes[0];
  EXPECT_EQ(node->name, "foo");
  std::shared_ptr<dip::ValueNode> vnode = std::dynamic_pointer_cast<dip::ValueNode>(node);
  EXPECT_TRUE(vnode);
  EXPECT_EQ(vnode->value->to_string(), "false");

  node = env.nodes[1];
  EXPECT_EQ(node->name, "bar");
  vnode = std::dynamic_pointer_cast<dip::ValueNode>(env.nodes[1]);
  EXPECT_TRUE(vnode);
  EXPECT_EQ(vnode->value->to_string(), "[false, true, false]");

  // returning incompatible value
  d = dip::DIP();
  d.add_value_function("foo", get_scalar_string);
  d.add_string("bar bool = (foo)");
  try {
    d.parse();
    FAIL() << "Expected std::runtime_error";
  } catch (const std::runtime_error& e) {
    EXPECT_STREQ(e.what(), "Value data type has ID=0, but node data type has ID=1");
  } catch (...) {
    FAIL() << "Expected std::runtime_error";
  }

}

TEST(Functions, IntegerValues) {

  // define scalar
  dip::DIP d;
  d.add_value_function("scalar_value", get_scalar_integer);
  d.add_value_function("array_value", get_array_integer);
  d.add_string("foo int = (scalar_value)");
  d.add_string("bar int[2,2] = (array_value)");
  dip::Environment env = d.parse();
  EXPECT_EQ(env.nodes.size(), 2);

  std::shared_ptr<dip::BaseNode> node = env.nodes[0];
  EXPECT_EQ(node->name, "foo");
  std::shared_ptr<dip::ValueNode> vnode = std::dynamic_pointer_cast<dip::ValueNode>(env.nodes[0]);
  EXPECT_TRUE(vnode);
  EXPECT_EQ(vnode->value->to_string(), "2");

  node = env.nodes[1];
  EXPECT_EQ(node->name, "bar");
  vnode = std::dynamic_pointer_cast<dip::ValueNode>(env.nodes[1]);
  EXPECT_TRUE(vnode);
  EXPECT_EQ(vnode->value->to_string(), "[[2, 3], [4, 5]]");
  
  // returning incompatible value
  d = dip::DIP();
  d.add_value_function("foo", get_scalar_string);
  d.add_string("bar bool = (foo)");
  try {
    d.parse();
    FAIL() << "Expected std::runtime_error";
  } catch (const std::runtime_error& e) {
    EXPECT_STREQ(e.what(), "Value data type has ID=0, but node data type has ID=1");
  } catch (...) {
    FAIL() << "Expected std::runtime_error";
  }
  
}

TEST(Functions, FloatValues) {

  // define scalar
  dip::DIP d;
  d.add_value_function("scalar_value", get_scalar_double);
  d.add_value_function("array_value", get_array_double);
  d.add_string("foo float = (scalar_value)");
  d.add_string("bar float[3] = (array_value)");
  dip::Environment env = d.parse();
  EXPECT_EQ(env.nodes.size(), 2);

  std::shared_ptr<dip::BaseNode> node = env.nodes[0];
  EXPECT_EQ(node->name, "foo");
  std::shared_ptr<dip::ValueNode> vnode = std::dynamic_pointer_cast<dip::ValueNode>(env.nodes[0]);
  EXPECT_TRUE(vnode);
  EXPECT_EQ(vnode->value->to_string(), "2.3400e+05");

  node = env.nodes[1];
  EXPECT_EQ(node->name, "bar");
  vnode = std::dynamic_pointer_cast<dip::ValueNode>(env.nodes[1]);
  EXPECT_TRUE(vnode);
  EXPECT_EQ(vnode->value->to_string(), "[2.3400e+05, 3.4500e+06, 4.5600e+07]");
  
  // returning incompatible value
  d = dip::DIP();
  d.add_value_function("foo", get_scalar_string);
  d.add_string("bar bool = (foo)");
  try {
    d.parse();
    FAIL() << "Expected std::runtime_error";
  } catch (const std::runtime_error& e) {
    EXPECT_STREQ(e.what(), "Value data type has ID=0, but node data type has ID=1");
  } catch (...) {
    FAIL() << "Expected std::runtime_error";
  }
  
}

TEST(Functions, StringValues) {

  // define scalar
  dip::DIP d;
  d.add_value_function("scalar_value", get_scalar_string);
  d.add_value_function("array_value", get_array_string);
  d.add_string("foo str = (scalar_value)");
  d.add_string("bar str[3] = (array_value)");
  dip::Environment env = d.parse();
  EXPECT_EQ(env.nodes.size(), 2);
  
  std::shared_ptr<dip::BaseNode> node = env.nodes[0];
  EXPECT_EQ(node->name, "foo");
  std::shared_ptr<dip::ValueNode> vnode = std::dynamic_pointer_cast<dip::ValueNode>(env.nodes[0]);
  EXPECT_TRUE(vnode);
  EXPECT_EQ(vnode->value->to_string(), "string");

  node = env.nodes[1];
  EXPECT_EQ(node->name, "bar");
  vnode = std::dynamic_pointer_cast<dip::ValueNode>(env.nodes[1]);
  EXPECT_TRUE(vnode);
  EXPECT_EQ(vnode->value->to_string(), "['foo', 'bar', 'baz']");
  
  // returning incompatible value
  d = dip::DIP();
  d.add_value_function("foo", get_scalar_boolean);
  d.add_string("bar str = (foo)");
  try {
    d.parse();
    FAIL() << "Expected std::runtime_error";
  } catch (const std::runtime_error& e) {
    EXPECT_STREQ(e.what(), "Value data type has ID=1, but node data type has ID=0");
  } catch (...) {
    FAIL() << "Expected std::runtime_error";
  }

}

dip::BaseNode::NodeListType get_scalar_table(const dip::Environment& env) {
  return {
    dip::BooleanNode::create_scalar("scalar_bool", false),
    dip::IntegerNode::create_scalar<int>("scalar_int", 1),
    dip::FloatNode::create_scalar<double>("scalar_double", 2.34e5),
    dip::StringNode::create_scalar("scalar_str", "baz_value"),
    
    dip::BooleanNode::create_array("array_bool", {false,true,false}),
    dip::IntegerNode::create_array<int>("array_int", {1,2,3,4}, {2,2}),
    dip::FloatNode::create_array<double>("array_double", {2.34e5, 3.45e6, 4.56e7}),
    dip::StringNode::create_array("array_str", {"foo","bar","baz"}),
  };
}

TEST(Functions, Tables) {

  dip::DIP d;
  d.add_table_function("foo", get_scalar_table);
  d.add_string("foo table = (foo)");
  dip::Environment env = d.parse();
  EXPECT_EQ(env.nodes.size(), 8);
  
  std::shared_ptr<dip::BaseNode> node = env.nodes[0];
  EXPECT_EQ(node->name, "foo.scalar_bool");
  std::shared_ptr<dip::ValueNode> vnode = std::dynamic_pointer_cast<dip::ValueNode>(env.nodes[0]);
  EXPECT_TRUE(vnode);
  EXPECT_EQ(vnode->value->to_string(), "false");

  node = env.nodes[1];
  EXPECT_EQ(node->name, "foo.scalar_int");
  vnode = std::dynamic_pointer_cast<dip::ValueNode>(node);
  EXPECT_TRUE(vnode);
  EXPECT_EQ(vnode->value->to_string(), "1");

  node = env.nodes[2];
  EXPECT_EQ(node->name, "foo.scalar_double");
  vnode = std::dynamic_pointer_cast<dip::ValueNode>(node);
  EXPECT_TRUE(vnode);
  EXPECT_EQ(vnode->value->to_string(), "2.3400e+05");

  node = env.nodes[3];
  EXPECT_EQ(node->name, "foo.scalar_str");
  vnode = std::dynamic_pointer_cast<dip::ValueNode>(node);
  EXPECT_TRUE(vnode);
  EXPECT_EQ(vnode->value->to_string(), "baz_value");

  node = env.nodes[4];
  EXPECT_EQ(node->name, "foo.array_bool");
  vnode = std::dynamic_pointer_cast<dip::ValueNode>(node);
  EXPECT_TRUE(vnode);
  EXPECT_EQ(vnode->value->to_string(), "[false, true, false]");

  node = env.nodes[5];
  EXPECT_EQ(node->name, "foo.array_int");
  vnode = std::dynamic_pointer_cast<dip::ValueNode>(node);
  EXPECT_TRUE(vnode);
  EXPECT_EQ(vnode->value->to_string(), "[[1, 2], [3, 4]]");

  node = env.nodes[6];
  EXPECT_EQ(node->name, "foo.array_double");
  vnode = std::dynamic_pointer_cast<dip::ValueNode>(node);
  EXPECT_TRUE(vnode);
  EXPECT_EQ(vnode->value->to_string(), "[2.3400e+05, 3.4500e+06, 4.5600e+07]");

  node = env.nodes[7];
  EXPECT_EQ(node->name, "foo.array_str");
  vnode = std::dynamic_pointer_cast<dip::ValueNode>(node);
  EXPECT_TRUE(vnode);
  EXPECT_EQ(vnode->value->to_string(), "['foo', 'bar', 'baz']");

}
