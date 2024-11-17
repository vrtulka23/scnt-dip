#include <gtest/gtest.h>

#include "../src/dip.h"

TEST(DIP, Initialization) {

  dip::DIP d;    
  EXPECT_EQ(d.to_string(), "DIP");  
  
}

