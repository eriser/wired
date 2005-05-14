
#if !defined(ACTION_H)
#define ACTION_H


#include <string>
#include <list>

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
	cAction();									// Constructeur
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
	void			Dump(bool alone = true);	// Debug - Dump les donnees membres
};

#endif
