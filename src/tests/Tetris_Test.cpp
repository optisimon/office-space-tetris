#include "../Tetris.hpp"

#include <gtest/gtest.h>

#include <atomic>
std::atomic<int> keyPresses(0);
std::atomic<bool> quitting(false);


TEST(Tetris,isPieceOutsideBackground)
{
	GridWithOffset rod(0, 0, 4, 4);
	rod(1, 0) = 2;
	rod(1, 1) = 2;
	rod(1, 2) = 2;
	rod(1, 3) = 2;

	GridWithOffset bg(0, 0, 5, 10);

	rod.setOffset(-2, 0);
	EXPECT_TRUE(isPieceOutsideBackground(bg, rod));
	rod.setOffset(-1, 0);
	EXPECT_FALSE(isPieceOutsideBackground(bg, rod));
	rod.setOffset(0, 0);
	EXPECT_FALSE(isPieceOutsideBackground(bg, rod));
	rod.setOffset(1, 0);
	EXPECT_FALSE(isPieceOutsideBackground(bg, rod));
	rod.setOffset(2, 0);
	EXPECT_FALSE(isPieceOutsideBackground(bg, rod));
	rod.setOffset(3, 0);
	EXPECT_FALSE(isPieceOutsideBackground(bg, rod));
	rod.setOffset(4, 0);
	EXPECT_TRUE(isPieceOutsideBackground(bg, rod));

	// OK to be above playing field
	rod.setOffset(2, -5);
	EXPECT_FALSE(isPieceOutsideBackground(bg, rod));

	// Not ok to be below playing field
	rod.setOffset(2, 20);
	EXPECT_TRUE(isPieceOutsideBackground(bg, rod));
}

TEST(Tetris,Wanted_isOtherBetter_noPenalty)
{
	int const rotation = 0;
	int const xOffset = 0;
	Wanted a = {rotation, xOffset, /*voidPenalty*/ 0, /*heightPenalty*/ 0};
	Wanted b = {rotation, xOffset, /*voidPenalty*/ 0, /*heightPenalty*/ 0};

	EXPECT_FALSE(b.isOtherBetter(a));
	EXPECT_FALSE(a.isOtherBetter(b));
}

TEST(Tetris,Wanted_isOtherBetter_heightPenalty)
{
	int const rotation = 0;
	int const xOffset = 0;
	Wanted a = {rotation, xOffset, /*voidPenalty*/ 0, /*heightPenalty*/ 0};
	Wanted b = {rotation, xOffset, /*voidPenalty*/ 0, /*heightPenalty*/ 1};

	EXPECT_TRUE(b.isOtherBetter(a));
	EXPECT_FALSE(a.isOtherBetter(b));
}

TEST(Tetris,Wanted_isOtherBetter_voidPenalty)
{
	int const rotation = 0;
	int const xOffset = 0;
	Wanted a = {rotation, xOffset, /*voidPenalty*/ 0, /*heightPenalty*/ 0};
	Wanted b = {rotation, xOffset, /*voidPenalty*/ 1, /*heightPenalty*/ 0};

	EXPECT_TRUE(b.isOtherBetter(a));
	EXPECT_FALSE(a.isOtherBetter(b));
}

TEST(Tetris,Wanted_isOtherBetter_voidAndHeightPenalty)
{
	int const rotation = 0;
	int const xOffset = 0;
	Wanted a = {rotation, xOffset, /*voidPenalty*/ 0, /*heightPenalty*/ 0};
	Wanted b = {rotation, xOffset, /*voidPenalty*/ 1, /*heightPenalty*/ 1};

	EXPECT_TRUE(b.isOtherBetter(a));
	EXPECT_FALSE(a.isOtherBetter(b));
}

TEST(Tetris,Wanted_isOtherBetter_equalHeightPenalty)
{
	int const rotation = 0;
	int const xOffset = 0;
	Wanted a = {rotation, xOffset, /*voidPenalty*/ 0, /*heightPenalty*/ 1};
	Wanted b = {rotation, xOffset, /*voidPenalty*/ 0, /*heightPenalty*/ 1};

	EXPECT_FALSE(b.isOtherBetter(a)); // TODO: don't care
	EXPECT_FALSE(a.isOtherBetter(b));
}

TEST(Tetris,Wanted_isOtherBetter_voidPenaltyWorseThanHeightPenalty)
{
	int const rotation = 0;
	int const xOffset = 0;
	Wanted a = {rotation, xOffset, /*voidPenalty*/ 0, /*heightPenalty*/ 1};
	Wanted b = {rotation, xOffset, /*voidPenalty*/ 1, /*heightPenalty*/ 0};

	EXPECT_TRUE(b.isOtherBetter(a));
	EXPECT_FALSE(a.isOtherBetter(b));
}

TEST(Tetris,Wanted_isOtherBetter_voidPenaltyWorseThanHeightPenalty2)
{
	int const rotation = 0;
	int const xOffset = 0;
	Wanted a = {rotation, xOffset, /*voidPenalty*/ 0, /*heightPenalty*/ 1};
	Wanted b = {rotation, xOffset, /*voidPenalty*/ 1, /*heightPenalty*/ 1};

	EXPECT_TRUE(b.isOtherBetter(a));
	EXPECT_FALSE(a.isOtherBetter(b));
}

TEST(Tetris,Wanted_isOtherBetter_equalVoidPenalty)
{
	int const rotation = 0;
	int const xOffset = 0;
	Wanted a = {rotation, xOffset, /*voidPenalty*/ 1, /*heightPenalty*/ 0};
	Wanted b = {rotation, xOffset, /*voidPenalty*/ 1, /*heightPenalty*/ 0};

	EXPECT_FALSE(b.isOtherBetter(a)); // TODO: don't care?
	EXPECT_FALSE(a.isOtherBetter(b));
}

