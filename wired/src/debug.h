// Copyright (C) 2004-2007 by Wired Team
// Under the GNU General Public License Version 2, June 1991
#ifndef _DEBUG_H_
#define _DEBUG_H_

//#define DEBUG_1561088			//Copying/Pasting racks doesn't work 
#define DEBUG_1647295			//Crash on MediaLibrary sort
#define DEBUG_DISABLE_FILTERS		//Filters in the ML are useless
#define DEBUG_DISABLE_CLEAN_MENUS	//These menu entries use CleanChildren which is bugged in the sequencer...
#define FEATURE_DISABLE_SAVE_ML		//Should be reenabled when the ML will be able to export the local tree along with a copy of all its wav files...
					//It has to be thought as an 'export ML' and 'import ML'
#define FEATURE_DISABLE_WAVE_EDITOR	//The WaveEditor is completely bugged and has been removed for the PFE Forum 2006.
#endif
