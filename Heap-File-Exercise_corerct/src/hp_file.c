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

  CALL_BF(BF_CreateFile(fileName),0);
  
  CALL_BF(BF_OpenFile(fileName, &file_handle),0); 

  BF_Block *block;

  BF_Block_Init(&block);
  
  CALL_BF(BF_AllocateBlock(file_handle, block),0);

  //GETTING DATA POINTER TO PUT FILE HEADER INFO 
  data = BF_Block_GetData(block); 

  HeapFileHeader * rec = data;  

  rec->size_of_record =  sizeof(Record);
  rec->records_per_block = BF_BLOCK_SIZE/sizeof(Record);
  rec->last_free_block = 0 ;
  rec->last_free_record = 0;
  
 

  BF_Block_SetDirty(block);

  CALL_BF(BF_UnpinBlock(block),0);

  BF_Block_Destroy(&block);

  CALL_BF(BF_CloseFile(file_handle),0);  

  return 1;
}

int HeapFile_Open(const char *fileName, int *file_handle, HeapFileHeader** header_info)
{

  CALL_BF(BF_OpenFile(fileName, file_handle), 0);

  BF_Block * block;

  BF_Block_Init(&block);              

  CALL_BF(BF_GetBlock(*file_handle, 0, block), 0);  // get block 0 (header)

  *header_info = (HeapFileHeader *) BF_Block_GetData(block);  // pointer to HeapFileHeader

  BF_Block_Destroy(&block);

  return 1;

}

int HeapFile_Close(int file_handle, HeapFileHeader * hp_info)
{
  

  BF_Block * block;

  BF_Block_Init(&block);

  for (int i = 0;i < hp_info->last_free_block;i++ ){
    
    CALL_BF(BF_GetBlock(file_handle, i, block), 0);  // get block 0 (header)
    CALL_BF(BF_UnpinBlock(block), 0);
    
  }

  BF_Block_Destroy(&block);
  

  CALL_BF(BF_CloseFile(file_handle),0);


 return 1;
}


int HeapFile_InsertRecord(int file_handle, HeapFileHeader *hp_info, const Record record)
{

  BF_Block * block;

  BF_Block_Init(&block);              

  void * data;


  if (hp_info->last_free_block == 0) {
    printf("Initializing 1st block \n");
    
    CALL_BF(BF_AllocateBlock(file_handle, block),0);

    hp_info->last_free_block += 1;

  }else{
    if (hp_info->last_free_record >= hp_info->records_per_block) {
      printf("Allocating new block \n");
      
      CALL_BF(BF_AllocateBlock(file_handle, block),0);

     
      hp_info->last_free_record = 0;
      hp_info->last_free_block += 1;

    }
  }
  

  CALL_BF(BF_GetBlock(file_handle, hp_info->last_free_block, block), 0);  

  data = BF_Block_GetData(block);  

  Record* rec = data;                         // Ο δείκτης rec δείχνει στην αρχή της περιοχής μνήμης data
  
  rec[hp_info->last_free_record] = record;

  hp_info->last_free_record += 1;


 // printRecord(rec[hp_info->last_free_record]);

  BF_Block_SetDirty(block);

  CALL_BF(BF_UnpinBlock(block), 0);
  
  BF_Block_Destroy(&block);

  return 1;
}


HeapFileIterator HeapFile_CreateIterator(    int file_handle, HeapFileHeader* header_info, int id)
{
  HeapFileIterator out;
  return out;
}


int HeapFile_GetNextRecord(    HeapFileIterator* heap_iterator, Record** record)
{
    * record=NULL;
    return 1;
}

