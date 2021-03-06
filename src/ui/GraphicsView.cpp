
#include "ui/GraphicsView.h"

//------------------------------------------------------------------------------
//	Macro definition
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Protected functions
//------------------------------------------------------------------------------
void GraphicsView::_init( void ) {
/*
	_vertex_edge_coverage = _pathwayPtr->nVertices() + _vertex_edge_ratio * sqrt( _pathwayPtr->nEdges() );
	cerr << "_vertex_edge_ratio = " << _vertex_edge_ratio
        << "_pathwayPtr->nVertices() = " << _pathwayPtr->nVertices()
        << " _pathwayPtr->nEdges() = " << _pathwayPtr->nEdges()
	    << " _vertex_edge_coverage = " << _vertex_edge_coverage << endl;
*/
}

void GraphicsView::_item_force_graph( ForceGraph &g ) {
	
	// draw edges
	BGL_FORALL_EDGES( ed, g, ForceGraph ) {
			
			ForceGraph::vertex_descriptor vdS = source( ed, g );
			ForceGraph::vertex_descriptor vdT = target( ed, g );
			QPainterPath path;
			path.moveTo( g[ vdS ].coordPtr->x(), -g[ vdS ].coordPtr->y() );
			path.lineTo( g[ vdT ].coordPtr->x(), -g[ vdT ].coordPtr->y() );
			
			GraphicsEdgeItem *itemptr = new GraphicsEdgeItem;
			g[ ed ].itemPtr = itemptr;
			itemptr->id() = g[ ed ].id;
			itemptr->setPen( QPen( QColor( 0, 0, 0, 100 ), 2 ) );
			itemptr->setBrush( QBrush( QColor( 255, 255, 255, 255 ), Qt::SolidPattern ) );
			itemptr->setPath( path );
			itemptr->weight() = g[ ed ].weight;
			
			_scene->addItem( itemptr );
		}
	
	BGL_FORALL_VERTICES( vd, g, ForceGraph ) {
			
			GraphicsVertexItem *itemptr = new GraphicsVertexItem;
			g[ vd ].itemPtr = itemptr;
			itemptr->id() = g[ vd ].id;
			itemptr->fontSize() = Common::getFontSize();
			itemptr->setPen( QPen( QColor( 0, 0, 0, 100 ), 2 ) );
			itemptr->setBrush( QBrush( QColor( 255, 255, 255, 255 ), Qt::SolidPattern ) );
			itemptr->setRect( QRectF( g[ vd ].coordPtr->x(), -g[ vd ].coordPtr->y(), 10, 10 ) );
			itemptr->text() = QString::fromStdString( to_string( g[ vd ].id ) );
			//itemptr->textOn() = true;
			
			_scene->addItem( itemptr );
		}
}

void GraphicsView::_update_item_force_graph( ForceGraph &g ) {
	
	// draw edges
	BGL_FORALL_EDGES( ed, g, ForceGraph ) {
			
			ForceGraph::vertex_descriptor vdS = source( ed, g );
			ForceGraph::vertex_descriptor vdT = target( ed, g );
			QPainterPath path;
			path.moveTo( g[ vdS ].coordPtr->x(), -g[ vdS ].coordPtr->y() );
			path.lineTo( g[ vdT ].coordPtr->x(), -g[ vdT ].coordPtr->y() );
			
			GraphicsEdgeItem *itemptr = g[ ed ].itemPtr;
			//itemptr->id() = g[ ed ].id;
			//itemptr->setPen( QPen( QColor( 0, 0, 0, 100 ), 2 ) );
			//itemptr->setBrush( QBrush( QColor( 255, 255, 255, 255 ), Qt::SolidPattern ) );
			itemptr->setPath( path );
			//itemptr->weight() = g[ ed ].weight;
			
			//_scene->addItem( itemptr );
		}
	
	BGL_FORALL_VERTICES( vd, g, ForceGraph ) {
			
			GraphicsVertexItem *itemptr = g[ vd ].itemPtr;
			//itemptr->id() = g[ vd ].id;
			//itemptr->fontSize() = Common::getFontSize();
			//itemptr->setPen( QPen( QColor( 0, 0, 0, 100 ), 2 ) );
			//itemptr->setBrush( QBrush( QColor( 255, 255, 255, 255 ), Qt::SolidPattern ) );
			itemptr->setRect( QRectF( g[ vd ].coordPtr->x(), -g[ vd ].coordPtr->y(), 10, 10 ) );
			//itemptr->text() = QString::fromStdString( to_string( g[ vd ].id ) );
			//itemptr->textOn() = true;
			
			//_scene->addItem( itemptr );
		}
}

void GraphicsView::_item_skeleton( void ) {
	
	ForceGraph &s = _levelBorderPtr->skeletonForceGraph();
	_item_force_graph( s );
}

void GraphicsView::_update_item_skeleton( void ) {
	
	ForceGraph &s = _levelBorderPtr->skeletonForceGraph();
	_update_item_force_graph( s );
}

void GraphicsView::_item_composite( void ) {
	
	ForceGraph &s = _levelBorderPtr->regionBase().forceGraph();
	_item_force_graph( s );
}

void GraphicsView::_update_item_composite( void ) {
	
	ForceGraph &s = _levelBorderPtr->regionBase().forceGraph();
	_update_item_force_graph( s );
}

void GraphicsView::_item_polygon( Polygon2 &p ) {
	
	QPolygonF polygon;
	for( unsigned int j = 0; j < p.elements().size(); j++ ) {
		polygon.append( QPointF( p.elements()[ j ].x(), -p.elements()[ j ].y() ) );
	}
	
	GraphicsPolygonItem *itemptr = new GraphicsPolygonItem;
	p.setItemPtr( itemptr );
	itemptr->setPolygon( polygon );
	
	_scene->addItem( itemptr );
}

void GraphicsView::_update_item_polygon( Polygon2 &p ) {
	
	QPolygonF polygon;
	for( unsigned int j = 0; j < p.elements().size(); j++ ) {
		polygon.append( QPointF( p.elements()[ j ].x(), -p.elements()[ j ].y() ) );
	}
	
	GraphicsPolygonItem *itemptr = p.getItemPtr();
	p.setItemPtr( itemptr );
	itemptr->setPolygon( polygon );
}

void GraphicsView::_item_polygonComplex( void ) {
	
	ForceGraph &s = _levelBorderPtr->skeletonForceGraph();
	
	map< unsigned int, Polygon2 > p = _levelBorderPtr->regionBase().polygonComplex();
	map< unsigned int, Polygon2 >::iterator itP = p.begin();
	
	for( ; itP != p.end(); itP++ ) {
		
		Polygon2 &p = itP->second;
		_item_polygon( p );
		
		GraphicsPolygonItem *itemptr = p.getItemPtr();
		
		vector< double > rgb;
		ForceGraph::vertex_descriptor vd = vertex( itP->first, s );
		unsigned int gid = s[ vd ].initID;
		_pathwayPtr->pickColor( _colorType, gid, rgb );
		
		QColor color( rgb[ 0 ], rgb[ 1 ], rgb[ 2 ], 100 );
		itemptr->setPen( QPen( QColor( color.red(), color.green(), color.blue(), 255 ), 2 ) );
		itemptr->setBrush( QBrush( QColor( color.red(), color.green(), color.blue(), 100 ), Qt::SolidPattern ) );
	}
}

void GraphicsView::_update_item_polygonComplex( void ) {
	
	ForceGraph &s = _levelBorderPtr->skeletonForceGraph();
	
	//vector < vector< Coord2 > > p = _levelBorderPtr->polygons();
	map< unsigned int, Polygon2 > p = _levelBorderPtr->regionBase().polygonComplex();
	map< unsigned int, Polygon2 >::iterator itP = p.begin();
	for( ; itP != p.end(); itP++ ) {
		
		Polygon2 &p = itP->second;
		_item_polygon( p );
		
		// GraphicsPolygonItem *itemptr = p.getItemPtr();
		//vector< double > rgb;
		//ForceGraph::vertex_descriptor vd = vertex( itP->first, s );
		//unsigned int gid = s[vd].initID;
		
		// pickBrewerColor( gid, rgb );
		//_pathwayPtr->pickColor( _colorType, gid, rgb );
		
		//QColor color( rgb[0], rgb[1], rgb[2], 100 );
		//itemptr->setPen( QPen( QColor( color.red(), color.green(), color.blue(), 255 ), 2 ) );
		//itemptr->setBrush( QBrush( QColor( color.red(), color.green(), color.blue(), 100 ), Qt::SolidPattern ) );
		//itemptr->setPolygon( polygon );
		
		//cerr << vertexCoord[vd];
		//_scene->addItem( itemptr );
	}
	// cerr << "polygon.size() = " << p.size() << endl;
}

void GraphicsView::_item_composite_polygons( void ) {
	
	ForceGraph &fg = _levelBorderPtr->regionBase().forceGraph();
	
	BGL_FORALL_VERTICES( vd, fg, ForceGraph ) {
			
			Polygon2 &p = *fg[ vd ].cellPtr;
			_item_polygon( p );
			GraphicsPolygonItem *itemptr = p.getItemPtr();
			
			vector< double > rgb;
			unsigned int gid = fg[ vd ].initID;
			_pathwayPtr->pickColor( _colorType, gid, rgb );
			
			QColor color( rgb[ 0 ], rgb[ 1 ], rgb[ 2 ], 100 );
			itemptr->setPen( QPen( QColor( color.red(), color.green(), color.blue(), 255 ), 2 ) );
			itemptr->setBrush( QBrush( QColor( color.red(), color.green(), color.blue(), 100 ), Qt::SolidPattern ) );
		}
}

