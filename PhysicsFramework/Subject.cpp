#include "Subject.h"

void Subject::Notify(Object * eventOrigin, Event * event)
{
	for (int i = 0; i < ObserverList.size(); ++i)
	{
		ObserverList[i]->OnNotify(eventOrigin, event);
	}
}

void Subject::NotifyScoped(Object * eventOrigin, Event * event, Observer * observer)
{
}

Subject::Subject()
{
}


Subject::~Subject()
{
}

void Subject::AddObserver(Observer * observer)
{
	ObserverList.push_back(observer);
}

void Subject::RemoveObserver(Observer * observer)
{
	for (int i = 0; i < ObserverList.size(); ++i)
	{
		// If observer is found in list, erase it from list, shift all following observers to left
		if (observer == ObserverList[i])
		{
			for (int j = i; j < ObserverList.size() - 1; ++j)
			{
				ObserverList[i] = ObserverList[i] + 1;
			}
			break;
		}
	}
}
