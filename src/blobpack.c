#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "blob.h"
typedef struct
{
  char *part_name;
  char *filename;
} partition_item;

// Number of required arguments before partition definition, including argv[0]
#define GENERIC_ARGS 3

int
main (int argc, char **argv)
{
  header_type hdr;
  memset (&hdr, 0, sizeof (header_type));

  if (argc < (GENERIC_ARGS+2)) // Require at least one partition
    {
      fprintf (stderr,"Usage: %s <headerfile> <outfile> <partitionname> <partitionfile> ...\n", argv[0]);
      fprintf(stderr, "Any number of partitionname partitionfilename entries can be entered\n");
      return -1;
    }
  char *headername = argv[1];
  char *outname = argv[2];
  int i;
  int partnums = argc - GENERIC_ARGS; 

  if(partnums <= 0 || partnums % 2 != 0)
  {
    fprintf(stderr, "Error in parameters. There needs to be equal partition names and partition filenames.");
    return -1;
  }
  // Two parameters per partition. 
  // At this point we know there is a dividable-by-two number of parameters left
  partnums = partnums / 2;
  printf("Found %d partitions as commandline arguments\n", partnums);
  partition_item *partitions = calloc(partnums, sizeof(partition_item));
  partition_item *curr_part = partitions;
  for(i=GENERIC_ARGS; i<argc; i+=2)
  {
    printf("Partname: %s Filename: %s\n", argv[i], argv[i+1]);
    curr_part->part_name = argv[i];
    curr_part->filename = argv[i+1];
    curr_part++;
  };

  FILE *hdrfil = fopen (headername, "r");
  fread (&hdr, sizeof (header_type), 1, hdrfil);
  fclose(hdrfil);
  hdrfil = NULL;
  FILE *outfile = fopen (outname, "w");
  hdr.num_parts = partnums;
  fwrite (&hdr, sizeof (header_type), 1, outfile);
  if (!memcmp (hdr.magic, MAGIC, MAGIC_SIZE))
    {
      printf ("Correct header\n");
    }
  printf ("Size: %d\n", hdr.size);
  printf ("%d partitions starting at offset 0x%X\n", hdr.num_parts,
	  hdr.part_offset);

  part_type *parts = calloc (hdr.num_parts, sizeof (part_type));
  memset(parts, 0, sizeof(part_type)*hdr.num_parts);
  int currentOffset = sizeof(header_type)+sizeof(part_type)*hdr.num_parts; // Is this right? maybe +1
  printf("Offset: %d\n", currentOffset);
  for (i = 0; i < hdr.num_parts; i++)
    {
      memcpy(parts[i].name, partitions[i].part_name, PART_NAME_LEN);
      parts[i].unknown = 1; // No idea, but seems to always be 1...
      parts[i].offset = currentOffset;
      FILE *curfile = fopen (partitions[i].filename, "r");
      if(curfile == NULL)
      {
        fprintf(stderr,"Error opening file %s\n", partitions[i].filename);
        return 0;
      }
      fseek (curfile, 0, SEEK_END);
      long fsize = ftell (curfile);
      fclose (curfile);
      parts[i].size = fsize;
      currentOffset += fsize;
    }

  fwrite (parts, sizeof (part_type), hdr.num_parts, outfile);
  for (i = 0; i < hdr.num_parts; i++)
    {
      char *buffer = (char *) malloc (parts[i].size);
      FILE *currFile = fopen (partitions[i].filename, "r");	// Read in update file
      fread (buffer, 1, parts[i].size, currFile);
      fclose(currFile);
      fwrite (buffer, 1, parts[i].size, outfile);
    };

  fclose (outfile);
  return 0;
}
