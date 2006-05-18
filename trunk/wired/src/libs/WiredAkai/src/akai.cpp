// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#include "akai.h"

/************************************************************************************************************/
/***                                                                                                      ***/
/*** akai2string                                                                                          ***/
/***                                                                                                      ***/
/************************************************************************************************************/

void akai2string(unsigned char akaiName[12], string &s)
{
  s = "";
  int i;
  for (i = 0; i < 12; i++)
  {
    if (akaiName[i] <= 9)
      s.push_back('0' + akaiName[i]);
    else if (akaiName[i] == 10)
      s.push_back(' ');
    else if ((akaiName[i] >= 11) && (akaiName[i] <= 36))
      s.push_back((char)('A' - 11 + akaiName[i]));
    else if (akaiName[i] == 37)
      s.push_back('#');
    else if (akaiName[i] == 38)
      s.push_back('+');
    else if (akaiName[i] == 39)
      s.push_back('-');
    else if (akaiName[i] == 40)
      s.push_back('.');
    else
      s.push_back(akaiName[i]);
  }
  for (i = s.length() - 1; i && (s[i] == ' '); i--);
  s = s.substr(0, i + 1);
}


/************************************************************************************************************/
/***                                                                                                      ***/
/*** akaiKeygroup                                                                                         ***/
/***                                                                                                      ***/
/************************************************************************************************************/

akaiKeygroup::akaiKeygroup(akaiProgram *prgm, t_akaiKeygroup *kg) :
  prgm(prgm), lowKey(kg->lowKey), highKey(kg->highKey), kg(kg)
{
  string name;
  for (int i = 0; i < 4; i++)
  {
    akai2string(kg->samples[i].akaiName, name);
    akaiSample *smp = prgm->getSample(name);
    if (smp)
      samples.push_back(smp);
  }
}

akaiKeygroup::~akaiKeygroup()
{
  for (vector<akaiSample *>::iterator i = samples.begin(); i != samples.end(); )
  {
    delete *i;
    i = samples.erase(i);
  }
  samples.clear();
}


/************************************************************************************************************/
/***                                                                                                      ***/
/*** akaiProgram                                                                                          ***/
/***                                                                                                      ***/
/************************************************************************************************************/

akaiProgram::akaiProgram(akaiFile *file, void *buf, unsigned long len) :
  buf(buf), file(file)
{
  prgm = (t_akaiProgram *)buf;
  akai2string(prgm->akaiName, name);
  unsigned long ofs = prgm->firstKeygroup;
  for (int i = 0; (ofs + sizeof(t_akaiKeygroup) < len) && (i < prgm->nbKeygroups); i++)
  {
    t_akaiKeygroup *kg = (t_akaiKeygroup *)((unsigned char *)buf + ofs);
    content.push_back(new akaiKeygroup(this, kg));
    ofs = kg->nextKeygroup;
  }
}

akaiProgram::~akaiProgram()
{
  for (vector<akaiKeygroup *>::iterator i = content.begin(); i != content.end(); )
  {
    delete *i;
    i = content.erase(i);
  }
  content.clear();
  free(buf);
}

akaiSample *akaiProgram::getSample(string path)
{
  return file->getSample(path);
}

string akaiProgram::getPath()
{
  return file->getPath();
}


/************************************************************************************************************/
/***                                                                                                      ***/
/*** akaiSample                                                                                           ***/
/***                                                                                                      ***/
/************************************************************************************************************/

akaiSample::akaiSample(akaiFile *file, void *buf, unsigned long len) :
  buf(buf), file(file)
{
  smp = (t_akaiSample *)buf;
  sample = (short *)((unsigned char *)buf + sizeof(t_akaiSample));
  size = len - sizeof(t_akaiSample);
  akai2string(smp->akaiName, name);
  rate = smp->sampleFreq;
}

akaiSample::~akaiSample()
{
  free(buf);
}

string akaiSample::getPath()
{
  return file->getPath();
}


/************************************************************************************************************/
/***                                                                                                      ***/
/*** akaiFile                                                                                             ***/
/***                                                                                                      ***/
/************************************************************************************************************/

akaiFile::akaiFile(akaiVolume *vol, t_akaiFile *file) :
  vol(vol), type(file->type), block(file->block)
{
  akai2string(file->akaiName, name);
  size = file->size[2];
  size <<= 8;
  size += file->size[1];
  size <<= 8;
  size += file->size[0];
}

