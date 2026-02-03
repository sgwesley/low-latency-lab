#include <gtest/gtest.h>
#include <fstream>
#include <cstdio>
#include <unistd.h>
#include <vector>

#include "../src/lottery_input_reader.h"

TEST(LotteryInputReaderTest, ReadsRowsOfFive) {
    std::string tmpPath = "/tmp/input_reader_test_" + std::to_string(::getpid()) + ".txt";

    std::ofstream ofs(tmpPath);
    ASSERT_TRUE(ofs.is_open());
    ofs << "1 2 3 4 5" << std::endl;
    ofs << "6 7 8 9 10";
    ofs.close();

    LotteryInputReader reader(tmpPath);
    EXPECT_TRUE(reader.Read());

    const auto& data = reader.GetData();
#ifdef ENABLE_SOA
    ASSERT_EQ(data.player_id.size(), 2u);
#else
    ASSERT_EQ(data.size(), 2u);
#endif

    std::vector<int> expected1 = {1,2,3,4,5};
    PlayerInfo mask1;
    Utils::SetPlayToMask(expected1, mask1.play_mask);

#ifdef ENABLE_SOA
    EXPECT_EQ(data.play_mask[0], mask1.play_mask);
#else
    EXPECT_EQ(data[0].play_mask, mask1.play_mask);
#endif

    std::vector<int> expected2 = {6,7,8,9,10};
    PlayerInfo mask2;
    Utils::SetPlayToMask(expected2, mask2.play_mask);
#ifdef ENABLE_SOA
    EXPECT_EQ(data.play_mask[1], mask2.play_mask);
#else
    EXPECT_EQ(data[1].play_mask, mask2.play_mask);
#endif

    // Clean up temporary file
    std::remove(tmpPath.c_str());
}

TEST(LotteryInputReaderTest, ReadsEmptyFile) {
    std::string tmpPath = "/tmp/input_reader_test_" + std::to_string(::getpid()) + ".txt";

    std::ofstream ofs(tmpPath);
    ASSERT_TRUE(ofs.is_open());
    ofs.close();

    LotteryInputReader reader(tmpPath);
    EXPECT_FALSE(reader.Read());

    // Clean up temporary file
    std::remove(tmpPath.c_str());
}

TEST(LotteryInputReaderTest, ReadsInvalidPlays) {
    std::string tmpPath = "/tmp/input_reader_test_" + std::to_string(::getpid()) + ".txt";

    std::ofstream ofs(tmpPath);
    ASSERT_TRUE(ofs.is_open());
    ofs << "0 2 3 4 91" << std::endl;
    ofs << "6 7 8 9";
    ofs.close();

    LotteryInputReader reader(tmpPath);
    EXPECT_FALSE(reader.Read());

    // Clean up temporary file
    std::remove(tmpPath.c_str());
}