/*
** Copyright (C) 2004 by Wired Team and Robert Melby
**  
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation; either version 2.1 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
** 
** You should have received a copy of the GNU Lesser General Public License
** along with this program; if not, write to the Free Software 
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

/*
  Some parts of this file were based on abrowse program by Robert Melby
  (http://abrowse.sourceforge.net) 
*/

#include "akai.h"

const t_akaiType	kAkaiTypes[] = {
	{  1, "S1000 VOLUME"},
	{  3, "S3000 VOLUME"},
	{100, "DRUM SETTING"},
	{112, "S1000 PROGRAM"},
	{113, "QLIST"},
	{115, "S1000 SAMPLE"},
	{120, "EFFECTS"},
	{237, "MULTI"},
	{240, "S3000 PROGRAM"},
	{243, "S3000 SAMPLE"},
	{0, 0}
};

static long long offset;
	
void		akaiToAscii(char *, int);

short		akaiReadFat(int fd, int block) 
{ 
  short 	value; 

  lseek(fd, offset + FAT_OFFSET + block * 2, SEEK_SET); 
  read(fd, &value, 2); 
#ifdef WORDS_BIGENDIAN
  value = bswap_16(value);
#endif
  return (value);
}

void		akaiReadBlock(int fd, int block, void *buffer) 
{
  lseek(fd, offset + block * BLOCK_SIZE, SEEK_SET);
  read(fd, buffer, BLOCK_SIZE);
}


t_akaiDirent	*akaiReadEntry(int fd, int block, int pos)
{ 
  char 		*buffer;
  t_akaiDirent	*rec; 

  buffer = (char *)malloc(sizeof(char) * 30); 
  rec = (t_akaiDirent *)malloc(sizeof(t_akaiDirent)); 
  
  rec->subdirs = 0;
  if (block == ROOT_ENTRY_OFFSET) 
  { 
    lseek(fd, offset + DIR_ENTRY_OFFSET + 
	      pos * DIR_ENTRY_SIZE, SEEK_SET);
    read(fd, buffer, DIR_ENTRY_SIZE);
    rec->type = (unsigned char)buffer[12] | ((unsigned char)buffer[13] << 8); 
    rec->offset = (unsigned char)buffer[14] | ((unsigned char)buffer[15] << 8); 
    rec->size = 0; 
    akaiToAscii(buffer, 12);
    rec->name = buffer;
    return (rec);
  }
  if (pos < 341)
    lseek(fd, offset + block * BLOCK_SIZE + 
	      pos * FILE_ENTRY_SIZE, SEEK_SET);
  else
    lseek(fd, offset + akaiReadFat(fd, block) * BLOCK_SIZE + 
	      (pos - 341) * FILE_ENTRY_SIZE, SEEK_SET);
  read(fd, buffer, FILE_ENTRY_SIZE);
  rec->type = (unsigned char)buffer[16]; 
  rec->offset = (unsigned char)buffer[20] | 
  		((unsigned char)buffer[21] << 8); 
  rec->size = (unsigned char)buffer[17] | 
	      ((unsigned char)buffer[18] << 8) |
	      ((unsigned char)buffer[19] << 16); 
  akaiToAscii(buffer, 12);
  rec->name = buffer; 
  return (rec); 
} 

t_list		*akaiReadAllEntries(int fd, int block) 
{ 
  int		i;
  int		fat; 
  t_akaiDirent	*rec; 
  int		nentries;
  t_list	*entries; 

  entries = 0;
  if (block == ROOT_ENTRY_OFFSET) 
    nentries = MAX_DIR_ENTRIES;
  else
    nentries = (akaiReadFat(fd, block) == FAT_MARK_RESERVED) ?
      		MAX_FILE_ENTRIES_S1000 : MAX_FILE_ENTRIES_S3000;
  for (i = 0; i < nentries; i++)
  {
    rec = akaiReadEntry(fd, block, i); 
    if ((rec->type) && (strlen(rec->name)))
      listAppend(&entries, rec);
    else
    {
      free(rec->name); 
      free(rec);
    }
  }
  return (entries);
}

t_akaiDirent	*akaiFindRecord(t_list *list, char *name)
{ 
  for (; list; list = list->next)
    if (!strcmp(name, elt(list, t_akaiDirent *)->name))
      return (list->elem);
  return (0);
}
 
