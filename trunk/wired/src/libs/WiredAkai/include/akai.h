#ifndef _AKAI_H_
#define _AKAI_H_

#include <vector>
#include <string>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

using namespace std;
using std::vector;
using std::string;

#define BLOCKSIZE     0x2000
#define END_OF_PART   0x8000
#define DIR_ENTRY_OFS 0x00CA
#define FAT_OFFSET    0x070A

#pragma pack(1)
typedef struct s_akaiVolume
{
  unsigned char akaiName[12];
  unsigned short type;
  unsigned short block;
} t_akaiVolume;

typedef struct s_akaiFile
{
  unsigned char akaiName[12];
  unsigned long pad;
  unsigned char type;
  unsigned char size[3];
  unsigned short block;
  unsigned short id;
} t_akaiFile;

typedef struct s_akaiLoop
{
  unsigned long loopStart;
  unsigned short loopFineLength;
  unsigned long loopLen;
  unsigned short loopTime;
} t_akaiLoop;

typedef struct s_akaiSample
{
  unsigned char pad3;
  unsigned char sampleRate;
  unsigned char midiNote;
  unsigned char akaiName[12];
  unsigned char pad128;
  unsigned char nbLoop;
  unsigned char pad0[2];
  unsigned char loopMode;
  char centsTune;
  char semiTune;
  unsigned char pad0820[4];
  unsigned long nbWord;
  unsigned long start;
  unsigned long end;
  t_akaiLoop loops[8];
  unsigned char pad00255255[4];
  unsigned short sampleFreq;
  unsigned char pad[10];
} t_akaiSample;

typedef struct s_akaiProgram
{
  unsigned char id;
  unsigned short firstKeygroup;
  unsigned char akaiName[12];
  unsigned char midiPrgmNumber;
  unsigned char midiChannel;
  unsigned char polyphony;
  unsigned char priority;
  unsigned char lowKey;
  unsigned char highKey;
  char octShift;
  unsigned char auxOutputSelect;
  unsigned char mixOutputLevel;
  char mixOutputPan;
  unsigned char volume;
  char velVolume;
  char keyVolume;
  char pressVolume;
  unsigned char panLFORate;
  unsigned char panLFODepth;
  unsigned char panLFODelay;
  char keyPan;
  unsigned char LFORate;
  unsigned char LFODepth;
  unsigned char LFODelay;
  unsigned char modLFODepth;
  unsigned char presLFODepth;
  unsigned char velFLODepth;
  unsigned char bendPitch;
  char presPitch;
  unsigned char keygroupCrossfade;
  unsigned char nbKeygroups;
  unsigned char prgmNumber;
  char keyTemps[12];
  unsigned char fxOutput;
  char modPan;
  unsigned char stereoCoherence;
  unsigned char LFODesynch;
  unsigned char pitchLaw;
  unsigned char voiceReassign;
  unsigned char softpedVolume;
  unsigned char softpedAttack;
  unsigned char softpedFilt;
  char tuneCents;
  char tuneSemitones;
  char keyLFORate;
  char keyLFODepth;
  char keyLFODelay;
  unsigned char voiceOutputScale;
  unsigned char stereoOutputScale;
  unsigned char pad[78];
} t_akaiProgram;

typedef struct s_akaiKeygroupSample
{
  unsigned char akaiName[12];
  unsigned char lowVel;
  unsigned char highVel;
  char tuneCents;
  char tuneSemitones;
  char loudness;
  char filter;
  char pan;
  unsigned char loopMode;
  unsigned char pad[4];
} t_akaiKeygroupSample;

typedef struct s_akaiKeygroup
{
  unsigned char id;
  unsigned short nextKeygroup;
  unsigned char lowKey;
  unsigned char highKey;
  char tuneCents;
  char tuneSemitones;
  unsigned char filter;
  unsigned char keyFilter;
  char velFilter;
  char presFilter;
  char env2Filter;
  unsigned char env1Attack;
  unsigned char env1Decay;
  unsigned char env1Sustain;
  unsigned char env1Release;
  char env1VelAttack;
  char env1VelRelease;
  char env1OffVelRelease;
  char env1KeyDecRel;
  unsigned char env2Attack;
  unsigned char env2Decay;
  unsigned char env2Sustain;
  unsigned char env2Release;
  char env2VelAttack;
  char env2VelRelease;
  char env2OffVelRelease;
  char env2KeyDecRel;
  char velEnv2Filter;
  char env2Pitch;
  unsigned char velZoneCrossfade;
  unsigned char velZonesUsed;
  unsigned char pad[2];
  t_akaiKeygroupSample samples[4];
  char beatDetune;
  unsigned char holdAttackUntilLoop;
  unsigned char keyTracking[4];
  unsigned char auxOutOffset[4];
  unsigned short velSampleStart[4];
  char velVolOffset;
  unsigned char pad2;
} t_akaiKeygroup;
#pragma pack(0)

