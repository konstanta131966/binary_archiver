#include "../include/Archiver.hpp"
#include "ArchiverFormat.hpp"
#include <filesystem>
#include <fstream>

namespace arc {

 std :: vector<FileEntry> entries;

void pack(const fs :: path& source_dir,const fs::path& archive_path);
   if (!(fs::is_directory(source_dir))){
    return;
   }
}