void GraphicsView::_update_item_composite_polygons( void ) {
	
	ForceGraph &fg = _levelBorderPtr->regionBase().forceGraph();
	
	BGL_FORALL_VERTICES( vd, fg, ForceGraph ) {
			
			Polygon2 &p = *fg[ vd ].cellPtr;
			_update_item_polygon( p );
			
			//GraphicsPolygonItem *itemptr = p.getItemPtr();
			
			//vector< double > rgb;
			// ForceGraph::vertex_descriptor vd = vertex( i, s );
			
			//unsigned int gid = fg[ vd ].initID;
			//_pathwayPtr->pickColor( _colorType, gid, rgb );
			//cerr << rgb[0] << ", " << rgb[1] << ", " << rgb[2] << endl;
			//QColor color( rgb[ 0 ], rgb[ 1 ], rgb[ 2 ], 100 );
			//itemptr->setPen( QPen( QColor( color.red(), color.green(), color.blue(), 255 ), 2 ) );
			//itemptr->setBrush( QBrush( QColor( color.red(), color.green(), color.blue(), 100 ), Qt::SolidPattern ) );
			//itemptr->setPolygon( polygon );
			
			//cerr << vertexCoord[vd];
			//_scene->addItem( itemptr );
		}
}

void GraphicsView::_item_boundary( void ) {
	
	vector< Octilinear * > *boundaryVecPtr = NULL;
	if( *_levelTypePtr == LEVEL_BORDER ) {
		boundaryVecPtr = &_levelBorderPtr->octilinearBoundaryVec();
	}
	else if( *_levelTypePtr == LEVEL_CELLCENTER ) {
		boundaryVecPtr = &_levelBorderPtr->octilinearBoundaryVec();
	}
	else if( *_levelTypePtr == LEVEL_CELLCOMPONENT ) {
		boundaryVecPtr = &_levelCellPtr->octilinearBoundaryVec();
	}
	else if( *_levelTypePtr == LEVEL_DETAIL ) {
		boundaryVecPtr = &_levelDetailPtr->octilinearBoundaryVec();
	}
	else if( *_levelTypePtr == LEVEL_ROUTE ) {
		//boundaryVecPtr = &_levelBorderPtr->octilinearBoundaryVec();
		boundaryVecPtr = &_levelDetailPtr->octilinearBoundaryVec();
	}
	else {
		cerr << "sth is wrong here... at " << __LINE__ << " in " << __FILE__ << endl;
	}
	
	if( boundaryVecPtr == NULL ) return;
	
	vector< Octilinear * > &boundaryVec = *boundaryVecPtr;
	
	for( unsigned int i = 0; i < boundaryVec.size(); i++ ) {
		
		BoundaryGraph &g = boundaryVec[ i ]->boundary();
		
		// draw edges
		BGL_FORALL_EDGES( ed, g, BoundaryGraph ) {
				
				BoundaryGraph::vertex_descriptor vdS = source( ed, g );
				BoundaryGraph::vertex_descriptor vdT = target( ed, g );
				QPainterPath path;
				path.moveTo( g[ vdS ].coordPtr->x(), -g[ vdS ].coordPtr->y() );
				path.lineTo( g[ vdT ].coordPtr->x(), -g[ vdT ].coordPtr->y() );
				
				GraphicsEdgeItem *itemptr = new GraphicsEdgeItem;
				g[ ed ].itemPtr = itemptr;
				
				itemptr->setPen( QPen( QColor( 100, 100, 100, 255 ), 2 ) );
				itemptr->setBrush( QBrush( QColor( 100, 100, 100, 255 ), Qt::SolidPattern ) );
				itemptr->setPath( path );
				itemptr->id() = g[ ed ].id;
				//itemptr->textOn() = true;
				
				_scene->addItem( itemptr );
			}
		
		// draw vertices
		BGL_FORALL_VERTICES( vd, g, BoundaryGraph ) {
				
				GraphicsVertexItem *itemptr = new GraphicsVertexItem;
				g[ vd ].itemPtr = itemptr;
				
				itemptr->fontSize() = Common::getFontSize();
				itemptr->setPen( QPen( QColor( 100, 100, 100, 255 ), 2 ) );
				itemptr->setBrush( QBrush( QColor( 100, 100, 100, 255 ), Qt::SolidPattern ) );
				itemptr->setRect( QRectF( g[ vd ].coordPtr->x(), -g[ vd ].coordPtr->y(), 10, 10 ) );
				itemptr->id() = g[ vd ].id;
				itemptr->text() = QString::fromStdString( to_string( g[ vd ].isFixed ) );
				//itemptr->textOn() = true;
				
				//cerr << vertexCoord[vd];
				_scene->addItem( itemptr );
			}
	}
}

void GraphicsView::_update_item_boundary( void ) {
	
	vector< Octilinear * > *boundaryVecPtr = NULL;
	if( *_levelTypePtr == LEVEL_BORDER ) {
		boundaryVecPtr = &_levelBorderPtr->octilinearBoundaryVec();
	}
	else if( *_levelTypePtr == LEVEL_CELLCENTER ) {
		boundaryVecPtr = &_levelBorderPtr->octilinearBoundaryVec();
	}
	else if( *_levelTypePtr == LEVEL_CELLCOMPONENT ) {
		boundaryVecPtr = &_levelCellPtr->octilinearBoundaryVec();
	}
	else if( *_levelTypePtr == LEVEL_DETAIL ) {
		boundaryVecPtr = &_levelDetailPtr->octilinearBoundaryVec();
	}
	else if( *_levelTypePtr == LEVEL_ROUTE ) {
		//boundaryVecPtr = &_levelBorderPtr->octilinearBoundaryVec();
		boundaryVecPtr = &_levelDetailPtr->octilinearBoundaryVec();
	}
	else {
		cerr << "sth is wrong here... at " << __LINE__ << " in " << __FILE__ << endl;
	}
	
	if( boundaryVecPtr == NULL ) return;
	
	vector< Octilinear * > &boundaryVec = *boundaryVecPtr;
	
	for( unsigned int i = 0; i < boundaryVec.size(); i++ ) {
		
		BoundaryGraph &g = boundaryVec[ i ]->boundary();
		
		// draw edges
		BGL_FORALL_EDGES( ed, g, BoundaryGraph ) {
				
				BoundaryGraph::vertex_descriptor vdS = source( ed, g );
				BoundaryGraph::vertex_descriptor vdT = target( ed, g );
				QPainterPath path;
				path.moveTo( g[ vdS ].coordPtr->x(), -g[ vdS ].coordPtr->y() );
				path.lineTo( g[ vdT ].coordPtr->x(), -g[ vdT ].coordPtr->y() );
				
				GraphicsEdgeItem *itemptr = g[ ed ].itemPtr;
				//itemptr->setPen( QPen( QColor( 100, 100, 100, 255 ), 2 ) );
				//itemptr->setBrush( QBrush( QColor( 100, 100, 100, 255 ), Qt::SolidPattern ) );
				itemptr->setPath( path );
				//itemptr->id() = g[ed].id;
				//itemptr->textOn() = true;
				
				//_scene->addItem( itemptr );
			}
		
		// draw vertices
		BGL_FORALL_VERTICES( vd, g, BoundaryGraph ) {
				
				GraphicsVertexItem *itemptr = g[ vd ].itemPtr;
				//itemptr->fontSize() = Common::getFontSize();
				//itemptr->setPen( QPen( QColor( 100, 100, 100, 255 ), 2 ) );
				//itemptr->setBrush( QBrush( QColor( 100, 100, 100, 255 ), Qt::SolidPattern ) );
				itemptr->setRect( QRectF( g[ vd ].coordPtr->x(), -g[ vd ].coordPtr->y(), 10, 10 ) );
				//itemptr->id() = g[vd].id;
				//itemptr->text() = QString::fromStdString( to_string( g[vd].isFixed ) );
				//itemptr->textOn() = true;
				
				//cerr << vertexCoord[vd];
				//_scene->addItem( itemptr );
			}
	}
}

void GraphicsView::_item_subpathways( void ) {
	
	MetaboliteGraph &g = _pathwayPtr->g();
	vector< MetaboliteGraph > &subg = _pathwayPtr->subG();
	vector< multimap< int, CellComponent > > &cellCVec = _levelCellPtr->cellComponentVec();

#ifdef DEBUG
	if( _is_subPathwayFlag ){
		for( unsigned int i = 0; i < subg.size(); i++ ){
			printGraph( subg[ i ] );
		}
	}
#endif // DEBUG
	
	for( unsigned int k = 0; k < cellCVec.size(); k++ ) {
		
		multimap< int, CellComponent > &componentMap = cellCVec[ k ];
		multimap< int, CellComponent >::iterator itC = componentMap.begin();
		for( ; itC != componentMap.end(); itC++ ) {
			
			ForceGraph &fg = itC->second.componentRegion.forceGraph();
			_item_force_graph( fg );
		}
	}
	
	_update_item_subpathways();
}

