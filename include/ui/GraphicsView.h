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

#include "base/Color.h"
#include "base/LevelCell.h"
#include "base/Road.h"
#include "base/Pathway.h"
#include "base/PathwayData.h"
#include "base/RegionData.h"
#include "ui/GraphicsVertexItem.h"
#include "ui/GraphicsEdgeItem.h"
#include "ui/GraphicsPolygonItem.h"
#include "optimization/Force.h"

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
class GraphicsView : public QGraphicsView, public PathwayData, public RegionData {
Q_OBJECT

private:
	
	QGraphicsScene *_scene;
	
	// data structure
	bool _is_simplifiedFlag;
	int _clonedThreshold;           // if == 0, cloned by type, else by threshold value
	string _inputpath, _tmppath,       // input output files
	_fileFreq, _fileType;       // metabolite frequency
	ENERGYTYPE _energyType;
	COLORTYPE _colorType;
	unsigned int _maxThread;                 // maximum thread number
	
	// ui
	int _font_size;
	double _vertex_edge_ratio, _vertex_edge_coverage;
	bool _is_skeletonFlag, _is_compositeFlag,
			_is_polygonFlag, _is_polygonComplexFlag,
			_is_boundaryFlag, _is_subPathwayFlag,
			_is_cellFlag, _is_cellPolygonFlag, _is_cellPolygonComplexFlag,
			_is_roadFlag, _is_laneFlag,
			_is_centerFlag, _is_centerPolygonFlag,
			_is_pathwayPolygonFlag, _is_pathwayPolygonContourFlag;
	
	void _init( void );
	
protected:
	
	void _item_boundary( void );
	
	void _update_item_boundary( void );
	
	void _item_skeleton( void );
	
	void _update_item_skeleton( void );
	
	void _item_composite( void );
	
	void _update_item_composite( void );
	
	void _item_pathways( void );
	
	void _item_subpathways( void );
	
	void _update_item_subpathways( void );
	
	void _item_centers( void );
	
	void _update_item_centers( void );
	
	void _item_centerPolygons( void );
	
	void _update_item_centerPolygons( void );
	
	void _item_cells( void );
	
	void _update_item_cells( void );
	
	void _item_seeds( void );
	
	void _item_polygonComplex( void );
	
	void _update_item_polygonComplex( void );
	
	void _item_polygons( void );
	
	void _update_item_polygons( void );
	
	void _item_cellPolygons( void );
	
	void _update_item_cellPolygons( void );
	
	void _item_cellPolygonComplex( bool fineFlag );
	
	void _update_item_cellPolygonComplex( bool fineFlag );
	
	void _item_interCellComponents( void );
	
	void _update_item_interCellComponents( void );
	
	void _item_pathwayPolygons( void );
	
	void _update_item_pathwayPolygons( void );
	
	void _item_road( void );
	
	void _item_lane( void );

public:
	explicit GraphicsView( QWidget *parent = 0 );
	
	~GraphicsView();

//------------------------------------------------------------------------------
//      Reference to members
//------------------------------------------------------------------------------
	int &cloneThreshold( void ) { return _clonedThreshold; }
	
	const int &cloneThreshold( void ) const { return _clonedThreshold; }
	
	string &inputPath( void ) { return _inputpath; }
	
	const string &inputPath( void ) const { return _inputpath; }
	
	string &tmpPath( void ) { return _tmppath; }
	
	const string &tmpPath( void ) const { return _tmppath; }
	
	string &fileFreq( void ) { return _fileFreq; }
	
	const string &fileFreq( void ) const { return _fileFreq; }
	
	string &fileType( void ) { return _fileType; }
	
	const string &fileType( void ) const { return _fileType; }
	
	ENERGYTYPE &energyType( void ) { return _energyType; }
	
	const ENERGYTYPE &energyType( void ) const { return _energyType; }
	
	int &fontSize( void ) { return _font_size; }
	
