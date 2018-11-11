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
#include "base/Color.h"

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
    bool                _is_skeletonFlag, _is_compositeFlag,
                        _is_polygonFlag, _is_polygonComplexFlag;
    QGraphicsScene      *_scene;
    Boundary            *_boundary, *_simplifiedBoundary;

protected:

    void _item_skeleton( void );
    void _item_composite( void );
    void _item_seeds( void );
    void _item_polygonComplex( void );
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

    bool &          isSkeletonFlag( void )          { return _is_skeletonFlag; }
    const bool &    isSkeletonFlag( void ) const    { return _is_skeletonFlag; }
    bool &          isCompositeFlag( void )         { return _is_compositeFlag; }
    const bool &    isCompositeFlag( void ) const   { return _is_compositeFlag; }
    bool &          isPolygonFlag( void )           { return _is_polygonFlag; }
    const bool &    isPolygonFlag( void ) const     { return _is_polygonFlag; }
    bool &          isPolygonComplexFlag( void )           { return _is_polygonComplexFlag; }
    const bool &    isPolygonComplexFlag( void ) const     { return _is_polygonComplexFlag; }

//------------------------------------------------------------------------------
//      Specific methods
//------------------------------------------------------------------------------
    void    init                ( Boundary * __boundary, Boundary * __simplifiedBoundary ){
        _boundary = __boundary;
        _simplifiedBoundary = __simplifiedBoundary;
    }
    void    initPolygonItems    ( void );
    void    initSceneItems      ( void );

    void exportPNG ( double x, double y, double w, double h );

Q_SIGNALS:

public Q_SLOTS:

};

#endif // GRAPHICSVIEW_H