TEST(Tetris,Wanted_isOtherBetter_heightPenalty2)
{
	int const rotation = 0;
	int const xOffset = 0;
	Wanted a = {rotation, xOffset, /*voidPenalty*/ 1, /*heightPenalty*/ 0};
	Wanted b = {rotation, xOffset, /*voidPenalty*/ 1, /*heightPenalty*/ 1};

	EXPECT_TRUE(b.isOtherBetter(a));
	EXPECT_FALSE(a.isOtherBetter(b));
}

TEST(Tetris,Wanted_isOtherBetter_equalAndNonZero)
{
	int const rotation = 0;
	int const xOffset = 0;
	Wanted a = {rotation, xOffset, /*voidPenalty*/ 1, /*heightPenalty*/ 1};
	Wanted b = {rotation, xOffset, /*voidPenalty*/ 1, /*heightPenalty*/ 1};

	EXPECT_FALSE(b.isOtherBetter(a)); // TODO: don't care
	EXPECT_FALSE(a.isOtherBetter(b));
}

TEST(Tetris,GetWanted_test1)
{
	std::vector<GridWithOffset> rod;
	rod.emplace_back(1, 1, 4, 4);
	rod.back()(1, 0) = 2;
	rod.back()(1, 1) = 2;
	rod.back()(1, 2) = 2;
	rod.back()(1, 3) = 2;
	GridWithOffset bg(0, 0, 5, 10);

	for (int y = 5; y < 10; y++)
	{
		for (int x = 1; x < 5; x++)
		{
			bg(x, y) = 1;
		}
	}

	Wanted w = getWanted(bg, rod);
	EXPECT_EQ(-1, w.xOffset);
	EXPECT_EQ(0, w.rotation);
	EXPECT_EQ(0, w.voidPenalty);
}

TEST(Tetris,GetWanted_test2)
{
	std::vector<GridWithOffset> rod;
	rod.emplace_back(1, 1, 4, 4);
	rod.back()(0, 1) = 2;
	rod.back()(1, 1) = 2;
	rod.back()(2, 1) = 2;
	rod.back()(3, 1) = 2;
	rod.emplace_back(1, 1, 4, 4);
	rod.back()(1, 0) = 2;
	rod.back()(1, 1) = 2;
	rod.back()(1, 2) = 2;
	rod.back()(1, 3) = 2;

	GridWithOffset bg(0, 0, 5, 10);

	for (int y = 5; y < 10; y++)
	{
		for (int x = 1; x < 5; x++)
		{
			bg(x, y) = 1;
		}
	}

	Wanted w = getWanted(bg, rod);
	EXPECT_EQ(-1, w.xOffset);
	EXPECT_EQ(1, w.rotation);
	EXPECT_EQ(0, w.voidPenalty);
}

TEST(Tetris,GetWanted_test3)
{
	std::vector<GridWithOffset> rod;
	// 0000
	// 2222
	// 0000
	// 0000
	rod.emplace_back(1, 1, 4, 4);
	rod.back()(0, 1) = 2;
	rod.back()(1, 1) = 2;
	rod.back()(2, 1) = 2;
	rod.back()(3, 1) = 2;

	// 0200
	// 0200
	// 0200
	// 0200
	rod.emplace_back(1, 1, 4, 4);
	rod.back()(1, 0) = 2;
	rod.back()(1, 1) = 2;
	rod.back()(1, 2) = 2;
	rod.back()(1, 3) = 2;

	// occupy all blocks except the leftmost ones
	// 00000
	// 00000
	// 00000
	// 00000
	// 00000
	// 00000
	// 00000
	// 01111
	// 01111
	// 01111
	GridWithOffset bg(0, 0, 5, 10);
	for (int y = 7; y < 10; y++)
	{
		for (int x = 1; x < 5; x++)
		{
			bg(x, y) = 1;
		}
	}

	Wanted w = getWanted(bg, rod);
	EXPECT_EQ(-1, w.xOffset);
	EXPECT_EQ(1, w.rotation);
	EXPECT_EQ(0, w.voidPenalty);
}

TEST(Tetris,GetWanted_test4)
{
	std::vector<GridWithOffset> rod;
	// 0000
	// 2222
	// 0000
	// 0000
	rod.emplace_back(1, 1, 4, 4);
	rod.back()(0, 1) = 2;
	rod.back()(1, 1) = 2;
	rod.back()(2, 1) = 2;
	rod.back()(3, 1) = 2;

	// 0200
	// 0200
	// 0200
	// 0200
	rod.emplace_back(1, 1, 4, 4);
	rod.back()(1, 0) = 2;
	rod.back()(1, 1) = 2;
	rod.back()(1, 2) = 2;
	rod.back()(1, 3) = 2;

	// occupy all blocks except the leftmost ones
	// 00000
	// 00000
	// 00000
	// 00000
	// 00000
	// 00110
	// 00100
	// 00100
	// 00100
	// 00100
	GridWithOffset bg(0, 0, 5, 10);
	for (int y = 5; y < 10; y++)
	{
			bg(2, y) = 1;
	}
	bg(3,5) = 1;

	Wanted w = getWanted(bg, rod);
	EXPECT_TRUE(w.xOffset == -1 || w.xOffset == 0 || w.xOffset == 3);
	EXPECT_EQ(1, w.rotation);
	EXPECT_EQ(4, w.voidPenalty);
}
