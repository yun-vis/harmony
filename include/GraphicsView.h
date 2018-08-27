#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <map>
#include <list>
#include <vector>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <thread>

using namespace std;

#ifndef Q_MOC_RUN
#include "GraphicsBallItem.h"
#include "GraphicsEdgeItem.h"
#include "Metro.h"
#endif // Q_MOC_RUN

#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGraphicsSceneDragDropEvent>
#include <QtGui/QMouseEvent>
#include <QtCore/QMimeData>
#include <QtSvg/QSvgGenerator>
#include <QtCore/QDir>
#include <QtCore/QTimer>


//------------------------------------------------------------------------------
//	Macro definitions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Class definition
//------------------------------------------------------------------------------
class GraphicsView : public QGraphicsView
{
    Q_OBJECT

private:

    QGraphicsScene      *_scene;
    Metro               *_metro, *_simmetro;

protected:

    void _item_nodes( void );
    void _item_edges( void );

public:
    explicit GraphicsView( QWidget *parent = 0 );
    ~GraphicsView();

//------------------------------------------------------------------------------
//      Reference to members
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//      Specific methods
//------------------------------------------------------------------------------
    void    init                ( Metro * __metro, Metro * __simmetro ){
        _metro = __metro;
        _simmetro = __simmetro;
    }
    void    initSceneItems      ( void );

Q_SIGNALS:

public Q_SLOTS:

};

#endif // GRAPHICSVIEW_H
