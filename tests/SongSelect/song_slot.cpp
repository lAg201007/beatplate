#include "catch_amalgamated.hpp"
#include "song_select.h"
#include <typeinfo>
#include <filesystem>
#include <fstream>

TEST_CASE("Creating Song Slot Vector", "[SongSelect]") {
    namespace fs = std::filesystem;
    int iterations = 5;

    fs::path testDir = fs::temp_directory_path() / "vector_loading_test_song_slot";
    fs::create_directories(testDir);

    for (int i = 1; i <= iterations; ++i) {
        fs::path songDir = testDir / ("Song" + std::to_string(i));
        fs::create_directories(songDir);

        fs::path dataFileDir = songDir / "data.json";

        std::ofstream dataFile(dataFileDir);
        dataFile << R"({
            "SongName": "Song )" << i << R"(",
            "Artist": "Artist )" << i << R"(",
            "Mapper": "Mapper )" << i << R"(",
            "Difficulty": )" << i*2 << R"(
        })";
        
        dataFile.close();
    }

    std::vector<SongSlotData> slotDataList = load_song_slot_data(testDir.string());

    std::vector<std::shared_ptr<SongSlot>> slots = createSongSlots(slotDataList, sf::Vector2f(974, 432));

    REQUIRE(slots.size() == iterations);
    REQUIRE(typeid(slots) == typeid(std::vector<std::shared_ptr<SongSlot>>));
    for (const auto& slot : slots) {
        REQUIRE(slot != nullptr);
        REQUIRE(typeid(slot) == typeid(std::shared_ptr<SongSlot>));
        REQUIRE(typeid(*slot) == typeid(SongSlot));
        REQUIRE(!slot->SongName.empty());
        REQUIRE(!slot->Artist.empty());
        REQUIRE(!slot->Mapper.empty());
        REQUIRE(typeid(slot->Difficulty) == typeid(int));
        REQUIRE(typeid(slot->SongName) == typeid(std::string));
        REQUIRE(typeid(slot->Artist) == typeid(std::string));
        REQUIRE(typeid(slot->Mapper) == typeid(std::string));
    }
}