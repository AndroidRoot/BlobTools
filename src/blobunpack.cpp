#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "blob.h"

#ifdef _WIN32
#define snprintf _snprintf
#endif
void dumpPartition (FILE * file, char *basename, part_type part, bool bSignOffset);
#define BUFFER_SIZE 2048

int
main (int argc, char **argv)
{
  header_type hdr;
  FILE *file;
  part_type *parts;
  int i;

  memset (&hdr, 0, sizeof (header_type));

  if (argc < 2)
    {
      fprintf (stderr, "Usage: %s <blobfile>\n", argv[0]);
      return -1;
    }

  file = fopen (argv[1], "rb");
  if (file == NULL)
    {
      fprintf (stderr, "Unable to open \"%s\"\n", argv[1]);
      return -1;
    }

  bool bSignOffset = false;
  fread (&hdr.magic, sizeof (header_type)-SIGNBLOB_SIZE, 1, file);	//Read the original way (ignore the new variable)
  if (memcmp (hdr.magic, MAGIC, MAGIC_SIZE))
    {
      fseek (file, 0, SEEK_SET);
	  fread (&hdr, sizeof (header_type), 1, file); 	//Read the new way (read in the sign so it is skipped)
      if (memcmp (hdr.magic, MAGIC, MAGIC_SIZE))
        {
          fprintf (stderr, "File \"%s\" is not a valid blob file\n", argv[1]);
          return -1;
        }
      printf ("Header contains SIGNBLOB\n");
      bSignOffset = true;
    }
  printf ("Header size: %d\n", hdr.size);
  printf ("%d partitions starting at offset 0x%X\n", hdr.num_parts, hdr.part_offset);
  printf ("Blob version: %d\n", hdr.version/0x10000);
  for(i=0; i<7; i++)
  {
    printf ("Blob 'unknown' %d: %d\n", i, hdr.unknown[i]);
  }
  
/*  snprintf (hdrfilename, 1024, "%s.HEADER", argv[1]);
  hdrfile = fopen (hdrfilename, "wb");
  fwrite (&hdr, sizeof (header_type), 1, hdrfile);
  fclose (hdrfile);*/


  fseek (file, hdr.part_offset+(bSignOffset?SIGNBLOB_SIZE:0), SEEK_SET);
  parts = (part_type *)calloc (hdr.num_parts, sizeof (part_type));
  fread (parts, sizeof (part_type), hdr.num_parts, file);

  for (i = 0; i < (int)hdr.num_parts; i++)
    {
      printf ("Partition %d\n", i);
      printf ("Name: %s\n", parts[i].name);
      printf ("Offset: %d (0x%X)\n", parts[i].offset, parts[i].offset);
      printf ("Size: %d (0x%X)\n", parts[i].size, parts[i].size);
      dumpPartition (file, argv[1], parts[i], bSignOffset);
    }
  fclose (file);
  return 0;
}

void
dumpPartition (FILE * file, char *basename, part_type part, bool bSignOffset)
{
  fseek (file, part.offset+(bSignOffset?SIGNBLOB_SIZE:0), SEEK_SET);
  int dataleft = part.size;
  char buffer[BUFFER_SIZE];
  char filename[1024];
  FILE *outfile;
  snprintf (filename, 1024, "%s.%s", basename, part.name);
  printf ("Writing file %s (%d bytes)\n", filename, part.size);
  outfile = fopen (filename, "wb");

  while (dataleft > 0)
    {
      int toRead = dataleft > BUFFER_SIZE ? BUFFER_SIZE : dataleft;
      int dataread = fread (buffer, 1, toRead, file);
      int datawritten = fwrite (buffer, 1, dataread, outfile);
      if (dataread != datawritten || (0 == dataread && 0 < dataleft))
      {
		abort ();
      }
      dataleft -= dataread;
    }
  fclose (outfile);
}
