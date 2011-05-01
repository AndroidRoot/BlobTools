#ifndef _BLOB_H
#define _BLOB_H

#define MAGIC       "MSM-RADIO-UPDATE"
#define MAGIC_SIZE  16
#define PART_NAME_LEN 4

typedef struct
{
    unsigned char magic[MAGIC_SIZE];
    unsigned char unknown1[4];
    unsigned int size;
    unsigned int part_offset;
    unsigned int num_parts;
    unsigned char unknown2[28];
} header_type;

typedef struct
{
    char name[PART_NAME_LEN];
    unsigned int offset;
    unsigned int size;
    unsigned int unknown; // Seems to always be 1.
} part_type;

#endif /* _BLOB_H*/
