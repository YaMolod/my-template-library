#include "gtest/gtest.h"
#include "MTL/Vector.hpp"
#include "MTL/String.hpp"

TEST(VectorTest, PushGrowth) {
    mtl::vector<int> v;
    EXPECT_TRUE(v.empty());
    EXPECT_EQ(v.size(), 0);

    for (int i = 0; i < 10; ++i) {
        v.push_back(i);
    }

    EXPECT_EQ(v.size(), 10);
    EXPECT_GE(v.capacity(), 10);
    EXPECT_EQ(v[5], 5);
}

TEST(VectorTest, EmplaceGrowth) {
    mtl::vector<std::pair<int, mtl::string>> v;
    EXPECT_TRUE(v.empty());
    EXPECT_EQ(v.size(), 0);

    for (int i = 0; i < 10; ++i) {
        v.emplace_back(i, "hello");
    }

    EXPECT_EQ(v.size(), 10);
    EXPECT_GE(v.capacity(), 10);
    EXPECT_EQ(v[5].first, 5);
    EXPECT_EQ(v[5].second, "hello");
}

TEST(VectorTest, CopyAndMove) {
    mtl::vector<int> original = { 1, 2, 3 };

    mtl::vector<int> copy = original;
    EXPECT_EQ(copy.size(), original.size());
    EXPECT_NE(copy.data(), original.data());
    EXPECT_EQ(copy[0], 1);

    int* old_data = original.data();
    mtl::vector<int> moved = std::move(original);
    EXPECT_EQ(moved.size(), 3);
    EXPECT_EQ(moved.data(), old_data);
}

TEST(VectorTest, PopAndClear) {

    mtl::vector<int> v = { 1, 2, 3, 4, 5 };
    v.pop_back();
    v.pop_back();
    v.pop_back();

    EXPECT_EQ(v[1], 2);
    EXPECT_EQ(v.size(), 2);
    EXPECT_EQ(v.capacity(), 5);
}

TEST(VectorTest, Resize) {

    mtl::vector<int> v(10);
    EXPECT_EQ(v.capacity(), 10);
    v.reserve(20);
    EXPECT_EQ(v.size(), 10);
    EXPECT_EQ(v.capacity(), 20);
    EXPECT_EQ(v[9], 0);

    v.clear();
    EXPECT_EQ(v.size(), 0);
    EXPECT_EQ(v.capacity(), 20);
}

TEST(VectorIteratorTest, BasicOperations) {
    mtl::vector<int> v = { 10, 20, 30 };
    auto it = v.begin();

    EXPECT_EQ(*it, 10);
    EXPECT_EQ(*(++it), 20);
    EXPECT_EQ(*(it++), 20);
    EXPECT_EQ(*it, 30);
}

TEST(VectorIteratorTest, Comparison) {
    mtl::vector<int> v = { 1, 2, 3 };
    auto it1 = v.begin();
    auto it2 = v.begin();
    auto it_end = v.end();

    EXPECT_EQ(it1, it2);
    EXPECT_NE(it1, it_end);
}

TEST(VectorIteratorTest, RangeBasedForLoop) {
    mtl::vector<int> v = { 1, 2, 3 };
    int sum = 0;
    for (int x : v) {
        sum += x;
    }
    EXPECT_EQ(sum, 6);
}