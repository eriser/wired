
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
	// Indique si l'objet est valide
	// Par defaut sa valeur est FALSE
	// Cet indicateur doit est force a TRUE 
	// si l'initialisation de l'action s'est bien passe
	bool mValid;

	// Indique si l'action est rattache au gestionnaire des actions ou pas
	bool mRegistered;

public:
	// Constructeur
	cAction();

	// Destructeur
	virtual ~cAction() 
	{};

	// Indique si l'objet est valide ou pas
	virtual bool IsValid () const
	{ return mValid; };

	// Indique si l'objet est connu du gestionnaire des actions
	virtual bool IsRegistered () const
	{ return mRegistered; };

	// Enregistre l'action courante dans la liste des actions gerees par le
	// Undo/Redo Manager : cette methode est appelee apres l'execution 
	// correcte de l'action, voir mehode Do()
	virtual void NotifyActionManager();

	// Execute l'action a faire
	virtual void Do()
	{ NotifyActionManager(); };

	// Annule l'action
	virtual void Undo()
	{};

	// Refait l'action
	virtual void Redo()
	{ Do(); };

	// Methode callbak
	virtual void Accept (cActionVisitor& visitor);
};

#endif
