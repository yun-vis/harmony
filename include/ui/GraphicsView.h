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
#include "ui/GraphicsBallItem.h"
#include "ui/GraphicsEdgeItem.h"
#include "ui/GraphicsPolygonItem.h"
#include "base/Boundary.h"
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

    bool                _is_simplifiedFlag;
    bool                _is_polygonFlag;
    QGraphicsScene      *_scene;
    Boundary            *_boundary, *_simplifiedBoundary;

protected:

    void _item_seeds( void );
    void _item_polygons( void );
    void _item_nodes( void );
    void _item_edges( void );

public:
    explicit GraphicsView( QWidget *parent = 0 );
    ~GraphicsView();

//------------------------------------------------------------------------------
//      Reference to members
//------------------------------------------------------------------------------
    bool &          isSimplifiedFlag( void )        { return _is_simplifiedFlag; }
    const bool &    isSimplifiedFlag( void ) const  { return _is_simplifiedFlag; }

    bool &          isPolygonFlag( void )           { return _is_polygonFlag; }
    const bool &    isPolygonFlag( void ) const     { return _is_polygonFlag; }

//------------------------------------------------------------------------------
//      Specific methods
//------------------------------------------------------------------------------
    void    init                ( Boundary * __boundary, Boundary * __simplifiedBoundary ){
        _boundary = __boundary;
        _simplifiedBoundary = __simplifiedBoundary;
    }
    void    initSceneItems      ( void );

Q_SIGNALS:

public Q_SLOTS:

};

#endif // GRAPHICSVIEW_H