void GraphicsView::_update_item_subpathways( void ) {
	
	MetaboliteGraph &g = _pathwayPtr->g();
	vector< MetaboliteGraph > &subg = _pathwayPtr->subG();
	vector< multimap< int, CellComponent > > &cellCVec = _levelCellPtr->cellComponentVec();
	
	for( unsigned int k = 0; k < cellCVec.size(); k++ ) {
		
		multimap< int, CellComponent > &componentMap = cellCVec[ k ];
		multimap< int, CellComponent >::iterator itC = componentMap.begin();
		for( ; itC != componentMap.end(); itC++ ) {
			
			ForceGraph &fg = itC->second.componentRegion.forceGraph();
			
			// draw edges
			BGL_FORALL_EDGES( ed, fg, ForceGraph ) {
					
					ForceGraph::vertex_descriptor vdS = source( ed, fg );
					ForceGraph::vertex_descriptor vdT = target( ed, fg );
					
					QPainterPath path;
					path.moveTo( fg[ vdS ].coordPtr->x(), -fg[ vdS ].coordPtr->y() );
					path.lineTo( fg[ vdT ].coordPtr->x(), -fg[ vdT ].coordPtr->y() );
					
					// add path
					GraphicsEdgeItem *itemptr = fg[ ed ].itemPtr;
					
					itemptr->setPen( QPen( QColor( 0, 0, 0, 125 ), 2 ) );
					//itemptr->setPen( QPen( QColor( 0, 0, 0, 200 ), 2 ) );
					itemptr->setPath( path );
					//_scene->addItem( itemptr );
				}
			
			// draw vertices
			BGL_FORALL_VERTICES( vd, fg, ForceGraph ) {
					
					MetaboliteGraph::vertex_descriptor vdF = vertex( fg[ vd ].initID, subg[ itC->second.groupID ] );
					
					MetaboliteGraph::vertex_descriptor initVD = vertex( subg[ itC->second.groupID ][ vdF ].initID, g );
#ifdef DEBUG
					cerr << " itC->second.groupID = " << itC->second.groupID << " fg[vd].initID = " << fg[vd].initID
					 << " subg[itC->second.groupID][vdF].initID = " << subg[itC->second.groupID][vdF].initID << endl;
#endif // DEBUG
					GraphicsVertexItem *itemptr = fg[ vd ].itemPtr;
					itemptr->fontSize() = Common::getFontSize();
					
					if( g[ initVD ].type == "reaction" ) {
						itemptr->vtype() = VERTEX_REACTION;
						itemptr->setPen( QPen( QColor( 0, 0, 0, 255 ), 2 ) );
					}
					else if( g[ initVD ].type == "metabolite" ) {
						itemptr->vtype() = VERTEX_METABOLITE;
						itemptr->setPen( QPen( QColor( 100, 100, 100, 255 ), 2 ) );
					}
					
					if( *g[ initVD ].isSelectedPtr == true ) {
						
						vector< double > rgb;
						Color::pickRouteColor( *g[ initVD ].selectedIDPtr, rgb );
						itemptr->setBrush( QBrush( QColor( rgb[ 0 ], rgb[ 1 ], rgb[ 2 ], 255 ), Qt::SolidPattern ) );
					}
					else if( *g[ initVD ].isClonedPtr == true )                 // blue
						itemptr->setBrush( QBrush( QColor( 158, 219, 255, 255 ), Qt::SolidPattern ) );
					else if( subg[ itC->second.groupID ][ vdF ].isAlias == true )   // pink
						itemptr->setBrush( QBrush( QColor( 255, 192, 203, 255 ), Qt::SolidPattern ) );
					else
						itemptr->setBrush( QBrush( QColor( 255, 255, 255, 255 ), Qt::SolidPattern ) );
					
					itemptr->setRect( QRectF( fg[ vd ].coordPtr->x(), -fg[ vd ].coordPtr->y(), 10, 10 ) );
					itemptr->id() = fg[ vd ].id;
					//itemptr->name() = QString::fromStdString( to_string( fg[vd].id ) );
					itemptr->name() = QString::fromStdString( *g[ initVD ].namePtr );
					
					//cerr << vertexCoord[vd];
					//_scene->addItem( itemptr );
				}
		}
	}
}

void GraphicsView::_item_centers( void ) {
	
	vector< RegionBase > &centerVec = _levelCellPtr->centerVec();
	
	for( unsigned int i = 0; i < centerVec.size(); i++ ) {
		
		ForceGraph &fg = centerVec[ i ].forceGraph();
		_item_force_graph( fg );
	}
}

void GraphicsView::_update_item_centers( void ) {
	
	vector< RegionBase > &centerVec = _levelCellPtr->centerVec();
	
	for( unsigned int i = 0; i < centerVec.size(); i++ ) {
		
		ForceGraph &fg = centerVec[ i ].forceGraph();
		_update_item_force_graph( fg );
	}
}

void GraphicsView::_item_centerPolygons( void ) {
	
	vector< RegionBase > &centerVec = _levelCellPtr->centerVec();
	
	for( unsigned int i = 0; i < centerVec.size(); i++ ) {
		
		ForceGraph &fg = centerVec[ i ].forceGraph();
		
		BGL_FORALL_VERTICES( vd, fg, ForceGraph ) {
				
				Polygon2 &p = *fg[ vd ].cellPtr;
				_item_polygon( p );
				GraphicsPolygonItem *itemptr = p.getItemPtr();
				
				vector< double > rgb;
				unsigned int gid = fg[ vd ].groupID;
				_pathwayPtr->pickColor( _colorType, gid, rgb );
				// cerr << rgb[0] << ", " << rgb[1] << ", " << rgb[2] << endl;
				QColor color( rgb[ 0 ], rgb[ 1 ], rgb[ 2 ], 100 );
				itemptr->setPen( QPen( QColor( color.red(), color.green(), color.blue(), 255 ), 2 ) );
				itemptr->setBrush(
						QBrush( QColor( color.red(), color.green(), color.blue(), 100 ), Qt::SolidPattern ) );
			}
	}
}

void GraphicsView::_update_item_centerPolygons( void ) {
	
	vector< RegionBase > &centerVec = _levelCellPtr->centerVec();
	
	for( unsigned int i = 0; i < centerVec.size(); i++ ) {
		
		ForceGraph &fg = centerVec[ i ].forceGraph();
		
		BGL_FORALL_VERTICES( vd, fg, ForceGraph ) {
				
				Polygon2 &p = *fg[ vd ].cellPtr;
				_update_item_polygon( p );
				
				//GraphicsPolygonItem *itemptr = p.getItemPtr();
				//vector< double > rgb;
				//unsigned int gid = fg[ vd ].groupID;
				//_pathwayPtr->pickColor( _colorType, gid, rgb );
				// cerr << rgb[0] << ", " << rgb[1] << ", " << rgb[2] << endl;
				//QColor color( rgb[ 0 ], rgb[ 1 ], rgb[ 2 ], 100 );
				//itemptr->setPen( QPen( QColor( color.red(), color.green(), color.blue(), 255 ), 2 ) );
				//itemptr->setBrush(
				//		QBrush( QColor( color.red(), color.green(), color.blue(), 100 ), Qt::SolidPattern ) );
			}
		
	}
}

void GraphicsView::_item_cells( void ) {
	
	vector< RegionBase > &cellVec = _levelCellPtr->cellVec();
	//vector< ForceGraph > &cellGVec   = _levelCellPtr->forceCellGraphVec();
	
	// create edge object from the spanning tree and add it to the scene
	for( unsigned int i = 0; i < cellVec.size(); i++ ) {
		
		ForceGraph &fg = cellVec[ i ].forceGraph();
		_item_force_graph( fg );
#ifdef DEBUG
		// bounding box
		Polygon2 &fineOutputContour = cellVec[i].force().fineOutputContour();
		Coord2 &boundingBox = componentRegion.fineOutputContour().boundingBox();
		QPainterPath path;
		path.moveTo( componentRegion.fineOutputContour().boxCenter().x() - 0.5*boundingBox.x(), -componentRegion.fineOutputContour().boxCenter().y() + 0.5*boundingBox.y() );
		path.lineTo( componentRegion.fineOutputContour().boxCenter().x() + 0.5*boundingBox.x(), -componentRegion.fineOutputContour().boxCenter().y() + 0.5*boundingBox.y() );
		path.lineTo( componentRegion.fineOutputContour().boxCenter().x() + 0.5*boundingBox.x(), -simpleContour.boxCenter().y() - 0.5*boundingBox.y() );
		path.lineTo( componentRegion.fineOutputContour().boxCenter().x() - 0.5*boundingBox.x(), -componentRegion.fineOutputContour().boxCenter().y() - 0.5*boundingBox.y() );
		path.moveTo( simpleContour.boxCenter().x() - 0.5*boundingBox.x(), -componentRegion.fineOutputContour().boxCenter().y() + 0.5*boundingBox.y() );

		// add path
		GraphicsEdgeItem *itemptr = new GraphicsEdgeItem;

		itemptr->setPen( QPen( QColor( 0, 0, 0, 255 ), 3 ) );
		//itemptr->setPen( QPen( QColor( 0, 0, 0, 200 ), 2 ) );
		itemptr->setPath( path );
		_scene->addItem( itemptr );
#endif // DEBUG
	}
	
	// vironoi seeds
	if( _energyType == ENERGY_STRESS ) {
		
		for( unsigned int i = 0; i < cellVec.size(); i++ ) {
			BoundaryGraph &b = cellVec[ i ].stress().boundaryGraph();
			
			BGL_FORALL_VERTICES( vd, b, BoundaryGraph ) {
					
					GraphicsVertexItem *itemptr = new GraphicsVertexItem;
					b[ vd ].itemPtr = itemptr;
					itemptr->fontSize() = Common::getFontSize();
					//itemptr->setPen( Qt::NoPen );
					itemptr->setPen( QPen( QColor( 0, 0, 0, 255 ), 2 ) );
					itemptr->setBrush( QBrush( QColor( 100, 0, 0, 255 ), Qt::SolidPattern ) );
					itemptr->setRect( QRectF( b[ vd ].centroidPtr->x(), -b[ vd ].centroidPtr->y(), 10, 10 ) );
					itemptr->id() = b[ vd ].id;
					
					//cerr << vertexCoord[vd];
					_scene->addItem( itemptr );
				}
		}
	}

#ifdef DEBUG
	for( unsigned int i = 0; i < cellVec.size(); i++ ) {
		Coord2 &center = _levelCellPtr->center[i];
		double &r = _levelCellPtr->radius[i];

		{
			GraphicsVertexItem *itemptr = new GraphicsVertexItem;
			itemptr->fontSize() = Common::getFontSize();
			//itemptr->setPen( Qt::NoPen );
			itemptr->setPen( QPen( QColor( 0, 0, 0, 255 ), 2 ) );
			itemptr->setBrush( QBrush( QColor( 100, 0, 0, 0 ), Qt::SolidPattern ) );
			itemptr->setRect( QRectF( center.x(), -center.y(), r, r ) );
			itemptr->radius() = r;
			itemptr->id() = i;
			_scene->addItem( itemptr );
		}

		{
			GraphicsVertexItem *itemptr = new GraphicsVertexItem;
			itemptr->fontSize() = Common::getFontSize();
			//itemptr->setPen( Qt::NoPen );
			itemptr->setPen( QPen( QColor( 0, 0, 0, 255 ), 2 ) );
			itemptr->setBrush( QBrush( QColor( 100, 0, 0, 0 ), Qt::SolidPattern ) );
			itemptr->setRect( QRectF( center.x(), -center.y(), r, r ) );
			itemptr->radius() = 2.0*r/3.0;
			itemptr->id() = i;
			_scene->addItem( itemptr );
		}

		{
			QPolygonF polygon;
			for( unsigned int j = 0; j < _levelCellPtr->con[i].elements().size(); j++ ){
				polygon.append( QPointF( _levelCellPtr->con[i].elements()[j].x(), -_levelCellPtr->con[i].elements()[j].y() ) );
				// cerr << "x = " << p[i][j].x() << " y = " << p[i][j].y() << endl;
			}

			GraphicsPolygonItem *itemptr = new GraphicsPolygonItem;
			vector< double > rgb;

			QColor color( 0, 0, 0, 100 );
			itemptr->setPen( QPen( QColor( color.red(), color.green(), color.blue(), 255 ), 2 ) );
			itemptr->setBrush( QBrush( QColor( color.red(), color.green(), color.blue(), 0 ), Qt::SolidPattern ) );
			itemptr->setPolygon( polygon );

			//cerr << vertexCoord[vd];
			_scene->addItem( itemptr );
		}
	}
#endif // DEBUG
}

