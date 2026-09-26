#include <exception>
#include <iostream>
#include "Archiver.hpp"
#include<string_view>
#include <iomanip>

void print_usage(std::string_view file_archiver){
    std::cout << "Select usage:\n"
              << " " << file_archiver << " Pack <source_dir><archive.arc>\n"
              << " " << file_archiver <<" List <archive.arc>\n"
              << " " << file_archiver << " Extract <archive.arc><output_dir>\n";
}

int main(int argc ,char* argv[]){
    if(argc < 2){
        print_usage(argv[0]);
        return 1;
    }

    std::string_view command = argv[1];

    try{
        if (command == "pack"){
            if (argc != 4){
                print_usage(argv[0]);
                return 1;
            }
            arc::pack(argv[2],argv[3]);
            std::cout << "Archive created succesfully: " << argv[3] << "\n";
        }
        else if (command == "list"){
            if (argc != 3){
                print_usage(argv[0]);
                return 1;
            }
            auto entries = arc::list(argv[2]);
            std::cout << std::left << std::setw(12) << "Size (B)"
                      << " " << "Pathn\n";
            std::cout << std::string(40,'-')<<"\n";
            for(const auto& entry : entries){
                std::cout << std::left << std::setw(12) << entry.file_size
                    << " " << entry.path << "\n";
            }  
        }
        else if (command == "extract"){
            if(argc != 4){
                print_usage(argv[0]);
                return 1;
            }
            arc::extract(argv[2],argv[3]);
            std::cout << "Extracted succesfully into: " << argv[3] << "\n";
        }
        else{
            std::cerr << "Unknown command: " << command << "\n";
            print_usage(argv[0]);
            return 1;
        }
    }
    catch (const std::exception& ex){
        std::cerr << "Error: " << ex.what() << "\n";
        return 1;
    }
    return 0;
}