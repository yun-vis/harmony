
#ifndef WORKERBONE_H
#define WORKERBONE_H

#include <iostream>

#ifndef Q_MOC_RUN
#include "base/Worker.h"
#endif // Q_MOC_RUN

#include <QObject>
#include <QThread>
#include <QTimer>

class WorkerBone : public Worker
{
    Q_OBJECT

private:

protected:

public:

    explicit WorkerBone( void );
    virtual ~WorkerBone();

public Q_SLOTS:

    void onTimeout( void );
    void process( const QString &parameter );

Q_SIGNALS:

};
#endif // WORKERBONE_H
