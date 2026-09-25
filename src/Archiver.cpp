#include "Archiver.hpp"
#include "ArchiverFormat.hpp"
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <stdexcept>

namespace arc {


    void pack(const fs :: path& source_dir,const fs::path& archive_path){
        //they depend on archive_path
        std :: ofstream out(archive_path, std::ios::binary);
        std :: vector<FileEntry> entries;

         if (!(fs::is_directory(source_dir))){
            throw std::runtime_error("Source path is not a directory: " + source_dir.string());
        }
        for (const auto& entry : fs :: recursive_directory_iterator(source_dir)){
          if(!entry.is_regular_file()){
           continue; //skip subdirectories,symlinks,..
          }
          //copy the relative path

          std::string rel_path = fs::relative(entry.path(),source_dir).generic_string();
          if (rel_path.size() >= MAX_PATH_LEN){
            throw std::runtime_error("File path exceeds maximum limit: " + rel_path);
          }  
          //zero-initialize entire struct (clears junk memory)
          FileEntry newEntry{};
          std::strncpy(newEntry.path,rel_path.c_str(),sizeof(newEntry.path)-1);

          /*where are we in the archive? out.tellp() returns te put-pointer pos 
          as a std::streampos*/
          newEntry.data_offset = static_cast<uint64_t>(out.tellp());
          newEntry.file_size = fs::file_size(entry.path());
          //exctracts POSIX permissions directly via <filesystem>
          newEntry.permissions = static_cast<uint32_t>(entry.status().permissions());

          //streaming the payload bytes
          std::ifstream input(entry.path(),std::ios::binary);
          if(!out){
            throw std::runtime_error("Failed to create archive file: " + archive_path.string());
          }
          if(!input){
            throw std::runtime_error("Failed to open file: " + entry.path().string());
          }
          char buffer[65536]; //64KB
          while(input.read(buffer,sizeof(buffer)) || input.gcount() > 0){
            out.write(buffer,input.gcount());
          }

          entries.push_back(newEntry);

        }
        //now all payloads are in out
    }

    std::vector<FileEntry> list(const fs::path& archive_path){

    }

    void extract(const fs::path& archive_path, const fs::path& output_dir){

    }
}
