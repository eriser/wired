// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

#ifndef __CLIPBOARD_H__
#define __CLIPBOARD_H__

#include "WaveFile.h"

class cClipBoard : public WaveFile
{
public:

	// Destructeur 
	~cClipBoard();
    
	// Retourne l'instance unique de cette classe
	static cClipBoard& Global();

	// Copie des frames
	void Copy (WaveFile& wave, int from, int size_of_copy);

	// Copie de frames dans le clipboard et suppression du wave source
	void Cut (WaveFile& wave, int from, int size_of_copy);

	// Insertion de frames dans un wave
	void Paste (WaveFile& wave, int to);
	
	// Suppression de frames dans un wave
	void Delete (WaveFile& wave, int from, int size_of_cut);

	long sizec;

private:
	// Pointer sur l'unique objet de cette classe
	static cClipBoard* spSingleton;

	// Constructeur prive
	// cClipBoard();
	cClipBoard(wxString filename = wxT("/tmp/tmp.wav"), bool loadmem = false, t_opening_mode open_mode = WaveFile::rwrite);
};


#endif