void GraphicsView::_update_item_cells( void ) {
	
	vector< RegionBase > &cellVec = _levelCellPtr->cellVec();
	//vector< ForceGraph > &cellGVec   = _levelCellPtr->forceCellGraphVec();
	
	// create edge object from the spanning tree and add it to the scene
	for( unsigned int i = 0; i < cellVec.size(); i++ ) {
		
		ForceGraph &fg = cellVec[ i ].forceGraph();
		_update_item_force_graph( fg );

#ifdef DEBUG
		// bounding box
		Polygon2 &simpleContour = cellVec[i].force().simpleContour();
		Coord2 &boundingBox = componentRegion.fineOutputContour().boundingBox();
		QPainterPath path;
		path.moveTo( componentRegion.fineOutputContour().boxCenter().x() - 0.5*boundingBox.x(), -componentRegion.fineOutputContour().boxCenter().y() + 0.5*boundingBox.y() );
		path.lineTo( componentRegion.fineOutputContour().boxCenter().x() + 0.5*boundingBox.x(), -componentRegion.fineOutputContour().boxCenter().y() + 0.5*boundingBox.y() );
		path.lineTo( componentRegion.fineOutputContour().boxCenter().x() + 0.5*boundingBox.x(), -componentRegion.fineOutputContour().boxCenter().y() - 0.5*boundingBox.y() );
		path.lineTo( componentRegion.fineOutputContour().boxCenter().x() - 0.5*boundingBox.x(), -componentRegion.fineOutputContour().boxCenter().y() - 0.5*boundingBox.y() );
		path.moveTo( componentRegion.fineOutputContour().boxCenter().x() - 0.5*boundingBox.x(), -componentRegion.fineOutputContour().boxCenter().y() + 0.5*boundingBox.y() );

		// add path
		GraphicsEdgeItem *itemptr = new GraphicsEdgeItem;

		itemptr->setPen( QPen( QColor( 0, 0, 0, 255 ), 3 ) );
		//itemptr->setPen( QPen( QColor( 0, 0, 0, 200 ), 2 ) );
		itemptr->setPath( path );
		_scene->addItem( itemptr );
#endif // DEBUG
	}
	
	// vironoi seeds
	if( _energyType == ENERGY_STRESS ) {
		
		for( unsigned int i = 0; i < cellVec.size(); i++ ) {
			BoundaryGraph &b = cellVec[ i ].stress().boundaryGraph();
			
			BGL_FORALL_VERTICES( vd, b, BoundaryGraph ) {
					
					GraphicsVertexItem *itemptr = b[ vd ].itemPtr;
					itemptr->fontSize() = Common::getFontSize();
					//itemptr->setPen( Qt::NoPen );
					itemptr->setPen( QPen( QColor( 0, 0, 0, 255 ), 2 ) );
					itemptr->setBrush( QBrush( QColor( 100, 0, 0, 255 ), Qt::SolidPattern ) );
					itemptr->setRect( QRectF( b[ vd ].centroidPtr->x(), -b[ vd ].centroidPtr->y(), 10, 10 ) );
					itemptr->id() = b[ vd ].id;
					
					//cerr << vertexCoord[vd];
					_scene->addItem( itemptr );
				}
		}
	}

#ifdef DEBUG
	for( unsigned int i = 0; i < cellVec.size(); i++ ) {
		Coord2 &center = _levelCellPtr->center[i];
		double &r = _levelCellPtr->radius[i];

		{
			GraphicsVertexItem *itemptr = new GraphicsVertexItem;
			itemptr->fontSize() = Common::getFontSize();
			//itemptr->setPen( Qt::NoPen );
			itemptr->setPen( QPen( QColor( 0, 0, 0, 255 ), 2 ) );
			itemptr->setBrush( QBrush( QColor( 100, 0, 0, 0 ), Qt::SolidPattern ) );
			itemptr->setRect( QRectF( center.x(), -center.y(), r, r ) );
			itemptr->radius() = r;
			itemptr->id() = i;
			_scene->addItem( itemptr );
		}

		{
			GraphicsVertexItem *itemptr = new GraphicsVertexItem;
			itemptr->fontSize() = Common::getFontSize();
			//itemptr->setPen( Qt::NoPen );
			itemptr->setPen( QPen( QColor( 0, 0, 0, 255 ), 2 ) );
			itemptr->setBrush( QBrush( QColor( 100, 0, 0, 0 ), Qt::SolidPattern ) );
			itemptr->setRect( QRectF( center.x(), -center.y(), r, r ) );
			itemptr->radius() = 2.0*r/3.0;
			itemptr->id() = i;
			_scene->addItem( itemptr );
		}

		{
			QPolygonF polygon;
			for( unsigned int j = 0; j < _levelCellPtr->con[i].elements().size(); j++ ){
				polygon.append( QPointF( _levelCellPtr->con[i].elements()[j].x(), -_levelCellPtr->con[i].elements()[j].y() ) );
				// cerr << "x = " << p[i][j].x() << " y = " << p[i][j].y() << endl;
			}

			GraphicsPolygonItem *itemptr = new GraphicsPolygonItem;
			vector< double > rgb;

			QColor color( 0, 0, 0, 100 );
			itemptr->setPen( QPen( QColor( color.red(), color.green(), color.blue(), 255 ), 2 ) );
			itemptr->setBrush( QBrush( QColor( color.red(), color.green(), color.blue(), 0 ), Qt::SolidPattern ) );
			itemptr->setPolygon( polygon );

			//cerr << vertexCoord[vd];
			_scene->addItem( itemptr );
		}
	}
#endif // DEBUG
}

void GraphicsView::_item_interCellComponents( void ) {
	
	vector< RegionBase > &centerVec = _levelCellPtr->centerVec();
	//vector< ForceGraph >                     &cellGVec    = _levelCellPtr->forceCellGraphVec();
	// vector< multimap< int, CellComponent > > &cellCVec    = _levelCellPtr->cellComponentVec();
	
	//multimap< Grid2, pair< CellComponent*, CellComponent* > > & interCCMap = _levelCellPtr->interCellComponentMap();
	multimap< Grid2, pair< CellComponent *, CellComponent * > > &interCCMap = _levelCellPtr->reducedInterCellComponentMap();
	multimap< Grid2, pair< CellComponent *, CellComponent * > >::iterator itC;
	
	// cerr << "size = " << interCCMap.size() << endl;
	for( itC = interCCMap.begin(); itC != interCCMap.end(); itC++ ) {
		
		unsigned int idS = itC->first.p();      // subsystem ID
		unsigned int idT = itC->first.q();      // subsystem ID
		CellComponent &ccS = *itC->second.first;
		CellComponent &ccT = *itC->second.second;
		
		if( ccS.cellgVec.size() == 0 && ccT.cellgVec.size() == 0 ) break;
		
		ForceGraph::vertex_descriptor vdS = vertex( ccS.id, centerVec[ idS ].forceGraph() );
		ForceGraph::vertex_descriptor vdT = vertex( ccT.id, centerVec[ idT ].forceGraph() );

#ifdef DEBUG
		cerr << "idS = " << idS << " idT = " << idT << endl;
		cerr << "ccS = " << ccS.id << " ccT = " << ccT.id << endl;
		cerr << "ccS.size() = " << ccS.cellgVec.size() << " ccT.size() = " << ccT.cellgVec.size() << endl;
		cerr << "centerVec[idS].forceGraph().size() = " << num_vertices( centerVec[idS].forceGraph() )
			 << " centerVec[idT].forceGraph() = " << num_vertices( centerVec[idT].forceGraph() ) << endl;
#endif // DEBUG
		
		QPainterPath path;
		path.moveTo( centerVec[ idS ].forceGraph()[ vdS ].coordPtr->x(),
		             -centerVec[ idS ].forceGraph()[ vdS ].coordPtr->y() );
		path.lineTo( centerVec[ idT ].forceGraph()[ vdT ].coordPtr->x(),
		             -centerVec[ idT ].forceGraph()[ vdT ].coordPtr->y() );
		
		GraphicsEdgeItem *itemptr = new GraphicsEdgeItem;
		itemptr->etype() = EDGE_BETWEEN_DOMAINS;
		itemptr->setPen( QPen( QColor( 255, 0, 0, 100 ), 3 ) );
		itemptr->setBrush( QBrush( QColor( 255, 255, 255, 255 ), Qt::SolidPattern ) );
		itemptr->setPath( path );
		
		_scene->addItem( itemptr );
		
	}
}

void GraphicsView::_item_cellPolygons( void ) {
	
	vector< RegionBase > &cellVec = _levelCellPtr->cellVec();
	
	for( unsigned int k = 0; k < cellVec.size(); k++ ) {
		
		ForceGraph &fg = cellVec[ k ].forceGraph();
		
		BGL_FORALL_VERTICES( vd, fg, ForceGraph ) {
				
				Polygon2 &p = *fg[ vd ].cellPtr;
				_item_polygon( p );
				GraphicsPolygonItem *itemptr = p.getItemPtr();
				
				vector< double > rgb;
				unsigned int gid = cellVec[ k ].forceGraph()[ vd ].groupID;
				_pathwayPtr->pickColor( _colorType, gid, rgb );
				QColor color( rgb[ 0 ], rgb[ 1 ], rgb[ 2 ], 100 );
				itemptr->setPen( QPen( QColor( color.red(), color.green(), color.blue(), 255 ), 2 ) );
				itemptr->setBrush(
						QBrush( QColor( color.red(), color.green(), color.blue(), 100 ), Qt::SolidPattern ) );
			}
	}
}

