#pragma once
#include "Object.h"
#include "Event.h"

// Abstract base class
// An observer is an object that can listen to other objects
class Observer : public Object
{
public:
	virtual ~Observer() {};
	virtual void OnNotify(Object * object, Event * event) = 0;
};

