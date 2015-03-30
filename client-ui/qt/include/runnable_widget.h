#ifndef RUNNABLE_WIDGET_H
#define RUNNABLE_WIDGET_H

#include "jansson.h"

class RunnableWidget
{
public:
	virtual json_t *GetUserValuesAsJSON (bool full_flag) = 0;
};

#endif // RUNNABLE_WIDGET_H
