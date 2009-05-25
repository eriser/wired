// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#if !defined(IMPORT_WAVE_ACTION_H)
#define IMPORT_WAVE_ACTION_H

#include "Track.h"
#include "cAction.h"
#include "cActionManager.h"
#include "Visitor.h"
#include "PluginLoader.h"

#define	INVALID_VALUE								-42
#define	HISTORY_LABEL_CREATE_EFFECT_ACTION			_("creating rack effect")
#define	HISTORY_LABEL_IMPORT_WAVE_ACTION			_("importing wave")
#define	HISTORY_LABEL_IMPORT_MIDI_ACTION			_("importing midi")
#define	HISTORY_LABEL_IMPORT_AKAI_ACTION			_("importing AKAI")
#define	HISTORY_LABEL_CHANGE_PARAM_EFFECT_ACTION	_("changing rack effect parameters")

#define	INVALID_VALUE								-42

/********************   class cImportWaveAction   ********************/

class					cImportWaveAction : public cAction 
{
private:
  // audio, midi, ..
   trackType				_TrackKindFlag;
   wxString				_WavePath;
   long					_trackIndex;
   bool					_ShouldAdd;
   Track*				_trackCreated;

public:
  cImportWaveAction (const wxString& path, trackType kind, bool shouldAdd);
  cImportWaveAction (const cImportWaveAction& copy){*this = copy;};
  ~cImportWaveAction () {};
  
  virtual void				Do ();
  virtual void				Redo ();
  virtual void				Undo ();
  virtual void				Accept (cActionVisitor& visitor)
  							{ visitor.Visit (*this); };
  virtual const wxString		getHistoryLabel()		// Returns History label wstring
  							{return HISTORY_LABEL_IMPORT_WAVE_ACTION;};
  void					AddWaveTrack();
  void					RemoveWaveTrack(bool selectFromIndex);
  void					AddWaveToEditor();
  
  cImportWaveAction		operator=(const cImportWaveAction& right);
};


/********************   class cImportMidiAction   ********************/

class					cImportMidiAction : public cAction 
{
private:
   trackType				mTrackKindFlag;
   wxString				mMidiPath;
   Track*				trackCreated;

public:
  cImportMidiAction (wxString& path, trackType kind);
  cImportMidiAction (const cImportMidiAction& copy){*this = copy;};
  ~cImportMidiAction () {};
  virtual void				Do();
  virtual void				Redo();
  virtual void				Undo();
  virtual void				Accept(cActionVisitor& visitor) { visitor.Visit (*this); };
  virtual const wxString		getHistoryLabel()		// Returns History label wstring
  							{return HISTORY_LABEL_IMPORT_MIDI_ACTION;};
  							
  cImportMidiAction		operator=(const cImportMidiAction& right);
};


/********************   class cImportAkaiAction   ********************/

class					cImportAkaiAction : public cAction 
{
private:
  trackType				mTrackKindFlag;
  Track*				trackCreated;
  wxString				mDevice;
  int					mPart;
  wxString				mPath;
  wxString				mName;

public:
  cImportAkaiAction (wxString& path, trackType kind);
  cImportAkaiAction (const cImportAkaiAction& copy){*this = copy;};
  ~cImportAkaiAction () {};
  virtual void			Do();
  virtual void			Redo();
  virtual void			Undo();
  virtual void			Accept(cActionVisitor& visitor) { visitor.Visit (*this); };
  virtual const wxString		getHistoryLabel()		// Returns History label wstring
   							{return HISTORY_LABEL_IMPORT_AKAI_ACTION;};
   
  cImportAkaiAction		operator=(const cImportAkaiAction& right);
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
	cChangeParamsEffectAction (const cChangeParamsEffectAction& copy){*this = copy;};
	~cChangeParamsEffectAction () {};
	virtual void			Do ();					// Does action
	virtual void			Redo ();				// Does redo action
	virtual void			Undo ();				// Does undo action
	virtual void			Accept					// Don't known
							(cActionVisitor& visitor) { visitor.Visit (*this); };
	virtual const wxString		getHistoryLabel()		// Returns History label wstring
							{return HISTORY_LABEL_CHANGE_PARAM_EFFECT_ACTION;};
	void					SaveDatas();			// Saves mDatas
	void					LoadDatas();			// Loads mDatas
	void					Dump();					// Debug - Draws member variables
  	
	cChangeParamsEffectAction		operator=(const cChangeParamsEffectAction& right);
};

#endif