t_list		*akaiReadDir(int fd, char *dir) 
{ 
  t_list	*list;
  t_akaiDirent	*rec;

  if ((!strcmp(dir, "/")) || (!strcmp(dir, "")))
    return (akaiReadAllEntries(fd, ROOT_ENTRY_OFFSET));
  list = akaiReadDir(fd, "/");
  rec = akaiFindRecord(list, dir); 
  if (rec)
    return (akaiReadAllEntries(fd, rec->offset));
  return (0);
}

char		*akaiGetType(int type) 
{ 
  char		*s;
  int		i;

  for (i = 0; (kAkaiTypes[i].typeName) && (kAkaiTypes[i].typeVal != type); i++) ;
  if (kAkaiTypes[i].typeName)
    return (kAkaiTypes[i].typeName);
  asprintf(&s, "%i", type);
  return (s);
}

int		akaiSelectPart(int fd, int part) 
{ 
  unsigned short size; 
  
  offset = 0; 
  while (part > 0) 
  {
    lseek(fd, offset, SEEK_SET);
    read(fd, &size, 2);
#ifdef WORDS_BIGENDIAN
    size = bswap_16(size);
#endif
    if ((size == 0x0fff) || (size == 0xffff) ||
        (size == PART_END_MARK) || (!size))
      return (-1);
    offset += size * BLOCK_SIZE;
    part--;
  }
  return (1);
}

void		akaiToAscii(char *buffer, int length) 
{ 
  int		i;

  for (i = 0; i < length; i++)
  { 
    switch (buffer[i])
    { 
      case 0:  
      case 1: 
      case 2:
      case 3:   
      case 4:
      case 5: 
      case 6: 
      case 7:
      case 8: 
      case 9: 
        buffer[i] += 48;
        break; 
      case 10:
        buffer[i] = 32; 
        break;
      case 11: 
      case 12: 
      case 13:
      case 14: 
      case 15: 
      case 16:
      case 17: 
      case 18: 
      case 19:
      case 20: 
      case 21:
      case 22:
      case 23:
      case 24:
      case 25:
      case 26:
      case 27:
      case 28:
      case 29: 
      case 30: 
      case 31:
      case 32:
      case 33:
      case 34:
      case 35:
      case 36:
        buffer[i] = 64 + (buffer[i] - 10);
        break; 
      default: 
        buffer[i] = 32;
    }
  }
  for (; buffer[length - 1] == 32; length--) ;
  buffer[length] = '\0';
}

int		akaiGetPart(char * part) 
{ 
  return (toupper(part[0]) - 64); 
}

