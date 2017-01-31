#pragma once
// Base type for all events
// All Subjects will define a derived class of Event which owns an enum of events they can generate
class Event
{
public:
	Event();
	virtual ~Event();
};