void GraphicsView::_update_item_cellPolygons( void ) {
	
	vector< RegionBase > &cellVec = _levelCellPtr->cellVec();
	
	for( unsigned int k = 0; k < cellVec.size(); k++ ) {
		
		ForceGraph &fg = cellVec[ k ].forceGraph();
		
		BGL_FORALL_VERTICES( vd, fg, ForceGraph ) {
				
				Polygon2 &p = *fg[ vd ].cellPtr;
				_update_item_polygon( p );
				
				//GraphicsPolygonItem *itemptr = p.getItemPtr();
				
				//vector< double > rgb;
				//unsigned int gid = cellVec[ k ].forceGraph()[ vd ].groupID;
				//_pathwayPtr->pickColor( _colorType, gid, rgb );
				//QColor color( rgb[ 0 ], rgb[ 1 ], rgb[ 2 ], 100 );
				//itemptr->setPen( QPen( QColor( color.red(), color.green(), color.blue(), 255 ), 2 ) );
				//itemptr->setBrush(
				//		QBrush( QColor( color.red(), color.green(), color.blue(), 100 ), Qt::SolidPattern ) );
				//itemptr->setPolygon( polygon );
				
				//cerr << vertexCoord[vd];
				//_scene->addItem( itemptr );
			}
	}
}

void GraphicsView::_item_cellPolygonComplex( bool fineFlag ) {
	
	vector< RegionBase > &cellVec = _levelCellPtr->cellVec();
	vector< multimap< int, CellComponent > > &cellCVec = _levelCellPtr->cellComponentVec();
	
	for( unsigned int k = 0; k < cellCVec.size(); k++ ) {
		
		multimap< int, CellComponent > &componentMap = cellCVec[ k ];
		multimap< int, CellComponent >::iterator itC = componentMap.begin();
		
		for( ; itC != componentMap.end(); itC++ ) {
			
			Polygon2 *c;
			//if( fineFlag == true ) c = &itC->second.simpleContour.simpleContour();
			if( fineFlag == true ) c = &itC->second.componentRegion.fineOutputContour().fineContour();
			else c = &itC->second.componentRegion.fineOutputContour().contour();
			
			ForceGraph::vertex_descriptor vd = vertex( itC->second.id, cellVec[ k ].forceGraph() );
			
			if( vd != NULL ) {
#ifdef DEBUG
				if( k == 1 ){
					cerr << "###############" << endl;
					cerr << "itC->second.id = " << itC->second.id
					<< " isSimple = " << c->isSimple() << endl;
					// cerr << *c << endl;
					cerr << "###############" << endl;
				}
#endif // DEBUG
				_item_polygon( *c );
				GraphicsPolygonItem *itemptr = c->getItemPtr();
				
				vector< double > rgb;
				
				unsigned int gid = cellVec[ k ].forceGraph()[ vd ].groupID;
				_pathwayPtr->pickColor( _colorType, gid, rgb );
				QColor color( rgb[ 0 ], rgb[ 1 ], rgb[ 2 ], 100 );
				itemptr->setPen( QPen( QColor( color.red(), color.green(), color.blue(), 255 ), 6 ) );
				//itemptr->setPen( QPen( QColor( color.red(), color.green(), color.blue(), 255 ), 8 ) );
				itemptr->setBrush(
						QBrush( QColor( color.red(), color.green(), color.blue(), 100 ), Qt::SolidPattern ) );
				//itemptr->setPolygon( polygon );
				//itemptr->id() = cellVec[k].forceGraph()[vd].id;
				//itemptr->textOn() = true;
				
				//cerr << vertexCoord[vd];
				//_scene->addItem( itemptr );
			}

#ifdef DEBUG
			for( unsigned int j = 0; j < c.elements().size(); j++ ){

				GraphicsVertexItem *itemptr = new GraphicsVertexItem;
				itemptr->fontSize() = Common::getFontSize();
				itemptr->setPen( QPen( QColor( 0, 0, 0, 255 ), 2 ) );
				itemptr->setBrush( QBrush( QColor( color.red(), color.green(), color.blue(), 255 ), Qt::SolidPattern ) );
				itemptr->setRect( QRectF( c.elements()[j].x(), -c.elements()[j].y(), 10, 10 ) );
				itemptr->id() = 0;

				//cerr << vertexCoord[vd];
				_scene->addItem( itemptr );
			}
#endif // DEBUG
		}
	}
}

void GraphicsView::_update_item_cellPolygonComplex( bool fineFlag ) {
	
	vector< RegionBase > &cellVec = _levelCellPtr->cellVec();
	vector< multimap< int, CellComponent > > &cellCVec = _levelCellPtr->cellComponentVec();
	
	for( unsigned int k = 0; k < cellCVec.size(); k++ ) {
		
		multimap< int, CellComponent > &componentMap = cellCVec[ k ];
		multimap< int, CellComponent >::iterator itC = componentMap.begin();
		for( ; itC != componentMap.end(); itC++ ) {
			
			Polygon2 *c;
			//if( fineFlag == true ) c = &itC->second.simpleContour.simpleContour();
			if( fineFlag == true ) c = &itC->second.componentRegion.fineOutputContour().fineContour();
			else c = &itC->second.componentRegion.fineOutputContour().contour();
			
			ForceGraph::vertex_descriptor vd = vertex( itC->second.id, cellVec[ k ].forceGraph() );
			
			if( vd != NULL ) {
				
				_update_item_polygon( *c );
				//GraphicsPolygonItem *itemptr = c->getItemPtr();
				//vector< double > rgb;
				//unsigned int gid = cellVec[k].forceGraph()[vd].groupID;
				//_pathwayPtr->pickColor( _colorType, gid, rgb );
				//QColor color( rgb[0], rgb[1], rgb[2], 100 );
				//itemptr->setPen( QPen( QColor( color.red(), color.green(), color.blue(), 255 ), 6 ) );
				//itemptr->setPen( QPen( QColor( color.red(), color.green(), color.blue(), 255 ), 8 ) );
				//itemptr->setBrush( QBrush( QColor( color.red(), color.green(), color.blue(), 100 ), Qt::SolidPattern ) );
				//itemptr->setPolygon( polygon );
				//itemptr->id() = cellVec[k].forceGraph()[vd].id;
				//itemptr->textOn() = true;
				
				//cerr << vertexCoord[vd];
				//_scene->addItem( itemptr );
			}

#ifdef DEBUG
			for( unsigned int j = 0; j < c.elements().size(); j++ ){

				GraphicsVertexItem *itemptr = new GraphicsVertexItem;
				itemptr->fontSize() = Common::getFontSize();
				itemptr->setPen( QPen( QColor( 0, 0, 0, 255 ), 2 ) );
				itemptr->setBrush( QBrush( QColor( color.red(), color.green(), color.blue(), 255 ), Qt::SolidPattern ) );
				itemptr->setRect( QRectF( c.elements()[j].x(), -c.elements()[j].y(), 10, 10 ) );
				itemptr->id() = 0;

				//cerr << vertexCoord[vd];
				_scene->addItem( itemptr );
			}
#endif // DEBUG
		}
	}
}

void GraphicsView::_item_pathwayPolygons( void ) {
	
	vector< multimap< int, CellComponent > > &cellCVec = _levelCellPtr->cellComponentVec();
	
	for( unsigned int k = 0; k < cellCVec.size(); k++ ) {
		
		
		multimap< int, CellComponent > &componentMap = cellCVec[ k ];
		multimap< int, CellComponent >::iterator itC = componentMap.begin();
		
		
		for( ; itC != componentMap.end(); itC++ ) {
			
			ForceGraph &fg = itC->second.componentRegion.forceGraph();
			
			BGL_FORALL_VERTICES( vd, fg, ForceGraph ) {
					
					Polygon2 &p = *fg[ vd ].cellPtr;
					_item_polygon( p );
					
					GraphicsPolygonItem *itemptr = p.getItemPtr();
					vector< double > rgb;
					
					_pathwayPtr->pickColor( _colorType, k, rgb );
					QColor color( rgb[ 0 ], rgb[ 1 ], rgb[ 2 ], 100 );
					//itemptr->setPen( QPen( QColor( 220, 220, 220, 100 ), 4 ) );
					if( _is_pathwayPolygonContourFlag == true )
						itemptr->setPen( QPen( QColor( color.red(), color.green(), color.blue(), 0 ), 2 ) );
					else
						itemptr->setPen( QPen( QColor( color.red(), color.green(), color.blue(), 255 ), 2 ) );
					itemptr->setBrush(
							QBrush( QColor( color.red(), color.green(), color.blue(), 100 ), Qt::SolidPattern ) );
				}
		}
	}
}

void GraphicsView::_update_item_pathwayPolygons( void ) {
	
	vector< multimap< int, CellComponent > > &cellCVec = _levelCellPtr->cellComponentVec();
	
	for( unsigned int k = 0; k < cellCVec.size(); k++ ) {
		
		
		multimap< int, CellComponent > &componentMap = cellCVec[ k ];
		multimap< int, CellComponent >::iterator itC = componentMap.begin();
		
		for( ; itC != componentMap.end(); itC++ ) {
			
			ForceGraph &fg = itC->second.componentRegion.forceGraph();
			
			BGL_FORALL_VERTICES( vd, fg, ForceGraph ) {
					
					Polygon2 &p = *fg[ vd ].cellPtr;
					_update_item_polygon( p );
					
					//GraphicsPolygonItem *itemptr = p.getItemPtr();
					//vector< double > rgb;
					//_pathwayPtr->pickColor( _colorType, k, rgb );
					//QColor color( rgb[ 0 ], rgb[ 1 ], rgb[ 2 ], 100 );
					//itemptr->setPen( QPen( QColor( 220, 220, 220, 100 ), 4 ) );
					//if( _is_pathwayPolygonContourFlag == true )
					//	itemptr->setPen( QPen( QColor( color.red(), color.green(), color.blue(), 0 ), 2 ) );
					//else
					//	itemptr->setPen( QPen( QColor( color.red(), color.green(), color.blue(), 255 ), 2 ) );
					//itemptr->setBrush(
					//		QBrush( QColor( color.red(), color.green(), color.blue(), 100 ), Qt::SolidPattern ) );
					//itemptr->setPolygon( polygon );
					
					//cerr << vertexCoord[vd];
					//_scene->addItem( itemptr );
				}
		}
	}
}

