#include "akai.h"

void dumpProgram(akaiProgram *prgm)
{
  vector<akaiKeygroup *> kgs;
  kgs = prgm->getKeygroups();
  vector<akaiKeygroup *>::iterator i;
  cout << "*** akaiProgram DUMP ***" << endl;
  cout << "path: " << prgm->getPath() << endl;
  int num = 1;
  for (i = kgs.begin(); i != kgs.end(); i++)
  {
    cout << "keygroup " << num++ << ":" << endl;
    cout << "  - lowkey: " << (int)(*i)->getLowKey() << endl;
    cout << "  - highKey: " << (int)(*i)->getHighKey() << endl;
    cout << "  - samples:" << endl;
    vector<akaiSample *> smps;
    vector<akaiSample *>::iterator j;
    smps = (*i)->getSamples();
    for (j = smps.begin(); j < smps.end(); j++)
      cout << "    - " << (*j)->getPath() << endl;
  }
  cout << "*** end of akaiProgram DUMP ***" << endl;
}

void dumpSample(akaiSample *sample)
{
  t_akaiSample *smp = sample->getHeaders();
  cout << "*** akaiSample DUMP ***" << endl;
  cout << "path: " << sample->getPath() << endl;
  cout << "sampleRate: " << (int)smp->sampleRate << endl;
  cout << "midiNote: " << (int)smp->midiNote << endl;
  cout << "nbLoop: " << (int)smp->nbLoop << endl;
  cout << "loopMode: " << (int)smp->loopMode << endl;
  cout << "centsTune: " << (int)smp->centsTune << endl;
  cout << "semiTune: " << (int)smp->semiTune << endl;
  cout << "nbWord: " << (int)smp->nbWord << endl;
  cout << "start: " << (int)smp->start << endl;
  cout << "end: " << (int)smp->end << endl;
  for (int i = 0; (i < 8) && (i < smp->nbLoop); i++)
  {
    cout << "loop " << (i + 1) << ":" << endl;
    cout << "  - start: " << (int)smp->loops[i].loopStart << endl;
    cout << "  - finelength: " << (int)smp->loops[i].loopFineLength << endl;
    cout << "  - len: " << (int)smp->loops[i].loopLen << endl;
    cout << "  - time: " << (int)smp->loops[i].loopTime << endl;
  }
  cout << "sampleFreq: " << (int)smp->sampleFreq << endl;
  cout << "*** end of akaiSample DUMP ***" << endl;
}

void dumpFiles(akaiVolume *vol, char c1, char c2)
{
  vector<akaiFile *> files = vol->getFiles();
  vector<akaiFile *>::iterator i;
  for (i = files.begin(); i != files.end(); i++)
  {
    printf("%c  %c  %c--+ %s type %02X size %06lX\n", c1, c2, ((i + 1) == files.end()) ? '\\' : '|', (*i)->getName().c_str(), (*i)->getType(), (*i)->getSize());
  }
}

void dumpVolumes(akaiPartition *part, char c)
{
  vector<akaiVolume *> vols = part->getVolumes();
  vector<akaiVolume *>::iterator i;
  for (i = vols.begin(); i != vols.end(); i++)
  {
    printf("%c  %c--+ %s type %02X\n", c, ((i + 1) == vols.end()) ? '\\' : '|', (*i)->getName().c_str(), (*i)->getType());
    dumpFiles(*i, c, ((i + 1) == vols.end()) ? ' ' : '|');
  }
}

void dumpPartitions(akaiImage *img)
{
  vector<akaiPartition *> parts = img->getPartitions();
  vector<akaiPartition *>::iterator i;
  int nb = 0;
  printf("+ %s\n", img->getPath().c_str());
  for (i = parts.begin(); i != parts.end(); i++)
  {
    printf("%c--+ Partition %c\n", ((i + 1) == parts.end()) ? '\\' : '|', 'A' + nb++);
    dumpVolumes(*i, (i + 1 == parts.end()) ? ' ' : '|');
  }
}

int main()
{
  akaiImage *img = new akaiImage("/akai.iso");
  dumpPartitions(img);
  if (img)
  {
    akaiSample *smp = img->getSample("A/01 VI LNG FF/VI LGFF G2-L");
    if (smp)
      dumpSample(smp);
    akaiProgram *prgm = img->getProgram("A/01 VI LNG FF/VI LONG FF");
    if (prgm)
      dumpProgram(prgm);
    if (prgm)
      delete prgm;
    if (smp)
      delete smp;
    delete img;
  }
  return 0;
}