	const int &fontSize( void ) const { return _font_size; }
	
	double &veRatio( void ) { return _vertex_edge_ratio; }
	
	const double &veRatio( void ) const { return _vertex_edge_ratio; }
	
	double &veCoverage( void ) { return _vertex_edge_coverage; }
	
	const double &veCoverage( void ) const { return _vertex_edge_coverage; }
	
	bool &isSimplifiedFlag( void ) { return _is_simplifiedFlag; }
	
	const bool &isSimplifiedFlag( void ) const { return _is_simplifiedFlag; }
	
	bool &isSkeletonFlag( void ) { return _is_skeletonFlag; }
	
	const bool &isSkeletonFlag( void ) const { return _is_skeletonFlag; }
	
	bool &isCompositeFlag( void ) { return _is_compositeFlag; }
	
	const bool &isCompositeFlag( void ) const { return _is_compositeFlag; }
	
	bool &isPolygonFlag( void ) { return _is_polygonFlag; }
	
	const bool &isPolygonFlag( void ) const { return _is_polygonFlag; }
	
	bool &isPolygonComplexFlag( void ) { return _is_polygonComplexFlag; }
	
	const bool &isPolygonComplexFlag( void ) const { return _is_polygonComplexFlag; }
	
	bool &isBoundaryFlag( void ) { return _is_boundaryFlag; }
	
	const bool &isBoundaryFlag( void ) const { return _is_boundaryFlag; }
	
	bool &isSubPathwayFlag( void ) { return _is_subPathwayFlag; }
	
	const bool &isSubPathwayFlag( void ) const { return _is_subPathwayFlag; }
	
	bool &isCenterPolygonFlag( void ) { return _is_centerPolygonFlag; }
	
	const bool &isCenterPolygonFlag( void ) const { return _is_centerPolygonFlag; }
	
	bool &isCenterFlag( void ) { return _is_centerFlag; }
	
	const bool &isCenterFlag( void ) const { return _is_centerFlag; }
	
	bool &isPathwayPolygonFlag( void ) { return _is_pathwayPolygonFlag; }
	
	const bool &isPathwayPolygonFlag( void ) const { return _is_pathwayPolygonFlag; }
	
	bool &isPathwayPolygonContourFlag( void ) { return _is_pathwayPolygonContourFlag; }
	
	const bool &isPathwayPolygonContourFlag( void ) const { return _is_pathwayPolygonContourFlag; }
	
	bool &isCellFlag( void ) { return _is_cellFlag; }
	
	const bool &isCellFlag( void ) const { return _is_cellFlag; }
	
	bool &isCellPolygonFlag( void ) { return _is_cellPolygonFlag; }
	
	const bool &isCellPolygonFlag( void ) const { return _is_cellPolygonFlag; }
	
	bool &isCellPolygonComplexFlag( void ) { return _is_cellPolygonComplexFlag; }
	
	const bool &isCellPolygonComplexFlag( void ) const { return _is_cellPolygonComplexFlag; }
	
	bool &isRoadFlag( void ) { return _is_roadFlag; }
	
	const bool &isRoadFlag( void ) const { return _is_roadFlag; }
	
	bool &isLaneFlag( void ) { return _is_laneFlag; }
	
	const bool &isLaneFlag( void ) const { return _is_laneFlag; }
	
	unsigned int &maxThread( void ) { return _maxThread; }
	
	const unsigned int &maxThread( void ) const { return _maxThread; }

//------------------------------------------------------------------------------
//      Specific methods
//------------------------------------------------------------------------------
	void init( void ) { _init(); }
	
	void initPolygonItems( void );
	
	void initSceneItems( void );
	
	void updateSceneItems( void );
	
	void exportPNG( double x, double y, double w, double h );
	
	void exportSVG( double x, double y, double w, double h );

Q_SIGNALS:

public Q_SLOTS:

};

#endif // GRAPHICSVIEW_H