akaiFile::~akaiFile()
{
}

akaiSample *akaiFile::getSample()
{
  if (type != 's')
    return NULL;
  void *buf = malloc(size);
  unsigned long left = size;
  unsigned short curblk = block;
  unsigned long ofs = 0;
  while (left > BLOCKSIZE)
  {
    unsigned char *buffer = _readBlock(curblk);
    memcpy((unsigned char *)buf + ofs, buffer, BLOCKSIZE);
    free(buffer);
    left -= BLOCKSIZE;
    ofs += BLOCKSIZE;
    curblk = _getNextBlock(curblk);
  }
  unsigned char *buffer = _readBlock(curblk);
  memcpy((unsigned char *)buf + ofs, buffer, left);
  free(buffer);
  return new akaiSample(this, buf, size);
}

akaiProgram *akaiFile::getProgram()
{
  if (type != 'p')
    return NULL;
  void *buf = malloc(size);
  unsigned long left = size;
  unsigned short curblk = block;
  unsigned long ofs = 0;
  while (left > BLOCKSIZE)
  {
    unsigned char *buffer = _readBlock(curblk);
    memcpy((unsigned char *)buf + ofs, buffer, BLOCKSIZE);
    free(buffer);
    left -= BLOCKSIZE;
    ofs += BLOCKSIZE;
    curblk = _getNextBlock(curblk);
  }
  unsigned char *buffer = _readBlock(curblk);
  memcpy((unsigned char *)buf + ofs, buffer, left);
  free(buffer);
  return new akaiProgram(this, buf, size);
}

akaiSample *akaiFile::getSample(string path)
{
  akaiFile *f = vol->getFile(path);
  if (f)
    return f->getSample();
  return NULL;
}

akaiProgram *akaiFile::getProgram(string path)
{
  akaiFile *f = vol->getFile(path);
  if (f)
    return f->getProgram();
  return NULL;
}

string akaiFile::getPath()
{
  string s = vol->getPath();
  s += "/";
  s += name;
  return s;
}

unsigned char *akaiFile::_readBlock(unsigned short block)
{
  return vol->_readBlock(block);
}

unsigned short akaiFile::_getNextBlock(unsigned short block)
{
  return vol->_getNextBlock(block);
}

unsigned long akaiFile::_read(unsigned long offset, void *buf, unsigned long size)
{
  return vol->_read(offset, buf, size);
}


/************************************************************************************************************/
/***                                                                                                      ***/
/*** akaiVolume                                                                                           ***/
/***                                                                                                      ***/
/************************************************************************************************************/

akaiVolume::akaiVolume(akaiPartition *part, t_akaiVolume *vol) :
  part(part), type(vol->type), block(vol->block)
{
  akai2string(vol->akaiName, name);
  int i = 0;
  t_akaiFile *file;
  unsigned short curblk = block;
  unsigned char *buf = _readBlock(curblk);
  do
  {
    if (i * sizeof(t_akaiFile) >= BLOCKSIZE)
    {
      free(buf);
      curblk = _getNextBlock(curblk);
      buf = _readBlock(curblk);
    }
    file = (t_akaiFile *)(buf + i++ * sizeof(t_akaiFile));
    if (file->type)
      content.push_back(new akaiFile(this, file));
  } while (file->type);
  free(buf);
}

akaiVolume::~akaiVolume()
{
  for (vector<akaiFile *>::iterator i = content.begin(); i != content.end(); )
  {
    delete *i;
    i = content.erase(i);
  }
  content.clear();
}

akaiFile *akaiVolume::getFile(string path)
{
  for (vector<akaiFile *>::iterator i = content.begin(); i != content.end(); i++)
    if (!path.compare((*i)->getName()))
      return (*i);
  return NULL;
}

string akaiVolume::getPath()
{
  string s = part->getPath();
  s += "/";
  s += name;
  return s;
}

unsigned char *akaiVolume::_readBlock(unsigned short block)
{
  return part->_readBlock(block);
}

unsigned short akaiVolume::_getNextBlock(unsigned short block)
{
  return part->_getNextBlock(block);
}

unsigned long akaiVolume::_read(unsigned long offset, void *buf, unsigned long size)
{
  return part->_read(offset, buf, size);
}


/************************************************************************************************************/
/***                                                                                                      ***/
/*** akaiPartition                                                                                        ***/
/***                                                                                                      ***/
/************************************************************************************************************/