t_akaiProgram	*akaiOpenProg(char *buffer, int size) 
{ 
  t_akaiProgram	*new_prog;
  t_akaiKeygrp	*new_keygrp;
  int		prog;
  int		chan;
  int		lowkey;
  int		highkey;
  int		nkeygrp;
  int		tune; 
  int		i; 
  short		next_keygroup;

  new_prog = (t_akaiProgram *)malloc(sizeof(t_akaiProgram)); 
  new_prog->name = (char *)malloc(sizeof(char) * 14); 
  akaiToAscii(buffer + 3, 12);
  strcpy(new_prog->name, buffer + 3);
  next_keygroup = (unsigned char)buffer[1] | 
	          ((unsigned char)buffer[2] << 8);
  if (next_keygroup != 150) 
    next_keygroup = 192;
  new_prog->num = buffer[0x0f];
  new_prog->channel = buffer[0x10];
  new_prog->lowkey = buffer[0x13];
  new_prog->highkey = buffer[0x14];
  new_prog->nkeygrps = buffer[0x2a];
  new_prog->keygrp = 0;
  for (i = 0; i < new_prog->nkeygrps; i++)
  { 
    new_keygrp = (t_akaiKeygrp *)malloc(sizeof(t_akaiKeygrp)); 
    listAppend(&new_prog->keygrp, new_keygrp);
    new_keygrp->num = i; 
    new_keygrp->zone[0] = (char *)malloc(14);
    new_keygrp->zone[1] = (char *)malloc(14); 
    new_keygrp->zone[2] = (char *)malloc(14); 
    new_keygrp->zone[3] = (char *)malloc(14); 
    new_keygrp->lowkey = buffer[next_keygroup + i * next_keygroup + 0x3];
    new_keygrp->highkey = buffer[next_keygroup + i * next_keygroup + 0x4];
    new_keygrp->tune = (short)buffer[next_keygroup + i * next_keygroup 
	                             + 0x5];
    new_keygrp->tune_semi = (short)buffer[next_keygroup + 
	                                  i * next_keygroup + 0x6];
    new_keygrp->env_A_rate[0] = (short)buffer[next_keygroup + 
	                                  i * next_keygroup + 12];
    new_keygrp->env_D_rate[0] = (short)buffer[next_keygroup +
	                                  i * next_keygroup + 13];
    new_keygrp->env_S[0] = (short)buffer[next_keygroup + 
	                                  i * next_keygroup + 14];
    new_keygrp->env_R_rate[0] = (short)buffer[next_keygroup + 
	                                  i * next_keygroup + 15];
    akaiToAscii(buffer + next_keygroup + i * next_keygroup + 0x22, 12);
    strcpy(new_keygrp->zone[0], buffer + next_keygroup + 
		                i * next_keygroup + 0x22);
    new_keygrp->zone_low_vel[0] = buffer[next_keygroup + 
	                                 i * next_keygroup + 0x2e];
    new_keygrp->zone_high_vel[0] = buffer[next_keygroup + 
	                                  i * next_keygroup + 0x2f];
    new_keygrp->zone_pan[0] = buffer[next_keygroup + 
	                             i * next_keygroup + 0x34];
    akaiToAscii(buffer + next_keygroup + 
		i * next_keygroup + 0x22 + 24, 12);
    strcpy(new_keygrp->zone[1], buffer + next_keygroup + 
		    i * next_keygroup + 0x22 + 24);
    new_keygrp->zone_low_vel[1] = buffer[next_keygroup + 
	    i * next_keygroup + (0x2e) + 24];
    new_keygrp->zone_high_vel[1] = buffer[next_keygroup + 
	    i * next_keygroup + (0x2f) + 24];
    new_keygrp->zone_pan[1] = buffer[next_keygroup + 
	    i * next_keygroup + 0x34 + 24];
    akaiToAscii(buffer + next_keygroup + i * next_keygroup + 
		     0x22 + 24 * 2, 12);
    strcpy(new_keygrp->zone[2], buffer + next_keygroup + 
		    i * next_keygroup + 0x22 + 24 * 2);
    new_keygrp->zone_low_vel[2] = buffer[next_keygroup + 
	    i * next_keygroup + 0x2e + 24 * 2];
    new_keygrp->zone_high_vel[2] = buffer[next_keygroup + 
	    i * next_keygroup + 0x2f + 24 * 2];
    new_keygrp->zone_pan[2] = buffer[next_keygroup + 
	    i * next_keygroup + 0x34 + 24 * 2];
    akaiToAscii(buffer + next_keygroup + i * next_keygroup + 0x22 + 
		    24 * 3, 12);
    strcpy(new_keygrp->zone[3], buffer + next_keygroup +
		    i * next_keygroup + 0x22 + 24 * 3);
    new_keygrp->zone_low_vel[3] = buffer[next_keygroup + 
	    i * next_keygroup + 0x2e + 24 * 3];
    new_keygrp->zone_high_vel[3] = buffer[next_keygroup+i*next_keygroup+0x2f + 24*3];
    new_keygrp->zone_pan[3] = buffer[next_keygroup + i * next_keygroup
	    + 0x34 + 24 * 3];
  }
  return (new_prog);
}

t_akaiSample		*akaiGetSample(int fd, long ofs, long size) 
{ 
  unsigned char		*buffer; 
  t_akaiSample		*sample;
  int i;

  sample = (t_akaiSample *)malloc(sizeof(t_akaiSample));
  sample->name = (char *)malloc(sizeof(char) * 14); 
  buffer = (void *)malloc(size);
  lseek(fd, offset + ofs * BLOCK_SIZE, SEEK_SET);
  read(fd, buffer, size);
  akaiToAscii(buffer + 3, 12);
  strcpy(sample->name, buffer + 3); 
  sample->base_note = buffer[0x02]; 
  sample->size = (unsigned char)buffer[0x1a] | 
    (((unsigned char)buffer[0x1b]) << 8) |
    (((unsigned char)buffer[0x1c]) << 16) |
    (((unsigned char)buffer[0x1d]) << 24);
  sample->start = (unsigned char)buffer[0x1e] |
    (((unsigned char)buffer[0x1f]) << 8) |
    (((unsigned char)buffer[0x20]) << 16) |
    (((unsigned char)buffer[0x21]) << 24);
  sample->end = (unsigned char)buffer[0x22] |
    (((unsigned char)buffer[0x23]) << 8) |
    (((unsigned char)buffer[0x24]) << 16) |
    (((unsigned char)buffer[0x25]) << 24);
  sample->loop_start = (unsigned char)buffer[0x26] |
    (((unsigned char)buffer[0x27]) << 8) |
    (((unsigned char)buffer[0x28]) << 16) |
    (((unsigned char)buffer[0x29]) << 24);
  sample->loop_len = (unsigned char)buffer[0x2c] |
    (((unsigned char)buffer[0x2d]) << 8) |
    (((unsigned char)buffer[0x2e]) << 16) |
    (((unsigned char)buffer[0x2f]) << 24);
  sample->loop_times = (unsigned char)buffer[0x30] |
    (((unsigned char)buffer[0x31]) << 8);
  sample->rate = (unsigned char)buffer[0x8a] |
    (((unsigned char)buffer[0x8b]) << 8);
  sample->buffer = (short*)buffer + 192;
#ifdef WORDS_BIGENDIAN
  for (i = 0; i < sample->size - 100; i++) 
    sample->buffer[i] = bswap_16(sample->buffer[i]);
#endif
  return (sample);
}

