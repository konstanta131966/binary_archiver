#pragma once

#include <cstdint>
#include <cstddef>

namespace arc {
    //constants
    inline constexpr uint32_t ARCHIVE_MAGIC = 0x41524348;
    inline constexpr uint32_t END_RECORD_MAGIC = 0x444E4541;
    inline constexpr uint16_t FORMAT_VERSION = 1;
    inline constexpr size_t MAX_PATH_LEN = 256;


#pragma pack(push,1) //we enforce zero padding

//represents one file in the Central Directory
struct FileEntry{
    char path[arc::MAX_PATH_LEN]; //null-terminated relative path
    uint64_t data_offset;        //byte position in archive where payload begins
    uint64_t file_size;            //size of uncompressed raw payload in bites
    uint32_t permissions;          //POSIX file permissions bitmask
};

//fixed-size record written at the end of the archive
struct EndRecord{
    uint64_t magic_number; //must match END_RECORD_MAGIC
    uint16_t version;       //archive format version
    uint64_t directory_offset; //byte offset where the array of FileEntry begins
    uint64_t entry_count; // number of FileEntry items in the directory
};

#pragma pack(pop)

static_assert(sizeof(FileEntry) == 276, "FileEntry layout has unexpected padding");
static_assert(sizeof(EndRecord) == 26,"EndRecord has unexpected padding");

}//namespace arc ends



/*
[ START OF FILE ]
|--- Payload 1: raw bytes of "hello.txt" ---------------------------|
|--- Payload 2: raw bytes of "cat.png" -----------------------------|
|--- Table of Contents (TOC) ---------------------------------------|
|    - FileEntry 1 (path="hello.txt", offset=0, size=14, perms)     | -> exactly 276 bytes
|    - FileEntry 2 (path="cat.png", offset=14, size=4096, perms)    | -> exactly 276 bytes
|--- EndRecord (Trailer) -------------------------------------------|
|    - magic, version, directory_offset, entry_count                | -> fixed byte size
[ END OF FILE ]

When reading an archive:
1) Go straight to the end minus sizeof(EndRecord)
2) Read EndRecord. Verify the magic number matches
3) Use directory_offset to jump straight to the TOC
4) Read each 276-byte FileEntry. Now we know every file's name,size,permission
and where its payload starts(data_offset)



*/