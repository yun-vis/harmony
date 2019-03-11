
#ifndef WorkerLevelDetailed_H
#define WorkerLevelDetailed_H

#include <iostream>

#ifndef Q_MOC_RUN
#include "base/Worker.h"
#endif // Q_MOC_RUN

#include <QObject>
#include <QThread>
#include <QTimer>

class WorkerLevelDetailed : public Worker
{
    Q_OBJECT

private:

protected:

public:

    explicit WorkerLevelDetailed( void );
    virtual ~WorkerLevelDetailed();

public Q_SLOTS:

    void onTimeout( void );
    void process( const QString &parameter );

Q_SIGNALS:

};
#endif // WorkerLevelDetailed_H