/*
int		akaiSample2WAV(t_akaiSample *sample, char *dstname) 
{ 
  AFfilesetup	afsetup; 
  AFfilehandle	af;

  afsetup = afNewFileSetup();
  afInitFileFormat(afsetup, AF_FILE_WAVE);
  afInitChannels(afsetup, AF_DEFAULT_TRACK, 1); 
  afInitSampleFormat(afsetup, AF_DEFAULT_TRACK, AF_SAMPFMT_TWOSCOMP, 16);
  afInitRate(afsetup, AF_DEFAULT_TRACK, (double)44100.0);
  afInitByteOrder(afsetup, AF_DEFAULT_TRACK, AF_BYTEORDER_LITTLEENDIAN);
  af = afOpenFile(dstname, "w", afsetup);
  afFreeFileSetup(afsetup);
  if (!af)
    return (-1);
  afWriteFrames(af, AF_DEFAULT_TRACK, sample->buffer, sample->size);
  afCloseFile(af);
  return (0);
}
*/

t_akaiSample	*akaiGetSampleByName(char *dev, int part, char *subdir, char *name)
{
  int		fd;
  t_list	*list;
  t_akaiDirent	*rec;

  if ((fd = open(dev, O_RDONLY)) < 0)
  {
    return (0);
  }
  if (akaiSelectPart(fd, part) < 0)
  {
    close(fd);
    return (0);
  }
  list = akaiReadDir(fd, subdir);
  if (!list) 
  {
    close(fd);
    return (0); 
  }
  rec = akaiFindRecord(list, name);
  if (!rec) 
  {
    close(fd);
    return (0);
  }
  return (akaiGetSample(fd, rec->offset, rec->size));
}

void		akaiLoadSamples(int fd, t_akaiProgram *prog, t_list *l) 
{ 
  t_akaiDirent	*rec; 
  t_list	*t;
  int		j;
  
  for (t = prog->keygrp; t; t = t->next)
  {
    for (j = 0; j < 4; j++)
    {
      rec = akaiFindRecord(l, elt(t, t_akaiKeygrp *)->zone[j]); 
      if (rec)
        elt(t, t_akaiKeygrp *)->zone_sample[j] = 
		akaiGetSample(fd, rec->offset, rec->size); 
      else 
        elt(t, t_akaiKeygrp *)->zone_sample[j] = 0;
    }
  }
}

t_akaiProgram	*akaiLoadProgram(char *dev, int partition, char *subdir, char *name) 
{
  void		*buffer; 
  static int	fd;
  t_list	*list; 
  t_akaiDirent	*rec;
  t_akaiProgram	*prog;

  if ((fd = open(dev, O_RDONLY)) < 0)
      return (0);
  if (akaiSelectPart(fd, partition) < 0)
  {
    close(fd);
    return (0);
  }
  list = akaiReadDir(fd, subdir);
  if (!list) 
  {
    close(fd);
    return (0); 
  }
  rec = akaiFindRecord(list, name);
  if (!rec)
  {
    close(fd);
    return (0);
  }
  buffer = (void *)malloc(rec->size);
  lseek(fd, offset + rec->offset * BLOCK_SIZE, SEEK_SET);
  read(fd, buffer, rec->size);
  prog = akaiOpenProg(buffer, rec->size);
  akaiLoadSamples(fd, prog, list);
  return (prog);
}
