#ifndef __h_timedevent
#define __h_timedevent

class TimedEvent
{
    public:
        unsigned int start;
        unsigned int delay;
        bool triggered;
        TimedEvent();
        ~TimedEvent();
};

#endif