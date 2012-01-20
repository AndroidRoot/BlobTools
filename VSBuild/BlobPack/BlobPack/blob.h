#ifndef _BLOB_H
#define _BLOB_H

// Previous unknown fields grabbed from 
// http://nv-tegra.nvidia.com/gitweb/?p=android/platform/build.git;a=commitdiff;h=0adc4478615891636d8b7c476c20c2014b788537

#define SIGNBLOB "\x2d\x53\x49\x47\x4e\x45\x44\x2d\x42\x59\x2d\x53\x49\x47\x4e\x42\x4c\x4f\x42\x2d\x29\xc3\x94\x20\x00\x01\x00\x00"	//"-SIGNED-BY-SIGNBLOB-)Ô "
#define SIGNBLOB_SIZE 28
#define MAGIC       "MSM-RADIO-UPDATE"
#define MAGIC_SIZE  16
#define PART_NAME_LEN 4

typedef struct
{
    unsigned char signblob[SIGNBLOB_SIZE];
    unsigned char magic[MAGIC_SIZE];
    unsigned int version; // Always 0x00010000
    unsigned int size; // Size of header
    unsigned int part_offset; // Same as size
    unsigned int num_parts; // Number of partitions
    unsigned int unknown[7]; // Always zero
} header_type;

typedef struct
{
    char name[PART_NAME_LEN]; // Name of partition. Has to match an existing tegra2 partition name (e.g. LNX, SOS)
    unsigned int offset; // offset in blob where this partition starts
    unsigned int size; // Size of partition
    unsigned int version; // Version is variable, but is always 1 in this app
} part_type;

#endif /* _BLOB_H*/
