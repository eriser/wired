
#if !defined(IMPORT_WAVE_ACTION_H)
#define IMPORT_WAVE_ACTION_H

#include "cAction.h"
#include "cActionManager.h"
#include "Visitor.h"

#include "AudioCenter.h"
#include "Rack.h"

class cImportWaveAction : public cAction 
{
private:
   bool mTrackKindFlag;
   string mWavePath;
 
public:
  cImportWaveAction (std::string path, bool kind);
  ~cImportWaveAction () {};
  virtual void Do ();
  virtual void Redo ();
  virtual void Undo ();
  virtual void Accept (cActionVisitor& visitor)
  { visitor.Visit (*this); };
};

/////////////////////////////////////////////////////////////////

class cImportMidiAction : public cAction 
{
private:
   bool mTrackKindFlag;
   string mMidiPath;

public:
  cImportMidiAction (std::string path, bool kind);
  ~cImportMidiAction () {};
  virtual void Do ();
  virtual void Redo ();
  virtual void Undo ();
  virtual void Accept (cActionVisitor& visitor)
  { visitor.Visit (*this); };
};

/////////////////////////////////////////////////////////////////

class cImportAkaiAction : public cAction 
{
private:
   bool mTrackKindFlag;
   string mDevice;
   int mPart;
   string mPath;
   string mName;

public:
  cImportAkaiAction (std::string path, bool kind);
  ~cImportAkaiAction () {};
  virtual void Do ();
  virtual void Redo ();
  virtual void Undo ();
  virtual void Accept (cActionVisitor& visitor)
  { visitor.Visit (*this); };
};


/////////////////////////////////////////////////////////////////

class cCreateEffectAction : public cAction 
{
private:
  PluginLoader * mPluginLoader;
  PlugStartInfo * mStartInfo;
  
public:
  cCreateEffectAction (PlugStartInfo* startInfo, PluginLoader * plugin);
  ~cCreateEffectAction () {};
  virtual void Do ();
  virtual void Redo ();
  virtual void Undo ();
  virtual void Accept (cActionVisitor& visitor)
  { visitor.Visit (*this); };
};


/////////////////////////////////////////////////////////////////


class cCreateRackAction : public cAction 
{
private:
  PluginLoader * mPluginLoader;
  PlugStartInfo * mStartInfo;
  
public:
  cCreateRackAction (PlugStartInfo* startInfo, PluginLoader* plugLoader);
  ~cCreateRackAction () {};
  virtual void Do ();
  virtual void Redo ();
  virtual void Undo ();
  virtual void Accept (cActionVisitor& visitor)
  { visitor.Visit (*this); };
};


#endif
