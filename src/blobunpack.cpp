
/*
* Copyright 2012 Jens Andersen/AndroidRoot.mobi
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*   limitations under the License.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "blob.h"
#ifdef _WIN32
#define snprintf _snprintf
#endif
void dumpPartition (FILE * file, char *basename, part_type part);
#define BUFFER_SIZE 2048

int secure_offset;

int
main (int argc, char **argv)
{
  secure_header_type sec_hdr;
  header_type *hdr;
  FILE *file;
  part_type *parts;
  int i;
  char magic_tag[21];
  memset (&sec_hdr, 0, sizeof (secure_header_type));
  hdr = &sec_hdr.real_header;
  secure_offset=0;

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

   fread (magic_tag, SECURE_MAGIC_SIZE, 1, file);
   if(!memcmp(magic_tag, SECURE_MAGIC, SECURE_MAGIC_SIZE))
   {
      fseek(file, 0 , SEEK_SET);
      fread (&sec_hdr, sizeof (secure_header_type), 1, file);
      secure_offset = SECURE_OFFSET;
   } else if (!memcmp (magic_tag, MAGIC, MAGIC_SIZE))
   {      
      fseek(file, 0 , SEEK_SET);
      fread (hdr, sizeof (header_type), 1, file);
   } else
  {
	fprintf(stderr, "Unsupport blob file format\n");
	exit(-1);
  }
  printf ("Header size: %d\n", hdr->size);
  printf ("%d partitions starting at offset 0x%X\n", hdr->num_parts,
	  hdr->part_offset);
  printf ("Blob version: %d\n", hdr->version);
  
  fseek (file, secure_offset + hdr->part_offset, SEEK_SET);
  parts = (part_type *)calloc (hdr->num_parts, sizeof (part_type));
  fread (parts, sizeof (part_type), hdr->num_parts, file);

  for (i = 0; i < (int)hdr->num_parts; i++)
    {
      printf ("Partition %d\n", i);
      printf ("Name: %s\n", parts[i].name);
      printf ("Offset: %d (0x%X)\n", parts[i].offset, parts[i].offset);
      printf ("Size: %d (0x%X)\n", parts[i].size, parts[i].size);
      dumpPartition (file, argv[1], parts[i]);
    }
  fclose (file);
  return 0;
}

void
dumpPartition (FILE * file, char *basename, part_type part)
{
  int dataleft = part.size;
  char buffer[BUFFER_SIZE];
  char filename[1024];
  FILE *outfile;
  snprintf (filename, 1024, "%s.%s", basename, part.name);
  printf ("Writing file %s (%d bytes)\n", filename, part.size);

  fseek (file, secure_offset + part.offset, SEEK_SET);
  outfile = fopen (filename, "wb");
  while (dataleft > 0)
    {
      int toRead = dataleft > BUFFER_SIZE ? BUFFER_SIZE : dataleft;
      int dataread = fread (buffer, 1, toRead, file);
      int datawritten = fwrite (buffer, 1, dataread, outfile);
      if (dataread != datawritten)
		abort ();
      dataleft -= dataread;
    }
  fclose (outfile);
}
