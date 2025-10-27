#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bf.h"
#include "hp_file_structs.h"
#include "record.h"

#define CALL_BF(call, retval)       \
{                                   \
  BF_ErrorCode code = call;         \
  if (code != BF_OK) {              \
    BF_PrintError(code);            \
    return retval;                  \
  }                                 \
}

int HeapFile_Create(const char* fileName)
{

/**
 * @brief Creates a new heap file with initialized header
 *
 * @param fileName Name of the file to create
 * @return 1 on success, 0 on failure
 */

  void* data;

  int file_handle;

  //creating and openning the file to store file header
  CALL_BF(BF_CreateFile(fileName),0);
  
  CALL_BF(BF_OpenFile(fileName, &file_handle),0); 

  BF_Block *block;

  BF_Block_Init(&block);

  //allocate block to write our first data
  CALL_BF(BF_AllocateBlock(file_handle, block),0);

  //Getting data pointer to put file header info in file
  data = BF_Block_GetData(block); 

  HeapFileHeader * rec = data;  

  //initializing header info
  rec->size_of_record =  sizeof(Record);
  rec->records_per_block = BF_BLOCK_SIZE/sizeof(Record);
  rec->last_free_record = 0;

  /*since we changed the block set it dirty
  and unpin it from buffer to reuse memory*/
  BF_Block_SetDirty(block);
  CALL_BF(BF_UnpinBlock(block),0);

  //free block memory and close file
  BF_Block_Destroy(&block);
  CALL_BF(BF_CloseFile(file_handle),0);  

  return 1;
}

//global variable for how many files are opened at the same time 
int Opened = 0;

int HeapFile_Open(const char *fileName, int *file_handle, HeapFileHeader** header_info)
{

  //if we have exceeded opened files limit then print error
  if (Opened >= BF_MAX_OPEN_FILES){
      printf("Error: Maximum number of open files reached\n");
      return 0;
  }

  //open file
  CALL_BF(BF_OpenFile(fileName, file_handle), 0);

  //store header in buffer
  BF_Block * block;

  BF_Block_Init(&block);              

  // get block 0 (header)
  CALL_BF(BF_GetBlock(*file_handle, 0, block), 0);

  // temporary pointer to HeapFileHeader
  HeapFileHeader *temp = (HeapFileHeader *) BF_Block_GetData(block);

  //store header in heap in order to unpin block and free memory block
  *header_info = malloc(sizeof(HeapFileHeader));
  **header_info = *temp;


  CALL_BF(BF_UnpinBlock(block), 0);
  BF_Block_Destroy(&block);

  //if opened successfully increase counter
  Opened += 1;

  return 1;

}

int HeapFile_Close(int file_handle, HeapFileHeader * hp_info)
{
    //free memory used in malloc for header
    free(hp_info);

    //close file and decrease the counter for opened files
    CALL_BF(BF_CloseFile(file_handle), 0);
    Opened -= 1;

    return 1;
}



int HeapFile_InsertRecord(int file_handle, HeapFileHeader *hp_info, const Record record)
{

  void * data;

  int blocks_num;

  BF_Block * block;

  BF_Block_Init(&block);   
  
  
  CALL_BF(BF_GetBlockCounter(file_handle, &blocks_num),0);

  /*if we have only the header block or the index of records in our last block
  exceeds it's capacity then we have to allocate a new block increase the number
  of blocks in buffer and reinitialize last_free_record to 0*/
  if (blocks_num == 1 || hp_info->last_free_record > hp_info->records_per_block - 1 )
  {
    CALL_BF(BF_AllocateBlock(file_handle, block),0);
    
    blocks_num += 1;

    hp_info->last_free_record = 0;

  }

  /*take the data of last block to store the new record in our free 
  spot and then increase the index of records in the current block*/
  CALL_BF(BF_GetBlock(file_handle, blocks_num-1, block), 0);  

  data = BF_Block_GetData(block);  

  Record * rec = data;                         

  rec[hp_info->last_free_record] = record;

  hp_info->last_free_record +=1;

  BF_Block_SetDirty(block);

  CALL_BF(BF_UnpinBlock(block), 0);

  BF_Block_Destroy(&block);

  return 1;
}


HeapFileIterator HeapFile_CreateIterator(int file_handle, HeapFileHeader* header_info, int id)
{
  HeapFileIterator out;

  out.file_handle = file_handle;
  out.header_info = header_info;
  out.id = id;
  out.current_block = 1; // block 0 is header data starts at block 1
  out.current_record = 0;
  return out;
}

int HeapFile_GetNextRecord(HeapFileIterator *heap_iterator, Record **record)
{
  *record = NULL;
  BF_Block *block;
  Record *current_record;
  char *data;
  unsigned int number_of_blocks;
  CALL_BF(BF_GetBlockCounter(heap_iterator->file_handle, &number_of_blocks), 0);
  
  do
  {
    BF_Block_Init(&block);
    CALL_BF(BF_GetBlock(heap_iterator->file_handle, heap_iterator->current_block, block), 0);
    data = BF_Block_GetData(block);

    

    current_record = (Record *)(data + heap_iterator->current_record * heap_iterator->header_info->size_of_record);
    
    if (current_record->id == heap_iterator->id)
    {
      *record = current_record;
    }

    if (heap_iterator->current_record  == heap_iterator->header_info->records_per_block -1)
    {
      if (heap_iterator->current_block  == number_of_blocks - 1)
      {
         CALL_BF(BF_UnpinBlock(block), 0);
        BF_Block_Destroy(&block);
        return 0;
      }
      heap_iterator->current_block += 1;
      heap_iterator->current_record = 0;
    }
    else
    {
      heap_iterator->current_record += 1;
    }
    
  } while (*record == NULL);

   CALL_BF(BF_UnpinBlock(block), 0);
  BF_Block_Destroy(&block);

  return 1;
}
