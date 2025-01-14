#include <gtest/gtest.h>

#include "../src/dip.h"

TEST(DIP, Initialization) {

  dip::DIP d;    
  EXPECT_EQ(d.to_string(), "DIP");

  std::string code = "foo str = \"bar\"\ncount int = 3\nnew bool = true";
  d.add_string(code);
  
}

TEST(DIP, Parsing) {

  dip::DIP d;    
  std::string code = "foo str = \"bar\"\n  count int = 3\n # stuff\n   my_group";
  d.add_string(code);
  d.parse();

}

TEST(DIP, ParseBoolean) {
  
  dip::DIP d;    
  std::string code = "foo bool[2:,3] = true";
  d.add_string(code);
  d.parse();
  
}

TEST(DIP, ParseString) {
  
  dip::DIP d;    
  std::string code = "foo str[2:,3] = \"bar\"";
  d.add_string(code);
  d.parse();
  
}
