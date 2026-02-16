#include <gtest/gtest.h>
#include "mtl/deque.hpp"
#include <deque>

TEST(DequeTest, DefaultConstructor) {
    mtl::deque<int> dq;
    EXPECT_TRUE(dq.empty());
    EXPECT_EQ(dq.size(), 0);
}

TEST(DequeTest, PushBackAndFront) {
    mtl::deque<int> dq;
    dq.push_back(10);
    dq.push_front(20);
    dq.push_back(30);

    EXPECT_EQ(dq.size(), 3);
    EXPECT_EQ(dq[0], 20);
    EXPECT_EQ(dq[1], 10);
    EXPECT_EQ(dq[2], 30);
}

TEST(DequeTest, ElementAccess) {
    mtl::deque<int> dq = { 1, 2, 3 };

    EXPECT_EQ(dq[1], 2);
    EXPECT_EQ(dq.front(), 1);
    EXPECT_EQ(dq.back(), 3);
}

TEST(DequeTest, PopOperations) {
    mtl::deque<int> dq = { 1, 2, 3 };

    dq.pop_front();
    EXPECT_EQ(dq.front(), 2);

    dq.pop_back();
    EXPECT_EQ(dq.back(), 2);
    EXPECT_EQ(dq.size(), 1);
}

TEST(DequeTest, Iterators) {
    mtl::deque<int> dq = { 10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
    auto it = dq.begin();

    EXPECT_EQ(*it, 10);
    EXPECT_EQ(it[2], 30);

    EXPECT_EQ(*(++it), 20);
    EXPECT_EQ(*(it++), 20);
    EXPECT_EQ(*it, 30);
    EXPECT_EQ(*(--it), 20);
    EXPECT_EQ(*(it--), 20);
    EXPECT_EQ(*it, 10);

    it = it + 9;
    EXPECT_EQ(*it, 100);

    it -= 7;
    EXPECT_EQ(*it, 30);

    auto it2 = it + 2;
    EXPECT_EQ(*(2 + it2), 70);

    //EXPECT_EQ(it2 - it, 2);
    //EXPECT_EQ(it - it2, -2);

    EXPECT_TRUE(it < it2);
    EXPECT_TRUE(it2 > it);
    EXPECT_TRUE(it <= it2);
    EXPECT_TRUE(it <= it);
    EXPECT_TRUE(it2 >= it);
    EXPECT_TRUE(it2 >= it2);
    EXPECT_TRUE(it != it2);
    EXPECT_EQ(it, dq.begin() + 2);
}

TEST(DequeTest, StressGrowth) {
    mtl::deque<int> dq;
    
    for (int i = 0; i < 1000; ++i) {
        dq.push_back(i);
        dq.push_front(i);
    }
    EXPECT_EQ(dq.size(), 2000);
    EXPECT_EQ(dq.front(), 999);
    EXPECT_EQ(dq.back(), 999);
}

TEST(DequeTest, ClearAndEmpty) {
    mtl::deque<int> dq = { 1, 2, 3, 4, 5 };
    dq.clear();
    EXPECT_EQ(dq.size(), 0);
    EXPECT_TRUE(dq.empty());
}