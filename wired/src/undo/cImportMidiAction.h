#if !defined(IMPORT_WAVE_ACTION_H)
#define IMPORT_WAVE_ACTION_H

using namespace				std;

#include <string>
#include "cAction.h"
#include "cActionManager.h"
#include "Visitor.h"
#include "PluginLoader.h"

#define	INVALID_VALUE								-42
#define	HISTORY_LABEL_CREATE_EFFECT_ACTION			"creating rack effect"
#define	HISTORY_LABEL_IMPORT_WAVE_ACTION			"importing wave"
#define	HISTORY_LABEL_IMPORT_MIDI_ACTION			"importing midi"
#define	HISTORY_LABEL_IMPORT_AKAI_ACTION			"importing AKAI"
#define	HISTORY_LABEL_CREATE_RACK_ACTION			"creating rack"
#define	HISTORY_LABEL_CHANGE_PARAM_EFFECT_ACTION	"changing rack effect parameters"

//class								Plugin;
//class								PluginLoader;
////class								WiredPluginData;
//typedef struct s_PlugStartInfo		PlugStartInfo;


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
  virtual std::string		getHistoryLabel()		// Returns History label string
  							{return HISTORY_LABEL_IMPORT_WAVE_ACTION;};
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
  virtual std::string		getHistoryLabel()		// Returns History label string
  							{return HISTORY_LABEL_IMPORT_MIDI_ACTION;};
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
   virtual std::string		getHistoryLabel()		// Returns History label string
   							{return HISTORY_LABEL_IMPORT_AKAI_ACTION;};
};


/********************   class cChangeParamsEffectAction   ********************/

class					cChangeParamsEffectAction : public cAction 
{
private:
//	WiredPluginData 		mDatas;					// Contexte datas
	Plugin*					mPlugin;				// Contexte instance
	bool					mShouldSave;				// Saves or load action
  
public:
	cChangeParamsEffectAction (Plugin* plugin, bool shouldSave);
	~cChangeParamsEffectAction () {};
	virtual void			Do ();					// Does action
	virtual void			Redo ();				// Does redo action
	virtual void			Undo ();				// Does undo action
	virtual void			Accept					// Don't known
							(cActionVisitor& visitor) { visitor.Visit (*this); };
	virtual std::string		getHistoryLabel()		// Returns History label string
							{return HISTORY_LABEL_CHANGE_PARAM_EFFECT_ACTION;};
	void					SaveDatas();			// Saves mDatas
	void					LoadDatas();			// Loads mDatas
  	void					Dump();					// Debug - Draws member variables
};


/********************   class cCreateEffectAction   ********************/

class						cCreateEffectAction : public cAction 
{
private:
	PluginLoader			*mPluginLoader;			// Contexte
	PlugStartInfo			*mStartInfo;			// Contexte
	bool					mShouldAdd;				// True if should add in Do()
	int						mRackIndex;				// Index du rack dans le RackPanel - Abandonne car remove casse les index dsna RackTracks
	//RackTrack				*mRackTrack;			// Effect instance
  
public:
	cCreateEffectAction (PlugStartInfo* startInfo, PluginLoader * plugin, bool shouldAdd);
	~cCreateEffectAction () {};
	virtual void			Do ();					// Does action
	virtual void			Redo ();				// Does redo action
	virtual void			Undo ();				// Does undo action
	virtual void			Accept					// Don't known
							(cActionVisitor& visitor) { visitor.Visit (*this); };
	virtual std::string		getHistoryLabel();		// Returns History label string
	void					AddRackEffect ();		// Adds a rack effect
	void					RemoveRackEffect ();	// Removes a rack effect
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
  virtual std::string		getHistoryLabel()		// Returns History label string
  							{return HISTORY_LABEL_CREATE_RACK_ACTION;};
};

#endif
