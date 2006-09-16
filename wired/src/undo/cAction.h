// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991


#if !defined(ACTION_H)
#define ACTION_H

#include <wx/string.h>

#define	HISTORY_LABEL_PARENT_INSTANCE_ACTION	wxT("ParentInstanceAction")

// ----------------------------------------------------------------------------
// Forward declaration
// ----------------------------------------------------------------------------

class cActionVisitor;
class cActionManager;


// ----------------------------------------------------------------------------
// Defintion classe cAction
// ----------------------------------------------------------------------------

class cAction  
{
protected:
	bool			mValid;						// Indique si l'objet est valide. Par defaut sa valeur est FALSE.
												// Cet indicateur doit est force a TRUE si l'initialisation de 
												// l'action s'est bien passe
	bool			mRegistered;				// Indique si l'action est rattache au gestionnaire des actions ou pas

public:
	int				m_Id;						// Id du menuItem correspondant

	cAction();									// Constructeur
	cAction(const cAction& copy)				// Constructeur par recopie
				{*this = copy;};
	virtual ~cAction() {};						// Destructeur
	
	virtual bool	IsValid () const			// Indique si l'objet est valide ou pas
					{ return mValid; };

	virtual bool	IsRegistered () const		// Indique si l'objet est connu du gestionnaire des actions
					{ return mRegistered; };

	virtual void	NotifyActionManager();		// Enregistre l'action courante dans la liste des actions gerees par le
												// Undo/Redo Manager : cette methode est appelee apres l'execution 
												// correcte de l'action, voir mehode Do()
	virtual void	Do()						// Execute l'action a faire
					{ NotifyActionManager(); };

	virtual void	Undo() {};					// Annule l'action
	virtual void	Redo() { Do(); };			// Refait l'action
	virtual void	Accept						// Methode callbak
					(cActionVisitor& visitor);
	virtual const wxString	getHistoryLabel()		// Retourne la chaine affichee dans la menu Historique Undo / Redo
			{return HISTORY_LABEL_PARENT_INSTANCE_ACTION;};
	void			Dump(bool alone = true);	// Debug - Dump les donnees membres
	
	cAction		operator=(const cAction& right);
};

#endif
