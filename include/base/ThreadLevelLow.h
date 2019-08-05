
#ifndef ThreadLevelLow_H
#define ThreadLevelLow_H

#include <iostream>

#include "base/ThreadBase.h"

class ThreadLevelLow : public ThreadBase
{
private:


protected:

public:

    explicit ThreadLevelLow( void );
    virtual ~ThreadLevelLow();

    virtual const char * className( void ) const { return "ThreadLevelLow"; }

    void force( void );
    void stress( void );
    void run( int id );
    //void run( int id, string &name );

};
#endif // ThreadLevelLow_H
