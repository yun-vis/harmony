
#ifndef WorkerLevelLow_H
#define WorkerLevelLow_H

#include <iostream>

#ifndef Q_MOC_RUN
#include "base/Worker.h"
#endif // Q_MOC_RUN

#include <QObject>
#include <QThread>
#include <QTimer>

class WorkerLevelLow : public Worker
{
    Q_OBJECT

private:

protected:

public:

    explicit WorkerLevelLow( void );
    virtual ~WorkerLevelLow();

public Q_SLOTS:

    void onTimeout( void );
    void process( const QString &parameter );

Q_SIGNALS:

};
#endif // WorkerLevelLow_H
