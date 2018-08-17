// used for keeping track of number of memory allocations and deallocations
// created by Silent Akufishi

#ifndef SILENT_H
#define SILENT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <math.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "LinkedList.h"
#include "MemoryDebug.h"

#define SLURP_SIZE           0xffff
#define SILENT_ENCRYPTION(x) ((x+42)^69420)
#define SILENT_DECRYPTION(x) ((x^69420)-42)

#define min(x,y)             (x<y?x:y)
#define max(x,y)             (x>y?x:y)
#define clamp(x,mx,mn)       min(mx,max(x,mn))

#define ror(x,steps)         (x>>steps)|(x<<(sizeof(x)*8-steps))
#define rol(x,steps)         (x<<steps)|(x>>(sizeof(x)*8-steps))

//#define DEBUG

typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;

/********************************/
/*FILE LAYOUT*/
/********************************/

/////////
//silent package layout
/////////
//3 bytes - signature
//1 byte  - encryption switch
//4 bytes - key
//4 bytes - num of files
//4 bytes - num of folders
//array of 2 bytes - size of file name
//array of 8 bytes - size of files
//array of 2 bytes - size of folder names
//array of folder names
//array of folder + file name + 2 bytes attributes + data

/********************************/
/*STRUCTURES*/
/********************************/

//directory entry
typedef struct {
    int   type;
    char  path[PATH_MAX];
} DirEntry;

//Silent interface
typedef struct {
    u64   offset;
    u64   size;
    u16   attributes;
    char  name[PATH_MAX];
} SilentNode;

typedef struct {
    LinkedList* nodes;
    bool encryption;
    char key[4];
    FILE* file;
} SilentInterface;

/********************************/
/*PROTOTYPES*/
/********************************/

//reads the header and creates the silent interface
SilentInterface* new_interface (char* input);
//unpacks whole silent package
void unsilence          (char* input, const char* output, char* password);
//unpacks certain file from silent package
void unsilence_specific (SilentInterface* interface, u64 index, char* folder, char* password);
//creates silent package + optional ecryption
void silence            (char* folder, const char* output, bool encryption, char* password);
//ecrypted write
void ewrite             (char* buffer, u64 size, bool encryption, FILE* f);
//decrypted read
void eread              (char* buffer, u64 size, bool encryption, FILE* f);
//get directory entires
int getEntries          (char* dir, char* super_folder, LinkedList* files);
//create directory
int makedir             (const char* path);
//deallocate silent interface
void free_interface     (SilentInterface* interface);
//create new silent node
SilentNode* newSNode    (char* name, u64 offset, u64 size, u16 attributes);
//get file size
u64 fsize               (FILE* f);
//get file attributes
u16 get_attributes      (char* path);
//set file attributes
void set_attributes     (char* path, u32 flags);
//reverse bits in byte
u8 reverse              (u8 x);
//perform a lookup 3 hash for password correction
u32 lookup3(void* data, u32 length, u32 init);
//easier access to lookup3 hash function
u32 lookup3hash(char* data, u32 length);
#endif