akaiPartition::akaiPartition(akaiImage *img, unsigned long offset, unsigned char letter) :
  img(img), offset(offset), letter(letter)
{
  int i = 0;
  t_akaiVolume vol;
  do
  {
    _read(DIR_ENTRY_OFS + i++ * sizeof(t_akaiVolume), &vol, sizeof(t_akaiVolume));
    if (vol.type)
      content.push_back(new akaiVolume(this, &vol));
  } while (vol.type);
}

akaiPartition::~akaiPartition()
{
  for (vector<akaiVolume *>::iterator i = content.begin(); i != content.end(); )
  {
    delete *i;
    i = content.erase(i);
  }
  content.clear();
}

akaiFile *akaiPartition::getFile(string path)
{
  int pos = path.find('/', 0);
  if (pos < 0)
    return NULL;
  string vol = path.substr(0, pos);
  for (vector<akaiVolume *>::iterator i = content.begin(); i != content.end(); i++)
    if (!vol.compare((*i)->getName()))
      return (*i)->getFile(path.substr(pos + 1, path.length()));
  return NULL;
}

string akaiPartition::getPath()
{
  string s = img->getPath();
  s += ":";
  s += letter;
  return s;
}

string akaiPartition::getName()
{
  string s = "";
  s += letter;
  return s;
}

unsigned char *akaiPartition::_readBlock(unsigned short block)
{
  return img->_readBlock(block, offset);
}

unsigned short akaiPartition::_getNextBlock(unsigned short block)
{
  return img->_getNextBlock(block, offset);
}

unsigned long akaiPartition::_read(unsigned long offset, void *buf, unsigned long size)
{
  return img->_read(this->offset + offset, buf, size);
}


/************************************************************************************************************/
/***                                                                                                      ***/
/*** akaiImage                                                                                            ***/
/***                                                                                                      ***/
/************************************************************************************************************/

akaiImage::akaiImage(string path) :
  path(path)
{
  fd = open(path.c_str(), O_RDONLY);
  if (fd != -1)
  {
    unsigned short sz;
    unsigned long offset = 0;
    unsigned char letter = 'A';
    do
    {
      _read(offset, &sz, 2);
      if (sz && (sz != END_OF_PART))
      {
        content.push_back(new akaiPartition(this, offset, letter++));
        offset += sz * BLOCKSIZE;
      }
    } while (sz && (sz != END_OF_PART));
  }
}

akaiImage::~akaiImage()
{
  if (fd != -1)
    close(fd);
  for (vector<akaiPartition *>::iterator i = content.begin(); i != content.end(); )
  {
    delete *i;
    i = content.erase(i);
  }
  content.clear();
}

akaiProgram *akaiImage::getProgram(string path)
{
  akaiFile *f = getFile(path);
  if (f)
    return f->getProgram();
  return NULL;
}

akaiSample *akaiImage::getSample(string path)
{
  akaiFile *f = getFile(path);
  if (f)
    return f->getSample();
  return NULL;
}

akaiFile *akaiImage::getFile(string path)
{
  if (fd == -1)
    return NULL;
  unsigned char part = path.c_str()[0] - 'A';
  if (part >= content.size())
    return NULL;
  return content[part]->getFile(path.substr(2, path.length()));
}

unsigned long akaiImage::_read(unsigned long offset, void *buf, unsigned long size)
{
  unsigned long len;
  if (fd == -1)
    return 0;
  lseek(fd, offset, SEEK_SET);
  len = read(fd, buf, size);
  if (len != size)
  {
    perror("read");
    cerr << "akaiImage::_read read failed offset " << offset << ", read " << len << " / wanted " << size << endl;
  }
  return len;
}

unsigned char *akaiImage::_readBlock(unsigned short block, unsigned long offset)
{
  unsigned char *buf;
  unsigned long len;
  if (fd == -1)
    return NULL;
  buf = (unsigned char *)malloc(BLOCKSIZE);
  if (!buf)
    return NULL;
  len = _read(offset + block * BLOCKSIZE, buf, BLOCKSIZE);
  if (len != BLOCKSIZE)
  {
    free(buf);
    return NULL;
  }
  return buf;
}

unsigned short akaiImage::_getNextBlock(unsigned short block, unsigned long offset)
{
  unsigned short blk;
  if (fd == -1)
    return 0;
  _read(offset + block * 2 + FAT_OFFSET, &blk, 2);
  return blk;
}
