#include <gtest/gtest.h>

#include <iostream>
#include <limits>

#include "../src/dip.h"
#include "../src/environment.h"
#include "../src/nodes/nodes.h"

TEST(Properties, Constant) {
  
  dip::DIP d;    
  d.add_string("foo bool = true");
  d.add_string(" !constant");
  dip::Environment env = d.parse();
  EXPECT_EQ(env.nodes.size(), 1);  // constant declaration is not returned as a separate node
  
  std::shared_ptr<dip::ValueNode> vnode = std::dynamic_pointer_cast<dip::ValueNode>(env.nodes[0]);
  EXPECT_TRUE(vnode);
  EXPECT_EQ(vnode->constant, true); // foo node is set as a constant

  // Throw an error if one is trying to override a constant node
  d = dip::DIP();
  d.add_string("foo bool = true");
  d.add_string("  !constant");
  d.add_string("foo bool = false");
  EXPECT_THROW(d.parse(), std::runtime_error);

  // Throw an error if indent is not higher
  d = dip::DIP();
  d.add_string("  foo bool = true");
  d.add_string("!constant");
  EXPECT_THROW(d.parse(), std::runtime_error);
  
}

TEST(Properties, Description) {
  
  dip::DIP d;    
  d.add_string("foo bool = true");
  d.add_string("  !description 'If foo is true, bar is false'");
  dip::Environment env = d.parse();
  EXPECT_EQ(env.nodes.size(), 1);  // description is not returned as a separate node
  
  std::shared_ptr<dip::ValueNode> vnode = std::dynamic_pointer_cast<dip::ValueNode>(env.nodes[0]);
  EXPECT_TRUE(vnode);
  EXPECT_EQ(vnode->description, "If foo is true, bar is false");

  // Throw an error if trying to assign description to unsuported node
  // TODO: The test below works only because the group node is the first node in the list.
  //       If there are value nodes before the group node, the description will be assigned
  //       to the first previous value node. This is because description nodes
  //       currently don't see the group nodes in a node list.
  //       This needs to be fixed!
  d = dip::DIP();
  d.add_string("foo");
  d.add_string("  !description 'This is a group node'");
  d.add_string("bar int = 3");
  EXPECT_THROW(d.parse(), std::runtime_error);
  
  // Throw an error if indent is not higher
  d = dip::DIP();
  d.add_string("  foo bool = true");
  d.add_string("!description 'If foo is true, bar is false'");
  EXPECT_THROW(d.parse(), std::runtime_error);
  
}

TEST(Properties, Format) {
  
  dip::DIP d;    
  d.add_string("foo str = 'bar'");
  d.add_string("  !format '[a-z]+'");
  dip::Environment env = d.parse();
  EXPECT_EQ(env.nodes.size(), 1);  // format is not returned as a separate node
  
  std::shared_ptr<dip::ValueNode> vnode = std::dynamic_pointer_cast<dip::ValueNode>(env.nodes[0]);
  EXPECT_TRUE(vnode);
  EXPECT_EQ(vnode->format, "[a-z]+");
  
  // Throw an error if node value does not match expected format
  d = dip::DIP();
  d.add_string("foo str = 'sdf34'");
  d.add_string("  !format '[a-z]+'");
  EXPECT_THROW(d.parse(), std::runtime_error);
  
  // Throw an error if indent is not higher
  d = dip::DIP();
  d.add_string("  foo str = 'bar'");
  d.add_string("!format '[a-z]+'");
  EXPECT_THROW(d.parse(), std::runtime_error);
  
}

TEST(Properties, Tags) {
  
  dip::DIP d;    
  d.add_string("foo str = 'bar'");
  d.add_string("  !tags ['buzz','word']");
  dip::Environment env = d.parse();
  EXPECT_EQ(env.nodes.size(), 1);  // tags is not returned as a separate node
  
  std::shared_ptr<dip::ValueNode> vnode = std::dynamic_pointer_cast<dip::ValueNode>(env.nodes[0]);
  EXPECT_TRUE(vnode);
  EXPECT_EQ(vnode->tags[0], "buzz");
  EXPECT_EQ(vnode->tags[1], "word");
    
  // Throw an error if indent is not higher
  d = dip::DIP();
  d.add_string("  foo str = 'bar'");
  d.add_string("!tags '[a-z]+'");
  EXPECT_THROW(d.parse(), std::runtime_error);
  
}

TEST(Properties, OptionsBolean) {

  // boolean nodes cannot have options
  dip::DIP d;    
  d.add_string("foo bool = true");
  d.add_string("  !options [false,true]");
  EXPECT_THROW(d.parse(), std::runtime_error);

}

TEST(Properties, OptionsInteger) {
  
  dip::DIP d;    
  d.add_string("foo int = 32");
  d.add_string("  !options [16,32,64]");
  dip::Environment env = d.parse();
  EXPECT_EQ(env.nodes.size(), 1);  // tags is not returned as a separate node

  std::shared_ptr<dip::ValueNode> vnode = std::dynamic_pointer_cast<dip::ValueNode>(env.nodes[0]);
  EXPECT_TRUE(vnode);
  EXPECT_EQ(vnode->options[0].value->to_string(), "16");
  EXPECT_EQ(vnode->options[1].value->to_string(), "32");
  EXPECT_EQ(vnode->options[2].value->to_string(), "64");

  // TODO: implement unit conversion of units

  // validate if node value is in options
  d = dip::DIP();
  d.add_string("foo int = 1");
  d.add_string("  !options [16,32,64]");
  EXPECT_THROW(d.parse(), std::runtime_error);
  
}

TEST(Properties, OptionsFloat) {
  
  dip::DIP d;    
  d.add_string("foo float = 2.34");
  d.add_string("  !options [1,2.34,5.6e7]");
  dip::Environment env = d.parse();
  EXPECT_EQ(env.nodes.size(), 1);  // tags is not returned as a separate node

  std::shared_ptr<dip::ValueNode> vnode = std::dynamic_pointer_cast<dip::ValueNode>(env.nodes[0]);
  EXPECT_TRUE(vnode);
  EXPECT_EQ(vnode->options[0].value->to_string(), "1.0000");
  EXPECT_EQ(vnode->options[1].value->to_string(), "2.3400");
  EXPECT_EQ(vnode->options[2].value->to_string(), "5.6000e+07");
  
  // TODO: implement unit conversion of units
  
  // validate if node value is in options
  d = dip::DIP();
  d.add_string("foo float = 2");
  d.add_string("  !options [1,2.34,5.6e7]");
  EXPECT_THROW(d.parse(), std::runtime_error);
  
}

TEST(Properties, OptionsString) {
  
  dip::DIP d;    
  d.add_string("foo str = 'bar'");
  d.add_string("  !options ['bar','snap','pow']");
  dip::Environment env = d.parse();
  EXPECT_EQ(env.nodes.size(), 1);  // tags is not returned as a separate node

  std::shared_ptr<dip::ValueNode> vnode = std::dynamic_pointer_cast<dip::ValueNode>(env.nodes[0]);
  EXPECT_TRUE(vnode);
  EXPECT_EQ(vnode->options[0].value->to_string(), "bar");
  EXPECT_EQ(vnode->options[1].value->to_string(), "snap");
  EXPECT_EQ(vnode->options[2].value->to_string(), "pow");
  
  // validate if node value is in options
  d = dip::DIP();
  d.add_string("foo str = 'buzz'");
  d.add_string("  !options ['bar','snap','pow']");
  EXPECT_THROW(d.parse(), std::runtime_error);
  
}
