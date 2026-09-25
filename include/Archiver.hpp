#pragma once
#include "ArchiverFormat.hpp"
#include <filesystem>
#include <vector>

namespace arc {

    namespace fs = std :: filesystem;
    
        //create an archive at 'archive_path' containing all files from source_dir
        void pack(const fs::path& source_dir,const fs :: path& archive_path);

        //read and return the metadata entries from an archive without extracting payload
        std :: vector<FileEntry> list(const fs :: path& archive_path);
        
        //exctract all files from archive_path to output_dir
         void exctract(const fs ::  path& archive_path, const fs :: path& output_dir);
    };

