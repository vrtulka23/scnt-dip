#include <gtest/gtest.h>

#include <iostream>
#include <limits>

#include "../src/dip.h"
#include "../src/environment.h"
#include "../src/nodes/nodes.h"

TEST(ParseStrings, BlockQuotes) {
  
  dip::DIP d;    
  d.add_string("foo str = \"\"\"");
  d.add_string("bar1");
  d.add_string("bar2");
  d.add_string("\"\"\"");
  d.add_string("bar str = \"\"\" \"\"\"");
  dip::Environment env = d.parse();
  EXPECT_EQ(env.nodes.size(), 2);
  
  std::shared_ptr<dip::BaseNode> node = env.nodes[0];
  EXPECT_EQ(node->name, "foo");
  EXPECT_EQ(node->value_raw[0], "\nbar1\nbar2\n");
  node = env.nodes[1];
  EXPECT_EQ(node->name, "bar");
  EXPECT_EQ(node->value_raw[0], " ");  

}

TEST(ParseStrings, EscapeSymbols) {

  dip::DIP d;    
  d.add_string("foo str = \"\"\"foo \\\" foo \\\' foo \\n foo\"\"\"");
  d.add_string("bar str = \"bar \\\" bar \\\' bar \\n bar\"");
  d.add_string("baz str = 'baz \\\" baz \\\' baz \\n baz'");
  dip::Environment env = d.parse();
  EXPECT_EQ(env.nodes.size(), 3);
  
  std::shared_ptr<dip::BaseNode> node = env.nodes[0];
  EXPECT_EQ(node->name, "foo");
  EXPECT_EQ(node->value_raw[0], "foo \\\" foo \\' foo \\n foo");  
  node = env.nodes[1];
  EXPECT_EQ(node->name, "bar");
  EXPECT_EQ(node->value_raw[0], "bar \\\" bar \\' bar \\n bar");
  node = env.nodes[2];
  EXPECT_EQ(node->name, "baz");
  EXPECT_EQ(node->value_raw[0], "baz \\\" baz \\' baz \\n baz");  
    
}

TEST(ParseStrings, CommentSymbol) {

  dip::DIP d;    
  d.add_string("foo str = bar#baz # comment");
  d.add_string("bar str = \"baz#foo\" # comment");
  d.add_string("baz str[:] = [\"foo#\",\"bar\"] # comment");
  dip::Environment env = d.parse();
  EXPECT_EQ(env.nodes.size(), 3);
  
  std::shared_ptr<dip::BaseNode> node = env.nodes[0];
  EXPECT_EQ(node->name, "foo");
  EXPECT_EQ(node->value_raw[0], "bar#baz");
  node = env.nodes[1];
  EXPECT_EQ(node->name, "bar");
  EXPECT_EQ(node->value_raw[0], "baz#foo");
  node = env.nodes[2];
  EXPECT_EQ(node->name, "baz");
  EXPECT_EQ(node->value_raw, std::vector<std::string>({"foo#","bar"}));  
  
}
