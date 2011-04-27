#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "blob.h"

int
main (int argc, char **argv)
{
  header_type hdr;
  memset (&hdr, 0, sizeof (header_type));

  if (argc < 3)
    {
      printf ("Usage: %s <blobname> <outfile>\n", argv[0]);
      return 0;
    }
  char *blobname = argv[1];
  char *outname = argv[2];

  FILE *fil = fopen (blobname, "r");
  FILE *outfile = fopen (outname, "w");

  fread (&hdr, sizeof (header_type), 1, fil);
  fwrite (&hdr, sizeof (header_type), 1, outfile);
  if (!memcmp (hdr.magic, MAGIC, MAGIC_SIZE))
    {
      printf ("Correct header\n");
    }
  printf ("Size: %d\n", hdr.size);
  printf ("%d partitions starting at offset 0x%X\n", hdr.num_parts,
	  hdr.part_offset);
  if (sizeof (header_type) != hdr.part_offset)	// If the partitions don't start exactly after the header, it's an unknown type possibly?
    {
      printf ("Error?");
      return 0;
    }

  fseek (fil, hdr.part_offset, SEEK_SET);
  part_type *parts = calloc (hdr.num_parts, sizeof (part_type));
  fread (parts, sizeof (part_type), hdr.num_parts, fil);
  int i;
  int currentOffset = parts[0].offset;
  for (i = 0; i < hdr.num_parts; i++)
    {
      printf ("Partition %d\n", i);
      printf ("Name: %s\n", parts[i].name);
      parts[i].offset = currentOffset;	// Update offset  in case previous files have changed size
      FILE *curfile = fopen (parts[i].name, "r");
      fseek (curfile, 0, SEEK_END);
      long fsize = ftell (curfile);
      fclose (curfile);
      parts[i].size = fsize;	// Update size
      currentOffset += fsize;
    }
  fwrite (parts, sizeof (part_type), hdr.num_parts, outfile);
  for (i = 0; i < hdr.num_parts; i++)
    {
      char *buffer = (char *) malloc (parts[i].size);
      FILE *currFile = fopen (parts[i].name, "r");	// Read in update file
      fread (buffer, 1, parts[i].size, currFile);
      fwrite (buffer, 1, parts[i].size, outfile);
    };

  fclose (fil);
  fclose (outfile);
  return 0;
}