void GraphicsView::_item_road( void ) {
	
	ForceGraph &s = _levelBorderPtr->skeletonForceGraph();
	
	map< unsigned int, Polygon2 > p = _levelBorderPtr->regionBase().polygonComplex();
	map< unsigned int, Polygon2 >::iterator itP = p.begin();
	
	for( ; itP != p.end(); itP++ ) {
		
		Contour2 c;
		c.contour() = itP->second;
		c.computeChaikinCurve(  5, 50 );
		Polygon2 &p = c.fineContour();
		QPolygonF polygon;
		for( unsigned int j = 0; j < p.elements().size(); j++ ) {
			polygon.append( QPointF( p.elements()[ j ].x(), -p.elements()[ j ].y() ) );
		}
		
		GraphicsPolygonItem *itemptr = new GraphicsPolygonItem;

		itemptr->setPolygon( polygon );
		itemptr->setPen( QPen( QColor( 0, 0, 0, 255 ), 8 ) );
		itemptr->setBrush( QBrush( QColor( 0, 0, 0, 0 ), Qt::SolidPattern ) );

		_scene->addItem( itemptr );
	}
	
#ifdef DEBUG
	
	vector< MetaboliteGraph > &subg = _pathwayPtr->subG();
	UndirectedBaseGraph &road = ( *_roadPtr )[ 0 ].road();
	vector< vector< Highway > > &highwayMat = ( *_roadPtr )[ 0 ].highwayMat();
	// vector< vector< Coord2 > > & roadChaikinCurveVec = _roadPtr->roadChaikinCurve();
	vector< Contour2 > &subsysContour = ( *_roadPtr )[ 0 ].subsysContour();

	// draw background
	for( unsigned int i = 0; i < subsysContour.size(); i++ ) {
		
		vector< Coord2 > &p = subsysContour[i].contour().elements();
		//vector< Coord2 > &p = subsysContour[ i ].fineContour().elements();
		
		QPolygonF polygon;
		for( unsigned int k = 0; k < p.size(); k++ ) {
			
			// cerr << "c = " << p[k];
			polygon.append( QPointF( p[ k ].x(), -p[ k ].y() ) );
		}
		
		// add polygon
		GraphicsPolygonItem *itemptr = new GraphicsPolygonItem;
		vector< double > rgb;
		
		_pathwayPtr->pickColor( _colorType, i, rgb );
		QColor color( rgb[ 0 ], rgb[ 1 ], rgb[ 2 ], 100 );
		
		itemptr->setPen( QPen( QColor( color.red(), color.green(), color.blue(), 255 ), 2 ) );
		itemptr->setBrush( QBrush( QColor( color.red(), color.green(), color.blue(), 50 ), Qt::SolidPattern ) );
		
		itemptr->setPolygon( polygon );
		_scene->addItem( itemptr );
	}
	
	// draw simpleContour
	for( unsigned int i = 0; i < subsysContour.size(); i++ ) {
		
		vector< Coord2 > &p = subsysContour[i].contour().elements();
		//vector< Coord2 > &p = subsysContour[ i ].fineContour().elements();
		
		QPainterPath path;
		for( unsigned int k = 0; k < p.size(); k++ ) {
			
			if( k == 0 ) {
				path.moveTo( p[ k ].x(), -p[ k ].y() );
			}
			else {
				path.lineTo( p[ k ].x(), -p[ k ].y() );
			}
		}
		path.lineTo( p[ 0 ].x(), -p[ 0 ].y() );
		
		// add path
		GraphicsEdgeItem *itemptr = new GraphicsEdgeItem;
		
		//itemptr->setPen( QPen( QColor( 0, 0, 255, 255 ), 3 ) );
		itemptr->setPen( QPen( QColor( 0, 0, 0, 255 ), 8 ) );
		itemptr->setPath( path );
		_scene->addItem( itemptr );
	}
#endif DEBUG
}

void GraphicsView::_item_lane( void ) {
	vector< Road > &lane = *_lanePtr;

#ifdef SKIP
	for( unsigned int i = 0; i < 1; i++ ){
	//for( unsigned int i = 0; i < lane.size(); i++ ){

		UndirectedBaseGraph &road = lane[i].road();

		// draw edges
		BGL_FORALL_EDGES( ed, road, UndirectedBaseGraph ) {

			UndirectedBaseGraph::vertex_descriptor vdS = source( ed, road );
			UndirectedBaseGraph::vertex_descriptor vdT = target( ed, road );

			QPainterPath path;
			path.moveTo( road[ vdS ].coordPtr->x(), -road[ vdS ].coordPtr->y() );
			path.lineTo( road[ vdT ].coordPtr->x(), -road[ vdT ].coordPtr->y() );

			// add path
			GraphicsEdgeItem *itemptr = new GraphicsEdgeItem;

			//itemptr->setPen( QPen( QColor( 0, 0, 255, 255 ), 3 ) );
			itemptr->setPen( QPen( QColor( 0, 100, 0, 200 ), 5 ) );
			itemptr->setPath( path );
			itemptr->id() = road[ ed ].visitedTimes;
			//itemptr->textOn() = true;

			_scene->addItem( itemptr );
		}

		// draw vertices
		BGL_FORALL_VERTICES( vd, road, UndirectedBaseGraph ) {

			GraphicsVertexItem *itemptr = new GraphicsVertexItem;
			itemptr->fontSize() = Common::getFontSize();
			itemptr->setPen( QPen( QColor( 0, 100, 0, 255 ), 2 ) );
			itemptr->setBrush( QBrush( QColor( 0, 100, 0, 255 ), Qt::SolidPattern ) );
			itemptr->setRect( QRectF( road[vd].coordPtr->x(), -road[vd].coordPtr->y(), 10, 10 ) );
			itemptr->id() = road[ vd ].id;
			itemptr->text() = QString::fromStdString( to_string( road[ vd ].id ) );
			itemptr->textOn() = true;

			//cerr << vertexCoord[vd];
			_scene->addItem( itemptr );
		}
	}
#endif // SKIP

#ifdef SKIP
	// draw routers
	//for( unsigned int i = 0; i < 1; i++ ){
	for( unsigned int i = 0; i < lane.size(); i++ ){

		UndirectedBaseGraph &road = lane[i].road();
		vector < Terminal > &terminalVec = lane[i].terminalVec();

		for( unsigned int j = 0; j < terminalVec.size(); j++ ){

			if( i != j ){
				Coord2 &coord = *road[ terminalVec[j].routerVD ].coordPtr;

				GraphicsVertexItem *itemptr = new GraphicsVertexItem;
				itemptr->fontSize() = Common::getFontSize();
				itemptr->setPen( QPen( QColor( 0, 0, 0, 255 ), 10 ) );
				itemptr->setBrush( QBrush( QColor( 0, 0, 0, 255 ), Qt::SolidPattern ) );
				itemptr->setRect( QRectF( coord.x(), -coord.y(), 10, 10 ) );
				itemptr->id() = road[ terminalVec[j].routerVD ].id;

				//cerr << vertexCoord[vd];
				_scene->addItem( itemptr );
			}
		}
	}
#endif // SKIP

#ifdef SKIP
	// draw steiner tree
	//for( unsigned int i = 0; i < 1; i++ ){
	for( unsigned int i = 0; i < lane.size(); i++ ){

		UndirectedBaseGraph &road = lane[i].road();
		vector< pair< UndirectedBaseGraph::vertex_descriptor,
				UndirectedBaseGraph::vertex_descriptor > > &treeEdgeVec = lane[i].treeEdgeVec();

		for( unsigned int k = 0; k < treeEdgeVec.size(); k++ ){

			QPainterPath path;
			Coord2 &coordS = *road[ treeEdgeVec[k].first ].coordPtr;
			Coord2 &coordT = *road[ treeEdgeVec[k].second ].coordPtr;
			path.moveTo( coordS.x(), -coordS.y() );
			path.lineTo( coordT.x(), -coordT.y() );

			// add path
			GraphicsEdgeItem *itemptr = new GraphicsEdgeItem;

			//itemptr->setPen( QPen( QColor( 0, 0, 255, 255 ), 3 ) );
			itemptr->setPen( QPen( QColor( 255, 0, 0, 255 ), 4 ) );
			itemptr->setPath( path );
			_scene->addItem( itemptr );

		}
	}
#endif // SKIP

#ifdef SKIP
	// draw lane Chaikin curve
	for( unsigned int i = 0; i < lane.size(); i++ ){

		vector< Coord2 > &p = lane[i].laneChaikinCurve();

		QPainterPath path;
		for( unsigned int k = 0; k < p.size(); k++ ){

			if( k == 0 ){
				path.moveTo( p[k].x(), -p[k].y() );
			}
			else{
				path.lineTo( p[k].x(), -p[k].y() );
			}
		}

		// add path
		GraphicsEdgeItem *itemptr = new GraphicsEdgeItem;

		//itemptr->setPen( QPen( QColor( 0, 0, 255, 255 ), 3 ) );
		itemptr->setPen( QPen( QColor( 255, 0, 0, 255 ), 4 ) );
		itemptr->setPath( path );
		_scene->addItem( itemptr );
	}
#endif // SKIP
	
	// draw steinter tree
	for( unsigned int i = 0; i < lane.size(); i++ ) {
		
		vector< Line2 > &paths = lane[ i ].curvytree().paths();
		UndirectedBaseGraph &tree = lane[ i ].curvytree().tree();
		
		// draw the paths
		for( unsigned int j = 0; j < paths.size(); j++ ) {
			
			Line2 &line = paths[ j ];
			
			QPainterPath path;
			//vector< Coord2 > &samples = line.samples();
			vector< Coord2 > &samples = line.fineSamples();
			
			for( unsigned int k = 0; k < samples.size(); k++ ) {
				
				Coord2 &coord = samples[ k ];
				if( k == 0 ) path.moveTo( coord.x(), -coord.y() );
				else path.lineTo( coord.x(), -coord.y() );
			}
			
			// add path
			GraphicsEdgeItem *itemptr = new GraphicsEdgeItem;
			
			//itemptr->setPen( QPen( QColor( 0, 0, 255, 255 ), 3 ) );
			vector< double > rgb;
			Color::pickRouteColor( i, rgb );
			
			itemptr->setPen( QPen( QColor( rgb[ 0 ], rgb[ 1 ], rgb[ 2 ], 255 ), 4 ) );
			itemptr->setPath( path );
			_scene->addItem( itemptr );
		}

#ifdef SKIP
		// draw the tree
		BGL_FORALL_EDGES( ed, tree, UndirectedBaseGraph ) {

			UndirectedBaseGraph::vertex_descriptor vdS = source( ed, tree );
			UndirectedBaseGraph::vertex_descriptor vdT = target( ed, tree );

			QPainterPath path;
			Coord2 &coordS = *tree[ vdS ].coordPtr;
			Coord2 &coordT = *tree[ vdT ].coordPtr;
			path.moveTo( coordS.x(), -coordS.y() );
			path.lineTo( coordT.x(), -coordT.y() );

			// add path
			GraphicsEdgeItem *itemptr = new GraphicsEdgeItem;

			//itemptr->setPen( QPen( QColor( 0, 0, 255, 255 ), 3 ) );
			itemptr->setPen( QPen( QColor( 255, 0, 0, 255 ), 4 ) );
			itemptr->setPath( path );
			_scene->addItem( itemptr );
		}
#endif // SKIP
	}
}


