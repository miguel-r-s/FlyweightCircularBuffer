#include "flyweight_circular_buffer.h"

#include <gtest/gtest.h>

template<typename T, typename Hasher = std::hash<T>>
struct FlyweightCircularBufferTest : public ::testing::Test
{
	FlyweightCircularBufferTest()
	: buffer(5)
	{}

	FlyweightCircularBuffer<T, Hasher> buffer;
};

using IntFlyweightCircularBuffer = FlyweightCircularBufferTest<int>;

TEST_F(IntFlyweightCircularBuffer, Init_Works)
{
	EXPECT_EQ(buffer.DistinctObjectCount(), 0);
}

TEST_F(IntFlyweightCircularBuffer, Size_WellSet)
{
	EXPECT_EQ(5, buffer.Size()); 
}

TEST_F(IntFlyweightCircularBuffer, BadQueryIdx_Throws)
{
	// Query empty buffer
	EXPECT_THROW(buffer.Query(0), std::out_of_range);

	buffer.Insert(1);

	// Querying the first element should work, but the second should still throw
	EXPECT_NO_THROW(buffer.Query(0));
	EXPECT_THROW(buffer.Query(1), std::out_of_range);

	// Fill the buffer with 4 more elements
	for (int i = 0; i < 3; ++i) buffer.Insert(i);

	EXPECT_THROW(buffer.Query(4), std::out_of_range);
	EXPECT_THROW(buffer.Query(5), std::out_of_range);
}

TEST_F(IntFlyweightCircularBuffer, OutOfBoundsQueryIdx_Throws)
{
	EXPECT_THROW(buffer.Query(-1), std::out_of_range);
	EXPECT_THROW(buffer.Query(5), std::out_of_range);
}

TEST_F(IntFlyweightCircularBuffer, InsertOneElement_RightIndex)
{
	buffer.Insert(1);
	EXPECT_EQ(buffer.Query(0), 1);
}

TEST_F(IntFlyweightCircularBuffer, InsertTwoElements_RightIndices)
{
	buffer.Insert(1);
	EXPECT_EQ(buffer.Query(0), 1);

	buffer.Insert(2);
	EXPECT_EQ(buffer.Query(0), 1);
	EXPECT_EQ(buffer.Query(1), 2);
}

TEST_F(IntFlyweightCircularBuffer, FillBuffer_RightIndices)
{
	for (int i = 0; i < 5; ++i)
	{
		buffer.Insert(i);
	}

	for (int i = 0; i < 5; ++i)
	{
		EXPECT_EQ(buffer.Query(i), i);
	}
}

TEST_F(IntFlyweightCircularBuffer, InsertOneMoreThanSizeElements_IndiceWrapsAroundCorrectly)
{
	// Wrap around by 1 element "too much"
	for (int i = 0; i < 6; ++i)
		buffer.Insert(i);

	for (int i = 0; i < 5; ++i)
		EXPECT_EQ(buffer.Query(i), i+1);
}

TEST_F(IntFlyweightCircularBuffer, InsertMoreThanSizeElements_IndicesWrapAroundCorrectly)
{
	// 200 is a multiple of the size (5)
	// so the last three numbers should override 
	// 195, 196, 197 making 198 the element at index 0 
	for (int i = 0; i < 200+3; ++i)
		buffer.Insert(i);

	for (int i = 0; i < 5; ++i)
		EXPECT_EQ(buffer.Query(i), i+198);
}

TEST_F(IntFlyweightCircularBuffer, Flyweight_HandleRepeats)
{
	EXPECT_EQ(buffer.DistinctObjectCount(), 0);

	for (int i = 0; i < 5; ++i)
	{
		buffer.Insert(100);
		EXPECT_EQ(buffer.DistinctObjectCount(), 1);
	}

	for (int i = 0; i < 100; ++i)
	{
		buffer.Insert(i);
	}
	EXPECT_EQ(buffer.DistinctObjectCount(), 5);
}

using StringFlyweightCircularBuffer = FlyweightCircularBufferTest<std::string>;

TEST_F(StringFlyweightCircularBuffer, Init)
{
	EXPECT_EQ(buffer.DistinctObjectCount(), 0);
}

TEST_F(StringFlyweightCircularBuffer, InsertOne)
{
	buffer.Insert("asdf");
	EXPECT_EQ(buffer.Query(0), "asdf");
	EXPECT_EQ(buffer.DistinctObjectCount(), 1);
}

TEST_F(StringFlyweightCircularBuffer, InsertThreeDifferent)
{
	buffer.Insert("qwer");
	buffer.Insert("asdf");
	buffer.Insert("zxcv");
	EXPECT_EQ(buffer.Query(0), "qwer");
	EXPECT_EQ(buffer.Query(1), "asdf");
	EXPECT_EQ(buffer.Query(2), "zxcv");
	EXPECT_EQ(buffer.DistinctObjectCount(), 3);
}

TEST_F(StringFlyweightCircularBuffer, InsertRepeated_NoExtraCopies)
{
	buffer.Insert("qwer");
	buffer.Insert("qwer");
	buffer.Insert("qwer");
	EXPECT_EQ(buffer.DistinctObjectCount(), 1);
}

TEST_F(StringFlyweightCircularBuffer, InsertRepeatedAfterFull_StillNoExtraCopies)
{
	buffer.Insert("1");
	buffer.Insert("2");
	buffer.Insert("3");
	buffer.Insert("4");
	buffer.Insert("5");

	for (int i = 0; i < 5; ++i)
	{
		EXPECT_EQ(buffer.DistinctObjectCount(), 5-i);
		buffer.Insert("5");
	}

	EXPECT_EQ(buffer.DistinctObjectCount(), 1);

	buffer.Insert("1");

	EXPECT_EQ(buffer.DistinctObjectCount(), 2);
}

TEST_F(StringFlyweightCircularBuffer, InsertMoreThanSizeElements_IndicesWrapAroundCorrectly)
{
	for (int i = 0; i < 5; ++i)
	{
		buffer.Insert(std::to_string(i));
		EXPECT_EQ(buffer.Query(i%5), std::to_string(i));
	}

	buffer.Insert("100");
	EXPECT_EQ(buffer.DistinctObjectCount(), 5);

	EXPECT_EQ(buffer.Contains("0"), false);
	EXPECT_EQ(buffer.Contains("100"), true);

	EXPECT_EQ(buffer.Query(0), "1");
	EXPECT_EQ(buffer.Query(4), "100");
}

TEST_F(StringFlyweightCircularBuffer, InsertManyElements_IndicesWrapAroundCorrectly)
{
	for (int i = 0; i < 2000; ++i)
	{
		std::string str = std::to_string(i);
		buffer.Insert(str);
	}

	EXPECT_EQ(buffer.Query(0), "1995");

	buffer.Insert("asdf");

	EXPECT_EQ(buffer.Query(0), "1996");
	EXPECT_EQ(buffer.Query(3), "1999");
	EXPECT_EQ(buffer.Query(4), "asdf");

	EXPECT_EQ(buffer.Contains("1995"), false);
	EXPECT_EQ(buffer.Contains("1999"), true);
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
