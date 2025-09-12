#include "catch_amalgamated.hpp"
#include "song_select.h"
#include <filesystem>
#include <fstream>

TEST_CASE("SongSlot Loads And Return Data", "[SongSelect]") {
    namespace fs = std::filesystem;

    fs::path testDir = fs::temp_directory_path() / "load_data_test_song_slot";
    fs::create_directories(testDir);
   
    fs::path songDir = testDir / "TestSong";
    fs::create_directories(songDir);

    fs::path dataFileDir = songDir / "data.json";

    std::ofstream dataFile(dataFileDir);
    dataFile << R"({
        "SongName": "Test Song",
        "Artist": "Test Artist",
        "Mapper": "Test Mapper",
        "Difficulty": 5
    })";
    
    dataFile.close();

    std::string songDirPath = songDir.string();

    auto songData = load_and_return_data(songDirPath);

    REQUIRE(songData.FolderLocation == songDirPath);
    REQUIRE(!songData.SongName.empty());
    REQUIRE(!songData.Artist.empty());
    REQUIRE(!songData.Mapper.empty());
    REQUIRE(songData.Difficulty == 5);

    REQUIRE(songData.SongName == "Test Song");
    REQUIRE(songData.Artist == "Test Artist");
    REQUIRE(songData.Mapper == "Test Mapper");

    fs::remove_all(testDir);
}