//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
void GraphicsView::initSceneItems( void ) {
	
	// initialization
	_scene->clear();
	
	if( *_levelTypePtr == LEVEL_BORDER ) {
		if( _is_polygonFlag == true ) _item_composite_polygons();
		if( _is_polygonComplexFlag == true ) _item_polygonComplex();
		if( _is_compositeFlag == true ) {
			//_item_skeleton();
			_item_composite();
		}
		if( _is_centerFlag == true ) _item_centers();
		_item_boundary();
	}
	else if( *_levelTypePtr == LEVEL_CELLCENTER ) {
		//_item_polygonComplex();
		_item_centerPolygons();
		_item_interCellComponents();
		_item_centers();
		_item_boundary();
	}
	else if( *_levelTypePtr == LEVEL_CELLCOMPONENT ) {
		if( _is_cellPolygonComplexFlag == true ) {
			if( _is_laneFlag == true )
				_item_cellPolygonComplex( true );
			else {
				if( _is_cellPolygonFlag == true )
					_item_cellPolygons();
				else
					_item_cellPolygonComplex( false );
			}
		}
		_item_cells();
		_item_boundary();
	}
	else if( *_levelTypePtr == LEVEL_DETAIL ) {
		if( _is_cellPolygonComplexFlag == true ) {
			if( _is_laneFlag == true )
				_item_cellPolygonComplex( true );
			else
				_item_cellPolygonComplex( false );
		}
		else {
			_item_pathwayPolygons();
			_item_boundary();
		}
		if( _is_roadFlag == true ) _item_road();        // cluster boundary
		if( _is_laneFlag == true ) _item_lane();        // route connecting duplicated nodes
		_item_subpathways();
	}

#ifdef DEBUG
	/*
	vector< QPointF > test;

	test.push_back( QPointF(310.792,	-328.956 ) );
	test.push_back( QPointF(310.792,	-237.622 ) );
	test.push_back( QPointF(242.897,	-169.727 ) );
	test.push_back( QPointF(242.897,	-120.323 ) );
	test.push_back( QPointF(128.926,	-234.295 ) );
	test.push_back( QPointF(128.926,	-328.956 ) );
	test.push_back( QPointF(129.713,	16.3117 ) );
	test.push_back( QPointF(50.6418,	-62.7591 ) );
	test.push_back( QPointF(50.6418,	-156.011 ) );
	test.push_back( QPointF(242.897,	-101.929 ) );
	test.push_back( QPointF(197.856,	-56.8884 ) );
	test.push_back( QPointF(197.856,	-40.2206 ) );
	test.push_back( QPointF(141.324,	16.3117 ) );
	test.push_back( QPointF(-200.527,	62.5449 ) );
	test.push_back( QPointF(-122.3,		-15.6823 ) );
	test.push_back( QPointF(-60.4159,	46.203 ) );
	test.push_back( QPointF(-120.294,	106.078 ) );
	test.push_back( QPointF(-176.043,	106.078 ) );
	test.push_back( QPointF(-200.527,	130.562 ) );
	test.push_back( QPointF(-455.697,	103.132 ) );
	test.push_back( QPointF(-345.96,	103.133 ) );
	test.push_back( QPointF(-292.543,	156.549 ) );
	test.push_back( QPointF(-323.773,	156.549 ) );
	test.push_back( QPointF(-323.773,	244.297 ) );
	test.push_back( QPointF(-455.697,	244.297 ) );
	test.push_back( QPointF(-455.697,	-105.952 ) );
	test.push_back( QPointF(-345.96,	3.78434 ) );
	test.push_back( QPointF(-226.514,	156.549 ) );
	test.push_back( QPointF(-259.288,	3.78438 ) );
	test.push_back( QPointF(-455.697,	-328.956 ) );
	test.push_back( QPointF(47.915,	16.3117 ) );
	test.push_back( QPointF( 18.0214,	46.2052 ) );

	for( unsigned int i = 0; i < test.size(); i++ ){

		GraphicsVertexItem *itemptr = new GraphicsVertexItem;
		itemptr->fontSize() = Common::getFontSize();
		itemptr->setPen( QPen( QColor( 100, 0, 0, 255 ), 2 ) );
		itemptr->setBrush( QBrush( QColor( 100, 0, 0, 255 ), Qt::SolidPattern ) );
		itemptr->setRect( QRectF( test[i].x(), -test[i].y(), 10, 10 ) );
		itemptr->id() = i;
		itemptr->text() = QString::fromStdString( to_string( i) );
		itemptr->textOn() = true;

		_scene->addItem( itemptr );
	}
*/
	QPolygonF polygon;
	
	polygon.append( QPointF( -280.317,	10.93 ) );
	polygon.append( QPointF( -280.327, 20.3251 ) );
	polygon.append( QPointF( -394.495, 134.473 ) );
	polygon.append( QPointF( -394.51, 146.737 ) );
	polygon.append( QPointF( -469.128, 221.356 ) );
	polygon.append( QPointF( -533.022, 157.611 ) );
	polygon.append( QPointF( -635.674, 157.683 ) );
	polygon.append( QPointF( -656.144, 137.299 ) );
	polygon.append( QPointF( -656.167, 5.51959 ) );
	polygon.append( QPointF( -675.784, -14.0743 ) );
	polygon.append( QPointF( -675.782, -158.373 ) );
	polygon.append( QPointF( -661.029, -173.125 ) );
	polygon.append( QPointF( -661.025, -295.864 ) );
	polygon.append( QPointF( -70.1812, -295.861 ) );
	polygon.append( QPointF( -70.1806, -238.812 ) );
	polygon.append( QPointF( -9.54215, -178.173 ) );
	polygon.append( QPointF( 14.7202, -202.431 ) );
	polygon.append( QPointF( 110.5, -202.428 ) );
	polygon.append( QPointF( 110.501, -147.701 ) );
	polygon.append( QPointF( 36.6619, -73.871 ) );
	polygon.append( QPointF( 36.657, -52.1613 ) );
	polygon.append( QPointF( -78.3776, 62.8682 ) );
	polygon.append( QPointF( -130.324, 10.9289 ) );
	
	GraphicsPolygonItem *itemptr = new GraphicsPolygonItem;
	vector< double > rgb;
	
	QColor color( 0, 0, 0, 100 );
	itemptr->setPen( QPen( QColor( color.red(), color.green(), color.blue(), 255 ), 2 ) );
	itemptr->setBrush( QBrush( QColor( color.red(), color.green(), color.blue(), 100 ), Qt::SolidPattern ) );
	itemptr->setPolygon( polygon );
	
	//cerr << vertexCoord[vd];
	_scene->addItem( itemptr );
#endif // DEBUG
}


//
//  GraphicsView::updateSceneItems -- initialize SceneItems
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void GraphicsView::updateSceneItems( void ) {
	
	if( *_levelTypePtr == LEVEL_BORDER ) {
		if( _is_polygonFlag == true ) _update_item_composite_polygons();
		if( _is_polygonComplexFlag == true ) _update_item_polygonComplex();
		if( _is_compositeFlag == true ) {
			//_update_item_skeleton();
			_update_item_composite();
		}
		if( _is_centerFlag == true ) _update_item_centers();
		_update_item_boundary();
	}
	else if( *_levelTypePtr == LEVEL_CELLCENTER ) {
		//_update_item_polygonComplex();
		_update_item_centerPolygons();
		_item_interCellComponents();
		_update_item_centers();
		_update_item_boundary();
	}
	else if( *_levelTypePtr == LEVEL_CELLCOMPONENT ) {
		if( _is_cellPolygonComplexFlag == true ) {
			if( _is_laneFlag == true )
				_update_item_cellPolygonComplex( true );
			else {
				if( _is_cellPolygonFlag == true )
					_update_item_cellPolygons();
				else
					_update_item_cellPolygonComplex( false );
			}
		}
		_update_item_cells();
		_update_item_boundary();
	}
	else if( *_levelTypePtr == LEVEL_DETAIL ) {
		if( _is_cellPolygonComplexFlag == true ) {
			if( _is_laneFlag == true )
				_update_item_cellPolygonComplex( true );
			else
				_update_item_cellPolygonComplex( false );
		}
		else {
			_update_item_pathwayPolygons();
			_update_item_boundary();
		}
		_update_item_subpathways();
	}
}

