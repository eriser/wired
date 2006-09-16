// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991


#if !defined(ACTION_MANAGER_H)
#define ACTION_MANAGER_H

#include 						<deque>
#include 						"cImportMidiAction.h"
#include 						<wx/menu.h>

#define	UNDO_LABEL				_("Undo ")
#define	REDO_LABEL				_("Redo ")
#define BEGIN_HISTORY_ID		424242


class	Plugin;

// ----------------------------------------------------------------------------
// Definitions de types
// ----------------------------------------------------------------------------

// Definition de type : liste d'actions
typedef std::deque<cAction *> 		tActionList;

typedef	struct s_PlugStartInfo		PlugStartInfo;
class									PluginLoader;

// Structure MenuInfo
typedef struct	s_menuInfo
{
	string		label;
	int			id;
}				t_menuInfo;

// ----------------------------------------------------------------------------
// Definition de classe cActionManager
// ----------------------------------------------------------------------------

class cActionManager
{
private:
	friend void cAction::NotifyActionManager();

	enum tStackKind { UNDO, REDO };			// Type de listes geres par ce manager

	static cActionManager*	spSingleton;	// Pointer sur l'unique objet de cette classe
	int						nextID;			// Id du menuItem de la prochaine action
	tActionList				mUndoList;		// Liste des actions potentiellement annulables
	int						mUndoCount;		// Nombre d'actions potentiellement annulables
	tActionList				mRedoList;		// Liste des actions potentielles a refaire
    int						mRedoCount;		// Nombre d'actions potentielles a refaire

	cActionManager();												// Constructeur prive
	cActionManager(const cActionManager& copy){*this = copy;};		// Constructeur par recopie prive
		
	bool		CanUndo() const										// Verifie que des actions peuvent etre annulees
				{ return (mUndoCount != 0);};
	bool		CanRedo() const										// Verifie que des actions peuvent etre refaites
				{ return (mRedoCount != 0);};

	void		AddAction(tStackKind stack, cAction* action);		// Ajoute une action d'une liste d'elements
	void		RemoveTopAction(tStackKind stack);					// Supprime une action d'une liste d'elements
	void		RegisterActionManager (cAction* action);			// Ajoute une action dans la liste d'action a annuler ou a refaire
	void		DumptActionList(const tActionList& actionList, 
								const std::string& listName);		// Debug - Dump un tActionList
	cActionManager		operator=(const cActionManager& right);		// Operateur =

public:

	~cActionManager();												// Destructeur 
	static		cActionManager& Global();							// Retourne l'instance unique de cette classe
	bool		Redo();												// Refait la derniere action annulee
	bool		Undo();												// Annule la derniere action effectuee
	bool		Redo(int id);										// Refait la derniere action annulee
	bool		Undo(int id);										// Annule la derniere action effectuee
	void		Dump();												// Debug - Dump les donnees membres
	void		AddEffectAction(PlugStartInfo* startInfo, 			// Adds a cEffectAction in cActionManager
								PluginLoader* plugLoader, 
								bool shouldAdd);
	void		AddChangeParamsEffectAction(Plugin* plugin, bool shouldSave);	// Adds a cChangeParamsEffectAction in cActionManager
	void		AddImportWaveAction(const string& path, bool kind, 
									bool selectFromIndex);				// Adds a cImportWaveAction in cActionManager
	void		AddEditWaveAction(const string& path, bool kind, bool selectFromIndex);
	std::list<t_menuInfo*>		getListActions(int* separatorIndex);	// Retourne la liste des actions Undo

};


#endif
