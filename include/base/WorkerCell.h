
#ifndef WORKERCELL_H
#define WORKERCELL_H

#include <iostream>

#ifndef Q_MOC_RUN
#include "base/Worker.h"
#endif // Q_MOC_RUN

#include <QObject>
#include <QThread>
#include <QTimer>

class WorkerCell : public Worker
{
    Q_OBJECT

private:

protected:

public:

    explicit WorkerCell( void );
    virtual ~WorkerCell();

public Q_SLOTS:

    void onTimeout( void );
    void process( const QString &parameter );

Q_SIGNALS:

};
#endif // WORKERCELL_H
