#include <catch2/catch_test_macros.hpp>
#include "Archiver.hpp"
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>


namespace fs = std::filesystem;

TEST_CASE("Pack,List and Extract Roundtrip", "[archiver]"){
    //temp sandbox
    fs::path temp_base = fs::temp_directory_path() / "arc_unit_test";
    fs::path src_dir   = temp_base / "source";
    fs::path arc_file  = temp_base / "bundle.arc";
    fs::path out_dir   = temp_base / "extracted";

    fs::remove_all(temp_base);
    fs::create_directories(src_dir/"nested");

    //sample files
    std::string content1 = " Hello, this is the Binary Archiver!";
    std::string content2 = " It is pretty windy today.\n I guess it might rain later";

    {
        std::ofstream f1(src_dir / "file1.txt");
        f1 << content1;
        std::ofstream f2(src_dir / "nested" / "file2.txt");
        f2 << content2;

    }
    //test pack
    REQUIRE_NOTHROW(arc::pack(src_dir,arc_file));
    REQUIRE(fs::exists(arc_file));
    //test list
    auto entries = arc::list(arc_file);
    REQUIRE((entries.size()) == 2);
    //test extract
    REQUIRE_NOTHROW(arc::extract(arc_file,out_dir));

    //verify contents
    fs::path extracted1 = out_dir / "file1.txt";
    fs::path extracted2 = out_dir / "nested" / "file2.txt";

    REQUIRE(fs::exists(extracted1));
    REQUIRE(fs::exists(extracted1));

    std::ifstream input1(extracted1);
    std::string read1((std::istreambuf_iterator<char>(input1)),{});
    CHECK(read1 == content1);

    std::ifstream input2(extracted2);
    std::string read2((std::istreambuf_iterator<char>(input2)),{});
    CHECK(read2 == content2);

    //clean up
    fs::remove_all(temp_base);

}