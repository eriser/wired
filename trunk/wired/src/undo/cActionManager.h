
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

	// Type de listes geres par ce manager
	enum tStackKind { UNDO, REDO };

	// Liste des actions a annuler
	tActionList mUndoList;

    // Liste des actions a annuler
	int mUndoCount;

	// Liste des actions a refaire
	tActionList mRedoList;

    // Liste des actions a annuler
	int mRedoCount;

	// Pointer sur l'unique objet de cette classe
	static cActionManager* spSingleton;

	// Constructeur prive
	cActionManager();

	// Ajoute une action dans la liste d'action a annuler ou a refaire
	void RegisterActionManager (cAction* action) ;

	// Verifie que des actions peuvent etre annulées
	bool CanUndo() const
	{ return (mUndoCount != 0); /*(mUndoList.empty() == false);*/ };

	// Verifie que des actions peuvent etre refaites
	bool CanRedo() const
	{ return (mRedoCount != 0); /*(mRedoList.empty() == false);*/ };

	// Supprime une action d'une liste d'elements
	void AddAction(tStackKind stack, cAction& action);

	// Supprime une action d'une liste d'elements
	void RemoveTopAction(tStackKind stack);

public:
	// Destructeur 
	~cActionManager();
    
	// Retourne l'instance unique de cette classe
	static cActionManager& Global();

	// Refait la derniere action annulee
	bool Redo();

	// Annule la derniere action effectuee
	bool Undo();
};


#endif
