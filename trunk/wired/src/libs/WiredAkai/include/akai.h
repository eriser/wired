// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

/*
** Copyright (C) 2004-2006 by Wired Team and Robert Melby
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

#ifndef _AKAI_H_
#define _AKAI_H_

#ifdef __cplusplus
	extern "C" {
#endif

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <byteswap.h>
#include <list.h>

#define FILE_ENTRY_SIZE      		24
#define DIR_ENTRY_OFFSET     		0xca
#define DIR_ENTRY_SIZE       		16
#define ROOT_ENTRY_OFFSET    		0x0
#define BLOCK_SIZE           		0x2000
#define PART_HEADER_OFFSET   		0x4400
#define PART_TABLE_OFFSET    		0x4500
#define PART_END_MARK        		0x8000
#define FAT_OFFSET           		0x70a
#define FAT_MARK_RESERVED    		0x4000 
#define FAT_MARK_RESERVED2   		0x8000
#define MAX_FILE_ENTRIES_S1000  	125
#define MAX_FILE_ENTRIES_S3000  	509
#define FILE_ENTRY_SIZE         	24
#define MAX_DIR_ENTRIES         	100
#define TYPE_FREE               	0
#define TYPE_DIR_S1000          	1
#define TYPE_DIR_S3000          	3

typedef struct 	s_akaiType
{
  int		typeVal;
  char		*typeName;
} 		t_akaiType;

typedef struct	s_akaiDirent 
{ 
  char		*name; 
  int		type; 
  int		size; 
  int		offset; 
  t_list	*subdirs;
}		t_akaiDirent;

typedef struct	s_akaiProgram
{ 
  char		*name; 
  int		num; 
  int		channel;
  int		highkey;
  int		lowkey; 
  int		nkeygrps;
  t_list	*keygrp;
}		t_akaiProgram;

typedef struct		s_akaiSample
{ 
  char 			*name; 
  unsigned short 	tune; 
  unsigned int 		size;
  unsigned int 		start;
  unsigned int 		end;
  unsigned int 		loop_start; 
  unsigned int 		loop_len; 
  unsigned short 	loop_times;
  unsigned short 	rate; 
  unsigned short 	base_note;
  int 			channels; 
  short  		*buffer; 
}			t_akaiSample;

typedef struct	s_akaiKeygrp
{
  int 		num;
  int 		highkey; 
  int 		lowkey; 
  int 		tune;
  int 		tune_semi;
  int 		env_A[2]; 
  int 		env_D[2]; 
  int 		env_S[2]; 
  int 		env_R[2]; 
  int 		env_A_rate[2]; 
  int 		env_D_rate[2]; 
  int 		env_S_rate[2]; 
  int 		env_R_rate[2]; 
  char 		*zone[4];
  t_akaiSample	*zone_sample[4];
  int 		zone_high_vel[4];
  int		zone_low_vel[4];
  int		zone_pan[4];
}		t_akaiKeygrp;

t_list		*akaiReadDir(int, char *);
int		akaiSelectPart(int, int);
char		*akaiGetType(int);
t_akaiSample	*akaiGetSampleByName(char *, int , char *, char *);
int		akaiSample2WAV(t_akaiSample *, char *);
t_akaiProgram	*akaiLoadProgram(char *dev, int partition, char *subdir, char *name);

#ifdef __cplusplus
  }
#endif

#endif
