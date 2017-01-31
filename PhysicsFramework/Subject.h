#pragma once
#include <vector>

#include "Object.h"
#include "Observer.h"

// Utility class
// Any event than can be observed is called a subject
class Subject
{
	/*----------MEMBER FUNCTIONS----------*/
public:
	Subject();
	virtual ~Subject();

	// Notifies all observers
	void Notify(Object * eventOrigin, Event * event);
	// Notifies specified observer
	void NotifyScoped(Object * eventOrigin, Event * event, Observer * observer);
	void AddObserver(Observer* observer);
	void RemoveObserver(Observer* observer);

	/*----------MEMBER VARIABLES----------*/
private:
	std::vector<Observer *> ObserverList;

};

