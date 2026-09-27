#include "Archiver.hpp"
#include "ArchiverFormat.hpp"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iterator>
#include <stdexcept>

namespace arc {
    //helper function
    void pack_single_file(const fs::path& disk_path,const std::string& entry_rel_path,
         std::ofstream& out,std::vector<arc::FileEntry>& entries){
        if (entry_rel_path.size() >= MAX_PATH_LEN){
                throw std::runtime_error("File path exceeds maximum limit: " + entry_rel_path);
            }
        
        arc::FileEntry newEntry{};
        std::strncpy(newEntry.path,entry_rel_path.c_str(),sizeof(newEntry.path)-1);

        newEntry.data_offset = static_cast<uint64_t>(out.tellp());
        newEntry.file_size   = fs::file_size(disk_path);
        newEntry.permissions = static_cast<uint32_t>(fs::status(disk_path).permissions());

        std::ifstream input(disk_path,std::ios::binary);
        if(!input){
            throw std::runtime_error("Failed to open file: " + disk_path.string());
        }
        char buffer[65536]; //64KB
        while(input.read(buffer,sizeof(buffer)) || input.gcount() > 0){
            out.write(buffer,input.gcount());
        }
        entries.push_back(newEntry);
    }
    //file writing
    void pack(const fs :: path& archive_path,const std::vector<fs::path>& inputs){
        //they depend on archive_path
        std :: ofstream out(archive_path, std::ios::binary);
        if(!out){
            throw std::runtime_error("Failed to create archive file: " + archive_path.string());
        }
        std :: vector<FileEntry> entries;
        for(const auto& input_path : inputs){
            if(!fs::exists(input_path)){
                throw std::runtime_error("Input path does not exists: " + input_path.string());
            }
            
            if(fs::is_regular_file(input_path)){
                //case 1: standalone file -> use just file name
                std::string stored_name = input_path.filename().generic_string();
                pack_single_file(input_path, stored_name, out, entries);
            }
            else if (fs::is_directory(input_path)){
                //case 2: directory -> traverse recursively
                //preserve parent name to "my_dir/file.txt" so it extracts to "my_dir/file.txt"
                fs::path parent = input_path.parent_path();

                for(const auto& entry : fs::recursive_directory_iterator(input_path)){
                    if(entry.is_regular_file()){
                        std::string rel_path = fs::relative(entry.path(),input_path).generic_string();
                        if(fs::exists(archive_path) && fs::equivalent(entry.path(),archive_path)){
                            continue;
                        }
                        pack_single_file(entry.path(), rel_path, out, entries);
                    }
                }
            }
        }
        //table of contents starts here
        uint64_t directory_offset = static_cast<uint64_t>(out.tellp());
        //writing table of contents(all FileEntries back-to-back)
        if(!entries.empty()){
            out.write(reinterpret_cast<const char*>(entries.data()),
             entries.size() * sizeof(FileEntry));
        }

        //populate and write the EndRecord
        EndRecord end_record{};
        end_record.magic_number     = END_RECORD_MAGIC;
        end_record.version          = FORMAT_VERSION;
        end_record.directory_offset = directory_offset;
        end_record.entry_count      = entries.size();
        
        out.write(reinterpret_cast<const char*>(&end_record),sizeof(EndRecord));
    }

    //file inspection
    std::vector<FileEntry> list(const fs::path& archive_path){
        std::ifstream input(archive_path,std::ios::binary);
        if(!input){
            throw std::runtime_error("Failed to open archive: "+ archive_path.string());
        }
        if(fs::file_size(archive_path) < sizeof(EndRecord)){
            throw std::runtime_error("Archive file is cirrupted or too small: "+ archive_path.string());
        }
        input.seekg(-static_cast<std::streamoff>(sizeof(EndRecord)),std::ios::end);
        //read and validate EndRecord

        EndRecord end_record{};
        input.read(reinterpret_cast<char*>(&end_record),sizeof(EndRecord));
        if(end_record.magic_number  != END_RECORD_MAGIC){
            throw std::runtime_error("Not a valid .arc archive (magic mismatch)");
        }
        //jump to the table of contents
        input.seekg(static_cast<std::streamoff>(end_record.directory_offset),std::ios::beg);
        //read all FileEntry records into a vector
        std::vector<FileEntry> entries(end_record.entry_count);
        if(end_record.entry_count > 0){
            input.read(reinterpret_cast<char*>(entries.data()),end_record.entry_count*sizeof(FileEntry));
        }
        return entries;
    }

    void extract(const fs::path& archive_path, const fs::path& output_dir){

        auto entries = list(archive_path);
        std::ifstream input(archive_path,std::ios::binary);
        if(!input){
            throw std::runtime_error("Failed to open archive: " + archive_path.string());
        }

        char buffer[65536]; //64KB

        for(const FileEntry& fileEntry : entries){
            //construting target file path & ensure parent directory structure exists
            fs::path destination = output_dir / fileEntry.path;
            if(destination.has_parent_path()){
                fs::create_directories(destination.parent_path()); 
            }
            //go to payload start
            input.seekg(static_cast<std::streamoff>(fileEntry.data_offset),std::ios::beg);

            std::ofstream out(destination,std::ios::binary);
            if(!out){
                throw std::runtime_error("Failed to create destination file: " + destination.string());
            }

            //stream exactly file_size bytes
            uint64_t remaining_bytes = fileEntry.file_size;
            while(remaining_bytes > 0){
                size_t chunk = std::min(static_cast<uint64_t>(sizeof(buffer)),remaining_bytes);

                input.read(buffer,chunk);
                out.write(buffer,chunk);
                remaining_bytes -= chunk;
            }
            out.close();
            //restore POSIX permissions
            fs::permissions(destination,static_cast<fs::perms>(fileEntry.permissions));
        }
    }
}