//------------------------------------------------------------------------------
//	Event handlers
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
int GraphicsView::exportPNG( void ) {
	
	double x = -( Common::getContentWidth() + LEFTRIGHT_MARGIN ) / 2.0;
	double y = -( Common::getContentHeight() + TOPBOTTOM_MARGIN ) / 2.0;
	double w = Common::getContentWidth() + LEFTRIGHT_MARGIN;
	double h = Common::getContentHeight() + TOPBOTTOM_MARGIN;
	
	// Take file path and name that will create
	//QString newPath = QFileDialog::getSaveFileName(this, trUtf8("Save SVG"),
	//                                               path, tr("SVG files (*.svg)"));
	static int id = 0;
	ostringstream ss;
	ss << setw( 8 ) << std::setfill( '0' ) << id;
	string s2( ss.str() );
	QString idStr = QString::fromStdString( s2 );
	// cerr << "s2 = " << s2 << endl;
	QString newPath = QString( QLatin1String( "svg/pathway-" ) ) + idStr + QString( QLatin1String( ".png" ) );
	_scene->setSceneRect( x, y, w, h );  // x, y, w, h
	
	if( newPath.isEmpty() ) return 0;
	
	QImage screenshot( w, h, QImage::Format_RGB32 ); // maximum 32767x32767
	
	//int dpm = 300 / 0.0254; // ~300 DPI
	//screenshot.setDotsPerMeterX( dpm );
	//screenshot.setDotsPerMeterY( dpm );
	
	QPainter painter( &screenshot );
	painter.setRenderHint( QPainter::Antialiasing );
	painter.fillRect( 0, 0, w, h, Qt::white );
	_scene->render( &painter );
	
	id++;
	
	int p = 0;
	if( _overlapFlag == true ) {
		
		int imgW = screenshot.width();
		int imgH = screenshot.height();
		
		for( int i = 0; i < imgW; i++ ) {
			for( int j = 0; j < imgH; j++ ) {
				QColor color = screenshot.pixelColor( i, j );
				if( color.red() < 160 ) {
					screenshot.setPixelColor( i, j, QColor( 0, 0, 0, 255 ) );
					// cerr << "p = " << color.red() << endl;
					p++;
				}
			}
		}
	}
	
	screenshot.save( newPath );
	return p;
	// cerr << "p = " << p << endl;
}

void GraphicsView::exportSVG( void ) {
	
	double x = -( Common::getContentWidth() + LEFTRIGHT_MARGIN ) / 2.0;
	double y = -( Common::getContentHeight() + TOPBOTTOM_MARGIN ) / 2.0;
	double w = Common::getContentWidth() + LEFTRIGHT_MARGIN;
	double h = Common::getContentHeight() + TOPBOTTOM_MARGIN;
	
	// Take file path and name that will create
	//QString newPath = QFileDialog::getSaveFileName(this, trUtf8("Save SVG"),
	//                                               path, tr("SVG files (*.svg)"));
	static int id = 0;
	ostringstream ss;
	ss << setw( 8 ) << std::setfill( '0' ) << id;
	string s2( ss.str() );
	QString idStr = QString::fromStdString( s2 );
	// cerr << "s2 = " << s2 << endl;
	QString newPath = QString( QLatin1String( "svg/pathway-" ) ) + idStr + QString( QLatin1String( ".svg" ) );
	_scene->setSceneRect( x, y, w, h );  // x, y, w, h
	
	if( newPath.isEmpty() ) return;
	
	QSvgGenerator generator;            // Create a file generator object
	generator.setFileName( newPath );    // We set the path to the file where to save vector graphics
	generator.setSize( QSize( w, h ) );  // Set the dimensions of the working area of the document in millimeters
	generator.setViewBox( QRect( 0, 0, w, h ) ); // Set the work area in the coordinates
	generator.setTitle( trUtf8( "SVG Example" ) );                                // The title document
	generator.setDescription( trUtf8( "File created by SVG Example" ) );
	
	QPainter painter;
	painter.begin( &generator );
	_scene->render( &painter );
	painter.end();
	
	id++;
}

void GraphicsView::computeNodeOverlaps( ofstream &ofs ) {
	
	_scene->clear();
	MetaboliteGraph &g = _pathwayPtr->g();
	vector< MetaboliteGraph > &subg = _pathwayPtr->subG();
	vector< multimap< int, CellComponent > > &cellCVec = _levelCellPtr->cellComponentVec();
	
	for( unsigned int k = 0; k < cellCVec.size(); k++ ) {
		
		multimap< int, CellComponent > &componentMap = cellCVec[ k ];
		multimap< int, CellComponent >::iterator itC = componentMap.begin();
		for( ; itC != componentMap.end(); itC++ ) {
			
			ForceGraph &fg = itC->second.componentRegion.forceGraph();
			_item_force_graph( fg );
			
			BGL_FORALL_EDGES( ed, fg, ForceGraph ) {
					GraphicsEdgeItem *itemptr = fg[ ed ].itemPtr;
					itemptr->hide();
				}
			
			BGL_FORALL_VERTICES( vd, fg, ForceGraph ) {
					
					MetaboliteGraph::vertex_descriptor vdF = vertex( fg[ vd ].initID, subg[ itC->second.groupID ] );
					MetaboliteGraph::vertex_descriptor initVD = vertex( subg[ itC->second.groupID ][ vdF ].initID, g );
					
					GraphicsVertexItem *itemptr = fg[ vd ].itemPtr;
					
					itemptr->name() = QString::fromStdString( *g[ initVD ].namePtr );
					itemptr->setPen( QPen( QColor( 100, 100, 100, 100 ), 0 ) );
					itemptr->setBrush( QBrush( QColor( 100, 100, 100, 100 ), Qt::SolidPattern ) );
					
					if( g[ initVD ].type == "reaction" ) {
						itemptr->vtype() = VERTEX_REACTION;
						//itemptr->setPen( QPen( QColor( 0, 0, 0, 255 ), 2 ) );
					}
					else if( g[ initVD ].type == "metabolite" ) {
						itemptr->vtype() = VERTEX_METABOLITE;
						//itemptr->setPen( QPen( QColor( 100, 100, 100, 255 ), 2 ) );
					}
					itemptr->textOn() = false;
				}
		}
	}
	
	_overlapFlag = true;
	_scene->update();
}

GraphicsView::GraphicsView( QWidget *parent )
		: QGraphicsView( parent ) {
//----------------------------------------------------------
// clear stored images
//----------------------------------------------------------
	QString path = "svg/";
	QDir dir( path );
	dir.setNameFilters( QStringList() << "*.*" );
	dir.setFilter( QDir::Files );
	for( const QString &dirFile: dir.entryList() ) {
		dir.remove( dirFile );
	}

//----------------------------------------------------------
// batch for experiment
//----------------------------------------------------------
	
	int batch_argc = qApp->arguments().count();
	for( unsigned int i = 0; i < batch_argc; i++ ) {
		QString batch_argv = qApp->arguments().at( i );
		//if( i == 0 )
		//	cerr << "batch_argv_binary " << i << " = " << batch_argv.toStdString() << endl;
		if( i == 1 ) {
			// _batch_str = batch_argv.toStdString();
			Common::setBatchStr( batch_argv.toStdString() );
		}
	}
	// Common::setBatchStr( "1" );
	// cerr << "batch_argv_id = " << Common::getBatchStr() << endl;

//----------------------------------------------------------
// configuration file
//----------------------------------------------------------
	int default_width = Common::getContentWidth();
	int default_height = Common::getContentHeight();
	setGeometry( 0, 0, default_width, default_height );
	
	string configFilePath = "config/" + Common::getBatchStr() + "/common.conf";
	Base::Config conf( configFilePath );
	
	if( conf.has( "clone_threshold" ) ) {
		string paramCloneThreshold = conf.gets( "clone_threshold" );
		_clonedThreshold = stoi( paramCloneThreshold );
	}
	if( conf.has( "input_path" ) ) {
		string paramInputPath = conf.gets( "input_path" );
		_inputpath = paramInputPath;
	}
	if( conf.has( "tmp_path" ) ) {
		string paramTmpPath = conf.gets( "tmp_path" );
		_tmppath = paramTmpPath;
	}
	if( conf.has( "file_type" ) ) {
		string paramFileType = conf.gets( "file_type" );
		_fileType = paramFileType;
	}
	if( conf.has( "file_freq" ) ) {
		string paramFileFreq = conf.gets( "file_freq" );
		_fileFreq = paramFileFreq;
	}
	if( conf.has( "energy_type" ) ) {
		string paramEnergyType = conf.gets( "energy_type" );
		if( paramEnergyType == "ENERGY_FORCE" )
			_energyType = ENERGY_FORCE;
		else if( paramEnergyType == "ENERGY_STRESS" )
			_energyType = ENERGY_STRESS;
		else
			cerr << "something is wrong here... at " << __LINE__ << " in " << __FILE__ << endl;
	}
	_maxThread = std::thread::hardware_concurrency() - 1; // preserve one thread for main thread
	
	cerr << "filepath: " << configFilePath << endl;
	cerr << "clone_threshold: " << _clonedThreshold << endl;
	cerr << "input_path: " << _inputpath << endl;
	cerr << "tmp_path: " << _tmppath << endl;
	cerr << "file_type: " << _fileFreq << endl;
	cerr << "file_freq: " << _fileType << endl;
	cerr << "energy_type: " << _energyType << endl;
	cerr << "max_thread: " << _maxThread << endl;
	
	setAutoFillBackground( true );
	setBackgroundBrush( QBrush( QColor( 255, 255, 255, 255 ), Qt::SolidPattern ) );
	
	_scene = new QGraphicsScene( this );
	_scene->setSceneRect( -default_width / 2.0, -default_height / 2.0,
	                      default_width, default_height );  // x, y, w, h
	
	//_colorType = COLOR_PREDEFINED;
	_colorType = COLOR_BREWER;
	_is_simplifiedFlag = false;
	_is_skeletonFlag = false;
	_is_compositeFlag = false;
	_is_polygonFlag = false;
	_is_polygonComplexFlag = false;
	_is_boundaryFlag = false;
	_is_subPathwayFlag = false;
	_is_cellFlag = false;
	_is_cellPolygonFlag = false;
	_is_cellPolygonComplexFlag = false;
	_is_roadFlag = false;
	_is_laneFlag = false;
	//_is_mclPolygonFlag = false;
	_is_centerFlag = false;
	_is_centerPolygonFlag = false;
	_is_pathwayPolygonFlag = false;
	_is_pathwayPolygonFlag = false;
	_overlapFlag = false;
	
	//setHorizontalScrollBarPolicy ( Qt::ScrollBarAlwaysOn );
	//setVerticalScrollBarPolicy ( Qt::ScrollBarAlwaysOn );
	//viewport()->scroll( 500, 500 );
	
	this->setScene( _scene );
}

GraphicsView::~GraphicsView() {
}