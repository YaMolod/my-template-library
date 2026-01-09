#include "gtest/gtest.h"
#include "MTL/String.hpp"

class StringTest : public ::testing::Test 
{
protected:
	mtl::string s1{ "hello" };
	mtl::string s2{ "world" };
};

TEST_F(StringTest, ConstructorsAndSize) {
    mtl::string empty;
    EXPECT_TRUE(empty.empty());
    EXPECT_EQ(empty.size(), 0);

    EXPECT_STREQ(s1.c_str(), "hello");
    EXPECT_EQ(s1.size(), 5);
}

TEST_F(StringTest, CopyAndMoveSemantics) {
    mtl::string copy = s1;
    EXPECT_EQ(copy, s1);
    EXPECT_NE(copy.c_str(), s1.c_str());

    mtl::string moved = std::move(copy);
    EXPECT_EQ(moved, "hello");
    EXPECT_TRUE(copy.empty());
}

TEST_F(StringTest, Modification) {
    s1 += " ";
    s1 += s2;
    EXPECT_EQ(s1, "hello world");
    EXPECT_EQ(s1.size(), 11);
}

TEST_F(StringTest, ElementAccess) {
    EXPECT_EQ(s1[0], 'h');
    s1[0] = 'H';
    EXPECT_EQ(s1[0], 'H');
}

TEST_F(StringTest, AddAndClear) {
    mtl::string str = s1 + " " + s2;
    EXPECT_EQ(str, "hello world");
    EXPECT_EQ(str.size(), 11);
    str.clear();
    EXPECT_EQ(str[0], '\0');
    EXPECT_EQ(str.size(), 0);
    EXPECT_TRUE(str.empty());
}

TEST(StringMemory, Reallocation) {
    mtl::string long_str;
    for (int i = 0; i < 1000; ++i) 
    {
        long_str += "a";
    }
    EXPECT_EQ(long_str.size(), 1000);
}


TEST(StringComparison, Operators) {
    mtl::string a = "apple";
    mtl::string b = "banana";

    EXPECT_TRUE(a < b);
    EXPECT_TRUE(a != b);
    EXPECT_FALSE(a == b);
}