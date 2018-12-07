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
#include "optimization/Force.h"
#include "base/Color.h"
#include "base/Boundary.h"
#include "base/Cell.h"
#include "base/Road.h"
#include "base/Pathway.h"
#include "base/PathwayData.h"
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
class GraphicsView : public QGraphicsView, public PathwayData
{
    Q_OBJECT

private:

    bool                _is_simplifiedFlag;
    bool                _is_skeletonFlag, _is_compositeFlag,
                        _is_polygonFlag, _is_polygonComplexFlag,
                        _is_boundaryFlag, _is_subPathwayFlag,
                        _is_cellFlag, _is_cellPolygonFlag, _is_cellPolygonComplexFlag,
                        _is_roadFlag, _is_laneFlag, _is_pathwayPolygonFlag;
    QGraphicsScene      *_scene;
    Boundary            *_boundary, *_simplifiedBoundary;
    Cell                *_cellPtr;
    Road                *_roadPtr;
    vector< Road >      *_lanePtr;

protected:

    void _item_boundary( void );
    void _item_skeleton( void );
    void _item_composite( void );
    void _item_pathways( void );
    void _item_subpathways( void );
    void _item_cells( void );
    void _item_seeds( void );
    void _item_polygonComplex( void );
    void _item_polygons( void );
    void _item_cellPolygons( void );
    void _item_cellPolygonComplex( void );
    void _item_interCellComponents( void );
    void _item_pathwayPolygons( void );
    void _item_road( void );
    void _item_lane( void );

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
    bool &          isBoundaryFlag( void )          { return _is_boundaryFlag; }
    const bool &    isBoundaryFlag( void ) const    { return _is_boundaryFlag; }
    bool &          isSubPathwayFlag( void )        { return _is_subPathwayFlag; }
    const bool &    isSubPathwayFlag( void ) const  { return _is_subPathwayFlag; }
    bool &          isPathwayPolygonFlag( void )        { return _is_pathwayPolygonFlag; }
    const bool &    isPathwayPolygonFlag( void ) const  { return _is_pathwayPolygonFlag; }
    bool &          isCellFlag( void )              { return _is_cellFlag; }
    const bool &    isCellFlag( void ) const        { return _is_cellFlag; }
    bool &          isCellPolygonFlag( void )       { return _is_cellPolygonFlag; }
    const bool &    isCellPolygonFlag( void ) const { return _is_cellPolygonFlag; }
    bool &          isCellPolygonComplexFlag( void )       { return _is_cellPolygonComplexFlag; }
    const bool &    isCellPolygonComplexFlag( void ) const { return _is_cellPolygonComplexFlag; }

    bool &          isRoadFlag( void )              { return _is_roadFlag; }
    const bool &    isRoadFlag( void ) const        { return _is_roadFlag; }
    bool &          isLaneFlag( void )              { return _is_laneFlag; }
    const bool &    isLaneFlag( void ) const        { return _is_laneFlag; }

//------------------------------------------------------------------------------
//      Specific methods
//------------------------------------------------------------------------------
    void    init                ( Boundary * __boundary, Boundary * __simplifiedBoundary,
                                  Cell * __cellPtr,
                                  Road * __roadPtr, vector< Road > * __lanePtr ){
        _boundary = __boundary;
        _simplifiedBoundary = __simplifiedBoundary;
        _cellPtr = __cellPtr;
        _roadPtr = __roadPtr;
        _lanePtr = __lanePtr;
    }
    void    initPolygonItems    ( void );
    void    initSceneItems      ( void );

    void exportPNG ( double x, double y, double w, double h );

Q_SIGNALS:

public Q_SLOTS:

};

#endif // GRAPHICSVIEW_H
