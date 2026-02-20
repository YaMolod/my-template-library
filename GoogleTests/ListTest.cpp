#include <gtest/gtest.h>
#include "MTL/List.hpp"

class ListTest : public ::testing::Test {
protected:
    mtl::list<int> empty_list;
    mtl::list<int> small_list;

    void SetUp() override {
        small_list.push_back(10);
        small_list.push_back(20);
        small_list.push_back(30);
    }
};

TEST_F(ListTest, DefaultConstructor) {
    EXPECT_TRUE(empty_list.empty());
    EXPECT_EQ(empty_list.size(), 0);
}

TEST_F(ListTest, CopyConstructor) {
    mtl::list<int> copied_list(small_list);
    EXPECT_EQ(copied_list.size(), small_list.size());
    EXPECT_EQ(copied_list.front(), small_list.front());

    copied_list.push_back(40);
    EXPECT_NE(copied_list.size(), small_list.size());
}

TEST_F(ListTest, PushBack) {
    empty_list.push_back(100);
    EXPECT_EQ(empty_list.size(), 1);
    EXPECT_EQ(empty_list.back(), 100);
}

TEST_F(ListTest, PushFront) {
    small_list.push_front(5);
    EXPECT_EQ(small_list.front(), 5);
    EXPECT_EQ(small_list.size(), 4);
}

TEST_F(ListTest, AccessFrontAndBack) {
    EXPECT_EQ(small_list.front(), 10);
    EXPECT_EQ(small_list.back(), 30);
}

TEST_F(ListTest, PopBack) {
    small_list.pop_back();
    EXPECT_EQ(small_list.size(), 2);
    EXPECT_EQ(small_list.back(), 20);
}

TEST_F(ListTest, Clear) {
    small_list.clear();
    EXPECT_TRUE(small_list.empty());
    EXPECT_EQ(small_list.size(), 0);
}

TEST_F(ListTest, Iterator) {
    int expected[] = { 10, 20, 30 };
    int i = 0;
    for (auto it = small_list.begin(); it != small_list.end(); ++it) {
        EXPECT_EQ(*it, expected[i++]);
    }
}
