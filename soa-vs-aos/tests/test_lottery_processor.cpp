#include <gtest/gtest.h>
#include <vector>
#include <set>
#include <string>
#include <chrono>
#include <random>

#include "../src/lottery_processor.h"
#include "../src/lottery_input_reader.h"

std::string generateRandomPlay() {
    std::string play;
    std::set<int> numbers;

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> dist(1, 60);
    while (numbers.size() < 5) {
        numbers.insert(dist(rng));
    }

    auto it = numbers.begin();
    for (int i = 0; i < numbers.size(); ++i) {
        play += std::to_string((*it++));
        if (i < 4) play += " ";
    }
    return play;
}

TEST(LotteryProcessorTest, CountsMatches) {
    LotteryProcessor lp;

#ifdef ENABLE_SOA
    PlayersInfo data;
#else
    std::vector<PlayerInfo> data;
#endif
    
    PlayerInfo play1;
    Utils::SetPlayToMask({1, 2, 3, 4, 5}, play1.play_mask); // 5 matches

    PlayerInfo play2;
    Utils::SetPlayToMask({1, 2, 10, 11, 12}, play2.play_mask); // 2 matches
    
    PlayerInfo play3;
    Utils::SetPlayToMask({10, 20, 30, 40, 50}, play3.play_mask); // 0 matches

#ifdef ENABLE_SOA
    data.player_id.emplace_back(play1.player_id);
    data.play_mask.emplace_back(play1.play_mask);
    data.player_id.emplace_back(play2.player_id);
    data.play_mask.emplace_back(play2.play_mask);
    data.player_id.emplace_back(play3.player_id);
    data.play_mask.emplace_back(play3.play_mask);
#else
    data.emplace_back(play1);
    data.emplace_back(play2);
    data.emplace_back(play3);
#endif

    testing::internal::CaptureStdout();
    lp.Process(data, {1, 2, 3, 4, 5});
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_EQ(output, "1 0 0 1\n");
}

TEST(LotteryProcessorTest, ValidatingProcessingTimeWith1MPlays) {
    std::string tmpPath = "/tmp/lottery_processor_test_" + std::to_string(::getpid()) + ".txt";

    std::ofstream ofs(tmpPath);
    ASSERT_TRUE(ofs.is_open());
    std::cout << "Creating 1 million plays for performance test..." << std::endl;
    for (size_t i = 0; i < 1'000'000; ++i) {
        ofs << generateRandomPlay() << std::endl;
    }
    std::cout << "Temporary file created." << std::endl;

    LotteryProcessor lp;
    LotteryInputReader reader(tmpPath);
    ASSERT_TRUE(reader.Read());
    
    // Measure processing time over 1000 iterations to get a reliable performance metric
    std::vector<uint64_t> perfTimes;
    for (size_t i = 0; i < 1000; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        lp.Process(reader.GetData(), {1, 11, 22, 50, 60});
        auto end = std::chrono::high_resolution_clock::now();
        
        auto elapsed_ms = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        perfTimes.emplace_back(elapsed_ms.count());
    }

    std::sort(perfTimes.begin(), perfTimes.end());

    // Calculate 90th percentile
    uint64_t percentile90 = perfTimes.size() * 90 / 100;

#ifdef ENABLE_SOA
    std::cout << "Processing time for 1 million plays (with Structure of Arrays): " << perfTimes[percentile90] << " us" << std::endl;
#else
    std::cout << "Processing time for 1 million plays: " << perfTimes[percentile90] << " us" << std::endl;
#endif
    EXPECT_LT(perfTimes[percentile90], 10'000); // Expect processing to be under 10 milliseconds

    // Clean up temporary file
    std::remove(tmpPath.c_str());
}