class akaiImage;
class akaiPartition;
class akaiVolume;
class akaiFile;
class akaiSample;
class akaiProgram;
class akaiKeygroup;

class akaiKeygroup
{
  public:
    akaiKeygroup(akaiProgram *, t_akaiKeygroup *);
    ~akaiKeygroup();
    unsigned char getLowKey() { return lowKey; }
    unsigned char getHighKey() { return highKey; }
    vector<akaiSample *> getSamples() { return samples; }
    t_akaiKeygroup *getHeaders() { return kg; }
  private:
    akaiProgram *prgm;
    unsigned char lowKey;
    unsigned char highKey;
    vector<akaiSample *> samples;
    t_akaiKeygroup *kg;
};

class akaiProgram
{
  public:
    akaiProgram(akaiFile *, void *, unsigned long);
    ~akaiProgram();
    t_akaiProgram *getHeaders() { return prgm; }
    vector<akaiKeygroup *> getKeygroups() { return content; }
    string getName() { return name; }
    string getPath();

    akaiSample *getSample(string);
  private:
    void *buf;
    akaiFile *file;
    t_akaiProgram *prgm;
    vector<akaiKeygroup *> content;
    string name;
};

class akaiSample
{
  public:
    akaiSample(akaiFile *, void *, unsigned long);
    ~akaiSample();
    short *getSample() { return sample; }
    t_akaiSample *getHeaders() { return smp; }
    string getName() { return name; }
    unsigned long getSize() { return size; }
    string getPath();
    unsigned short getRate() { return rate; }
  private:
    void *buf;
    akaiFile *file;
    short *sample;
    t_akaiSample *smp;
    unsigned long size;
    string name;
    unsigned short rate;
};

class akaiFile
{
  public:
    akaiFile(akaiVolume *, t_akaiFile *);
    ~akaiFile();
    string getName() { return name; }
    unsigned char getType() { return type; }
    unsigned long getSize() { return size; }
    string getPath();

    akaiSample *getSample();
    akaiProgram *getProgram();
    akaiSample *getSample(string);
    akaiProgram *getProgram(string);
    unsigned long _read(unsigned long, void *, unsigned long);
    unsigned char *_readBlock(unsigned short);
    unsigned short _getNextBlock(unsigned short);
  private:
    akaiVolume *vol;
    string name;
    unsigned char type;
    unsigned long size;
    unsigned short block;
};

class akaiVolume
{
  public:
    akaiVolume(akaiPartition *, t_akaiVolume *);
    ~akaiVolume();
    string getName() { return name; }
    unsigned short getType() { return type; }
    vector<akaiFile *>getFiles() { return content; }
    string getPath();

    akaiFile *getFile(string);
    unsigned long _read(unsigned long, void *, unsigned long);
    unsigned char *_readBlock(unsigned short);
    unsigned short _getNextBlock(unsigned short);
  private:
    akaiPartition *part;
    string name;
    unsigned short type;
    unsigned short block;
    vector<akaiFile *> content;
};

class akaiPartition
{
  public:
    akaiPartition(class akaiImage *, unsigned long, unsigned char);
    ~akaiPartition();
    vector<akaiVolume *> getVolumes() { return content; }
    string getPath();
    string getName();

    akaiFile *getFile(string);
    unsigned long _read(unsigned long, void *, unsigned long);
    unsigned char *_readBlock(unsigned short);
    unsigned short _getNextBlock(unsigned short);
  private:
    akaiImage *img;
    unsigned long offset;
    vector<akaiVolume *> content;
    unsigned char letter;
};

class akaiImage
{
  public:
    akaiImage(string);
    ~akaiImage();
    string getPath() { return path; }
    vector<akaiPartition *> getPartitions() { return content; }
    akaiSample *getSample(string);
    akaiProgram *getProgram(string);

    akaiFile *getFile(string);
    unsigned long _read(unsigned long, void *, unsigned long);
    unsigned char *_readBlock(unsigned short, unsigned long);
    unsigned short _getNextBlock(unsigned short, unsigned long);
  private:
    int fd;
    string path;
    vector<akaiPartition *> content;
};

#endif
