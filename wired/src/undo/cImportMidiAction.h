#if !defined(IMPORT_WAVE_ACTION_H)
#define IMPORT_WAVE_ACTION_H

#define	INVALID_VALUE	-42

using namespace				std;

#include <string>
#include "cAction.h"
#include "cActionManager.h"
#include "Visitor.h"

class								PluginLoader;
typedef struct s_PlugStartInfo		PlugStartInfo;


/********************   class cImportWaveAction   ********************/

class					cImportWaveAction : public cAction 
{
private:
   bool					mTrackKindFlag;
   string				mWavePath;
 
public:
  cImportWaveAction (std::string path, bool kind);
  ~cImportWaveAction () {};
  virtual void Do ();
  virtual void Redo ();
  virtual void Undo ();
  virtual void Accept (cActionVisitor& visitor)
  { visitor.Visit (*this); };
};


/********************   class cImportMidiAction   ********************/

class					cImportMidiAction : public cAction 
{
private:
   bool					mTrackKindFlag;
   string				mMidiPath;

public:
  cImportMidiAction (std::string path, bool kind);
  ~cImportMidiAction () {};
  virtual void				Do();
  virtual void				Redo();
  virtual void				Undo();
  virtual void				Accept(cActionVisitor& visitor) { visitor.Visit (*this); };
};


/********************   class cImportAkaiAction   ********************/

class					cImportAkaiAction : public cAction 
{
private:
   bool					mTrackKindFlag;
   string				mDevice;
   int					mPart;
   string				mPath;
   string				mName;
   
public:
   cImportAkaiAction (std::string path, bool kind);
   ~cImportAkaiAction () {};
   virtual void			Do();
   virtual void			Redo();
   virtual void			Undo();
   virtual void			Accept(cActionVisitor& visitor) { visitor.Visit (*this); };
};


/********************   class cCreateEffectAction   ********************/

class						cCreateEffectAction : public cAction 
{
private:
	PluginLoader			*mPluginLoader;			// Don't known
	PlugStartInfo			*mStartInfo;			// Don't known
	bool					mShouldAdd;				// True if should add in Do()
	int						mRackIndex;				// Index du rack dans le RackPanel
  
public:
	cCreateEffectAction (PlugStartInfo* startInfo, PluginLoader * plugin, bool shouldAdd);
	~cCreateEffectAction () {};
	virtual void			Do ();					// Does action
	virtual void			Redo ();				// Does redo action
	virtual void			Undo ();				// Does undo action
	virtual void			Accept					// Don't known
							(cActionVisitor& visitor) { visitor.Visit (*this); };
	void					AddRackEffect ();		// Adds a rack effect
	void					RemoveRackEffect ();	// Adds a rack effect
  	void					Dump();					// Debug - Draws member variables
};


/********************   class cCreateRackAction   ********************/

class					cCreateRackAction : public cAction 
{
private:
  PluginLoader				*mPluginLoader;
  PlugStartInfo				*mStartInfo;
  
public:
  cCreateRackAction (PlugStartInfo* startInfo, PluginLoader* plugLoader);
  ~cCreateRackAction () {};
  virtual void				Do ();
  virtual void				Redo ();
  virtual void				Undo ();
  virtual void				Accept (cActionVisitor& visitor)
  { visitor.Visit (*this); };
};

#endif
