#include "gtest/gtest.h"
#include "MTL/Memory.hpp"
#include <memory>

struct RefCounter 
{
    inline static int count = 0;
    RefCounter() { count++; }
    ~RefCounter() { count--; }
};

TEST(UniquePtrTest, BasicCreation) {
    RefCounter::count = 0;
    {
        mtl::unique_ptr<RefCounter> ptr(new RefCounter());
        EXPECT_EQ(RefCounter::count, 1);
        EXPECT_NE(ptr.get(), nullptr);
    }
    EXPECT_EQ(RefCounter::count, 0);
}

TEST(UniquePtrTest, MoveSemantics) {
    auto ptr1 = mtl::make_unique<RefCounter>();
    mtl::unique_ptr<RefCounter> ptr2 = std::move(ptr1);

    EXPECT_EQ(ptr1.get(), nullptr);
    ASSERT_NE(ptr2.get(), nullptr);
    EXPECT_EQ(RefCounter::count, 1);
}

TEST(UniquePtrTest, ResetAndRelease) {
    RefCounter::count = 0;
    mtl::unique_ptr<RefCounter> ptr(new RefCounter());

    ptr.reset(new RefCounter());
    EXPECT_EQ(RefCounter::count, 1);

    RefCounter* raw = ptr.release();
    EXPECT_EQ(ptr.get(), nullptr);
    EXPECT_EQ(RefCounter::count, 1);
    delete raw;
}

TEST(UniquePtrArrayTest, ArrayBehavior) {
    RefCounter::count = 0;
    {
        mtl::unique_ptr<RefCounter[]> arr(new RefCounter[3]);
        EXPECT_EQ(RefCounter::count, 3);
    }
    EXPECT_EQ(RefCounter::count, 0);
}

TEST(UniquePtrArrayTest, AccessOperator) {
    auto arr = mtl::make_unique<int[]>(5);
    for (int i = 0; i < 5; ++i) {
        arr[i] = i * 10;
    }
    EXPECT_EQ(arr[3], 30);
}

TEST(SharedPtrTest, DefaultConstructor) {
    mtl::shared_ptr<int> ptr;
    EXPECT_EQ(ptr.get(), nullptr);
    EXPECT_EQ(ptr.use_count(), 0);
}

TEST(SharedPtrTest, ConstructorWithObject) {
    mtl::shared_ptr<int> ptr(new int(42));
    ASSERT_NE(ptr.get(), nullptr);
    EXPECT_EQ(*ptr, 42);
    EXPECT_EQ(ptr.use_count(), 1);
}

TEST(SharedPtrTest, CopyConstructor) {
    mtl::shared_ptr<int> ptr1 = mtl::make_shared<int>(10);
    {
        mtl::shared_ptr<int> ptr2 = ptr1;
        EXPECT_EQ(ptr1.use_count(), 2);
        EXPECT_EQ(ptr2.use_count(), 2);
        EXPECT_EQ(*ptr1, *ptr2);
        EXPECT_EQ(ptr1.get(), ptr2.get());
    }
    EXPECT_EQ(ptr1.use_count(), 1);
}

TEST(SharedPtrTest, MoveConstructor) {
    mtl::shared_ptr<int> ptr1 = mtl::make_shared<int>(20);
    mtl::shared_ptr<int> ptr2(std::move(ptr1));

    EXPECT_EQ(ptr2.use_count(), 1);
    EXPECT_EQ(*ptr2, 20);
    EXPECT_EQ(ptr1.get(), nullptr);
    EXPECT_EQ(ptr1.use_count(), 0);
}

TEST(SharedPtrTest, MemoryCleanup) {
    RefCounter::count = 0;
    {
        mtl::shared_ptr<RefCounter> ptr1 = mtl::make_shared<RefCounter>();
        EXPECT_EQ(RefCounter::count, 1);
        {
            mtl::shared_ptr<RefCounter> ptr2 = ptr1;
            EXPECT_EQ(RefCounter::count, 1);
            EXPECT_EQ(ptr1.use_count(), 2);
        }
        EXPECT_EQ(RefCounter::count, 1);
    }
    EXPECT_EQ(RefCounter::count, 0);
}

TEST(SharedPtrTest, AssignmentOperator) {
    mtl::shared_ptr<int> ptr1(new int(5));
    mtl::shared_ptr<int> ptr2(new int(10));

    ptr2 = ptr1;

    EXPECT_EQ(ptr1.use_count(), 2);
    EXPECT_EQ(ptr2.use_count(), 2);
    EXPECT_EQ(*ptr2, 5);
}