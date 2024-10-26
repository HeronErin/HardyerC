#pragma once
#include "patch_resolver.h"
#include "string.h"
#include "common/defs.h"


struct LoadedFile{
    // An OWNED string to the contents of the file
   PatchString contents;
    
    // Max size path for file path on windows (PLUS NULL)
    char real_path[261];
};

typedef struct {
    // Array of type struct LoadedFile
    Array loaded_files;
    



} TranslationParserState;


// TAKES OWNERSHIP OF contents param
static inline int push_file(TranslationParserState* tsc, PatchString contents, char* name){
    int index = tsc->loaded_files.size / sizeof(struct LoadedFile);
    size_t name_len = strlen(name);
    debug_assert(260 >= name_len);


    struct LoadedFile* new_file = array_push_unallocated(&tsc->loaded_files,  sizeof(struct LoadedFile));

    array_force_null_terminate(&contents.internal_string);
    array_freeze(&contents.internal_string);
    array_freeze(&contents.patches);
    memcpy(&new_file->real_path, name, name_len + 1);
    new_file->contents = contents;
    

    return index;
}








