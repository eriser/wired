
#if !defined(ACTION_MANAGER_H)
#define ACTION_MANAGER_H


#include <deque>
#include "cAction.h"


// ----------------------------------------------------------------------------
// Definitions de types
// ----------------------------------------------------------------------------

// Definition de type : liste d'actions
typedef std::deque<cAction *> tActionList;


// ----------------------------------------------------------------------------
// Definition de classe cActionManager
// ----------------------------------------------------------------------------

class cActionManager 
{
private:
	friend void cAction::NotifyActionManager();

	enum tStackKind { UNDO, REDO };			// Type de listes geres par ce manager

	static	cActionManager* spSingleton;	// Pointer sur l'unique objet de cette classe
	tActionList	mUndoList;					// Liste des actions potentiellement annulables
	int			mUndoCount;					// Nombre d'actions potentiellement annulables
	tActionList	mRedoList;					// Liste des actions potentielles a refaire
    int			mRedoCount;					// Nombre d'actions potentielles a refaire

	cActionManager();						// Constructeur prive
	
	bool		CanUndo() const				// Verifie que des actions peuvent etre annulees
				{ return (mUndoCount != 0);};
	bool		CanRedo() const				// Verifie que des actions peuvent etre refaites
				{ return (mRedoCount != 0);};

	void		AddAction(tStackKind stack, cAction& action);	// Ajoute une action d'une liste d'elements
	void		RemoveTopAction(tStackKind stack);				// Supprime une action d'une liste d'elements
	void		RegisterActionManager (cAction* action);		// Ajoute une action dans la liste d'action a annuler ou a refaire
	void		DumptActionList(const tActionList& actionList, 
								const std::string& listName);	// Debug - Dump un tActionList

public:

	~cActionManager();						// Destructeur 
	static		cActionManager& Global();	// Retourne l'instance unique de cette classe
	bool		Redo();						// Refait la derniere action annulee
	bool		Undo();						// Annule la derniere action effectuee
	void		Dump();						// Debug - Dump les donnees membres
};


#endif
