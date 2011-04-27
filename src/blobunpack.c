#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "blob.h"

void dumpPartition(FILE *fil, part_type part);

int main(int argc, char **argv)
{
header_type hdr;
memset(&hdr, 0, sizeof(header_type));

if(argc < 2)
  return 0;

FILE *fil = fopen(argv[1], "r");
fread(&hdr, sizeof(header_type),  1, fil);
if(!memcmp(hdr.magic, MAGIC, MAGIC_SIZE))
{
  printf("Correct header\n");
}
printf("Size: %d\n", hdr.size);
printf("%d partitions starting at offset 0x%X\n", hdr.num_parts, hdr.part_offset);

fseek(fil, hdr.part_offset, SEEK_SET);
part_type *parts = calloc(hdr.num_parts, sizeof(part_type));
fread(parts, sizeof(part_type), hdr.num_parts, fil);
int i;
for(i=0;i<hdr.num_parts; i++)
{
  printf("Partition %d\n",i);
  printf("Name: %s\n", parts[i].name);
  printf("ImageOffset: %d (0x%X)\n", parts[i].offset, parts[i].size);
  printf("ImageLength: %d (0x%X)\n", parts[i].size, parts[i].size);
  dumpPartition(fil, parts[i]);
}
fclose(fil);
return 0;
}

#define BUFFER_SIZE 2048
void dumpPartition(FILE *fil, part_type part)
{
fseek(fil, part.offset, SEEK_SET);
int dataleft = part.size;
char buffer[BUFFER_SIZE];
printf("Writing file %s\n", part.name);
printf("FileSize: %d bytes\n", part.size);
FILE* outfile = fopen(part.name, "w");
while(dataleft > 0)
{
  int toRead = dataleft > BUFFER_SIZE ? BUFFER_SIZE : dataleft;
  int dataread = fread(buffer, 1,toRead, fil);
  int datawritten = fwrite(buffer, 1,dataread, outfile);
  if(dataread != datawritten)
   abort();
  dataleft -= dataread;
}
fclose(outfile);
}
