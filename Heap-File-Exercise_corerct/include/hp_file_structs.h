#ifndef HP_FILE_STRUCTS_H
#define HP_FILE_STRUCTS_H

#include <record.h>

/**
 * @file hp_file_structs.h
 * @brief Data structures for heap file management
 */

/* -------------------------------------------------------------------------- */
/*                              Data Structures                               */
/* -------------------------------------------------------------------------- */

/**
 * @brief Heap file header containing metadata about the file organization
 */
typedef struct HeapFileHeader {
    unsigned int size_of_record ;
    unsigned int records_per_block;
    unsigned int last_free_record;

} HeapFileHeader;

/**
 * @brief Iterator for scanning through records in a heap file
 */
typedef struct HeapFileIterator{
    int file_handle;
    HeapFileHeader *header_info;
    int id;                     // the id we are looking for
    unsigned int current_block; // block 0 is header
    int current_record;         // the record in the current block
} HeapFileIterator;

#endif /* HP_FILE_STRUCTS_H */