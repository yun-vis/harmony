
#include "ui/GraphicsView.h"

//------------------------------------------------------------------------------
//	Macro definition
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Protected functions
//------------------------------------------------------------------------------
void GraphicsView::_init( void ) {
	
	_vertex_edge_coverage = _pathwayPtr->nVertices() + _vertex_edge_ratio * _pathwayPtr->nEdges();
}

void GraphicsView::_item_seeds( void ) {
/*
    ForceGraph &c = _levelBorderPtr->regionBase()->forceGraph();

    BGL_FORALL_VERTICES( vd, c, ForceGraph ) {

        GraphicsBallItem *itemptr = new GraphicsBallItem;
        itemptr->fontSize() = _font_size;
        //itemptr->setPen( Qt::NoPen );
        itemptr->setPen( QPen( QColor( 0, 0, 0, 255 ), 2 ) );
        itemptr->setBrush( QBrush( QColor( 0, 0, 0, 255 ), Qt::SolidPattern ) );
        itemptr->setRect( QRectF( c[vd].coordPtr->x(), -c[vd].coordPtr->y(), 10, 10 ) );
        itemptr->id() = c[vd].id;

        //cerr << vertexCoord[vd];
        _scene->addItem( itemptr );
    }
    cerr << "seeds.size() = " << num_vertices( c ) << endl;
*/
	
	if( _energyType == ENERGY_STRESS ) {
		
		BoundaryGraph &b = _levelBorderPtr->regionBase().stress().boundaryGraph();
		BGL_FORALL_VERTICES( vd, b, BoundaryGraph ) {
				
				GraphicsVertexItem *itemptr = new GraphicsVertexItem;
				itemptr->fontSize() = _font_size;
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

void GraphicsView::_item_skeleton( void ) {
	ForceGraph &s = _levelBorderPtr->skeletonForceGraph();
	
	// draw edges
	BGL_FORALL_EDGES( ed, s, ForceGraph ) {
			
			ForceGraph::vertex_descriptor vdS = source( ed, s );
			ForceGraph::vertex_descriptor vdT = target( ed, s );
			QPainterPath path;
			path.moveTo( s[ vdS ].coordPtr->x(), -s[ vdS ].coordPtr->y() );
			path.lineTo( s[ vdT ].coordPtr->x(), -s[ vdT ].coordPtr->y() );
			
			GraphicsEdgeItem *itemptr = new GraphicsEdgeItem;
			s[ ed ].itemPtr = itemptr;
			itemptr->setPen( QPen( QColor( 0, 0, 0, 100 ), 2 ) );
			itemptr->setBrush( QBrush( QColor( 255, 255, 255, 255 ), Qt::SolidPattern ) );
			itemptr->setPath( path );
			
			_scene->addItem( itemptr );
		}
	
	BGL_FORALL_VERTICES( vd, s, ForceGraph ) {
			
			GraphicsVertexItem *itemptr = new GraphicsVertexItem;
			s[ vd ].itemPtr = itemptr;
			itemptr->fontSize() = _font_size;
			itemptr->setPen( QPen( QColor( 0, 0, 0, 100 ), 2 ) );
			itemptr->setBrush( QBrush( QColor( 255, 255, 255, 255 ), Qt::SolidPattern ) );
			itemptr->setRect( QRectF( s[ vd ].coordPtr->x(), -s[ vd ].coordPtr->y(), 10, 10 ) );
			itemptr->id() = s[ vd ].id;
			
			//cerr << vertexCoord[vd];
			_scene->addItem( itemptr );
		}
}

void GraphicsView::_update_item_skeleton( void ) {
	ForceGraph &s = _levelBorderPtr->skeletonForceGraph();
	
	// draw edges
	BGL_FORALL_EDGES( ed, s, ForceGraph ) {
			
			ForceGraph::vertex_descriptor vdS = source( ed, s );
			ForceGraph::vertex_descriptor vdT = target( ed, s );
			QPainterPath path;
			path.moveTo( s[ vdS ].coordPtr->x(), -s[ vdS ].coordPtr->y() );
			path.lineTo( s[ vdT ].coordPtr->x(), -s[ vdT ].coordPtr->y() );
			
			GraphicsEdgeItem *itemptr = s[ ed ].itemPtr;
			//itemptr->setPen( QPen( QColor( 0, 0, 0, 100 ), 2 ) );
			//itemptr->setBrush( QBrush( QColor( 255, 255, 255, 255 ), Qt::SolidPattern ) );
			itemptr->setPath( path );
			
			// _scene->addItem( itemptr );
		}
	
	BGL_FORALL_VERTICES( vd, s, ForceGraph ) {
			
			GraphicsVertexItem *itemptr = s[ vd ].itemPtr;
			//itemptr->fontSize() = _font_size;
			//itemptr->setPen( QPen( QColor( 0, 0, 0, 100 ), 2 ) );
			//itemptr->setBrush( QBrush( QColor( 255, 255, 255, 255 ), Qt::SolidPattern ) );
			itemptr->setRect( QRectF( s[ vd ].coordPtr->x(), -s[ vd ].coordPtr->y(), 10, 10 ) );
			//itemptr->id() = s[vd].id;
			
			//cerr << vertexCoord[vd];
			// _scene->addItem( itemptr );
		}
}

void GraphicsView::_item_composite( void ) {
	
	ForceGraph &s = _levelBorderPtr->regionBase().forceGraph();
	
	// draw edges
	BGL_FORALL_EDGES( ed, s, ForceGraph ) {
			
			ForceGraph::vertex_descriptor vdS = source( ed, s );
			ForceGraph::vertex_descriptor vdT = target( ed, s );
			QPainterPath path;
			path.moveTo( s[ vdS ].coordPtr->x(), -s[ vdS ].coordPtr->y() );
			path.lineTo( s[ vdT ].coordPtr->x(), -s[ vdT ].coordPtr->y() );
			
			GraphicsEdgeItem *itemptr = new GraphicsEdgeItem;
			s[ ed ].itemPtr = itemptr;
			itemptr->setPen( QPen( QColor( 0, 0, 0, 100 ), 2 ) );
			itemptr->setBrush( QBrush( QColor( 255, 255, 255, 255 ), Qt::SolidPattern ) );
			itemptr->setPath( path );
			itemptr->id() = s[ ed ].id;
			itemptr->weight() = s[ ed ].weight;
			// itemptr->textOn() = true;
			
			_scene->addItem( itemptr );
		}
	
	BGL_FORALL_VERTICES( vd, s, ForceGraph ) {
			
			GraphicsVertexItem *itemptr = new GraphicsVertexItem;
			s[ vd ].itemPtr = itemptr;
			itemptr->fontSize() = _font_size;
			itemptr->setPen( QPen( QColor( 0, 0, 0, 100 ), 2 ) );
			itemptr->setBrush( QBrush( QColor( 255, 255, 255, 255 ), Qt::SolidPattern ) );
			itemptr->setRect( QRectF( s[ vd ].coordPtr->x(), -s[ vd ].coordPtr->y(), 10, 10 ) );
			itemptr->id() = s[ vd ].id;
			itemptr->text() = QString::fromStdString( to_string( s[ vd ].id ) );
			//itemptr->textOn() = true;
			
			//cerr << vertexCoord[vd];
			_scene->addItem( itemptr );
		}
}

void GraphicsView::_update_item_composite( void ) {
	ForceGraph &s = _levelBorderPtr->regionBase().forceGraph();
	
	// draw edges
	BGL_FORALL_EDGES( ed, s, ForceGraph ) {
			
			ForceGraph::vertex_descriptor vdS = source( ed, s );
			ForceGraph::vertex_descriptor vdT = target( ed, s );
			QPainterPath path;
			path.moveTo( s[ vdS ].coordPtr->x(), -s[ vdS ].coordPtr->y() );
			path.lineTo( s[ vdT ].coordPtr->x(), -s[ vdT ].coordPtr->y() );
			
			GraphicsEdgeItem *itemptr = s[ ed ].itemPtr;
			//itemptr->setPen( QPen( QColor( 0, 0, 0, 100 ), 2 ) );
			//itemptr->setBrush( QBrush( QColor( 255, 255, 255, 255 ), Qt::SolidPattern ) );
			itemptr->setPath( path );
			//itemptr->id() = s[ed].id;
			//itemptr->weight() = s[ed].weight;
			// itemptr->textOn() = true;
			
			//_scene->addItem( itemptr );
		}
	
	BGL_FORALL_VERTICES( vd, s, ForceGraph ) {
			
			GraphicsVertexItem *itemptr = s[ vd ].itemPtr;
			//itemptr->fontSize() = _font_size;
			//itemptr->setPen( QPen( QColor( 0, 0, 0, 100 ), 2 ) );
			//itemptr->setBrush( QBrush( QColor( 255, 255, 255, 255 ), Qt::SolidPattern ) );
			itemptr->setRect( QRectF( s[ vd ].coordPtr->x(), -s[ vd ].coordPtr->y(), 10, 10 ) );
			//itemptr->id() = s[vd].id;
			//itemptr->text() = QString::fromStdString( to_string( s[vd].id ) );
			//itemptr->textOn() = true;
			
			//cerr << vertexCoord[vd];
			//_scene->addItem( itemptr );
		}
}

void GraphicsView::_item_polygonComplex( void ) {
	
	ForceGraph &s = _levelBorderPtr->skeletonForceGraph();
	
	//vector < vector< Coord2 > > p = _levelBorderPtr->polygons();
	map< unsigned int, Polygon2 > p = _levelBorderPtr->regionBase().polygonComplex();
	map< unsigned int, Polygon2 >::iterator itP = p.begin();
	for( ; itP != p.end(); itP++ ) {
		
		//cerr << " area = " << itP->second.area() << endl;
		QPolygonF polygon;
		for( unsigned int j = 0; j < itP->second.elements().size(); j++ ) {
			polygon.append( QPointF( itP->second.elements()[ j ].x(), -itP->second.elements()[ j ].y() ) );
			// cerr << "x = " << p[i][j].x() << " y = " << p[i][j].y() << endl;
		}
		
		GraphicsPolygonItem *itemptr = new GraphicsPolygonItem;
		itP->second.setItemPtr( itemptr );
		vector< double > rgb;
		ForceGraph::vertex_descriptor vd = vertex( itP->first, s );
		unsigned int gid = s[ vd ].initID;
		
		// pickBrewerColor( gid, rgb );
		_pathwayPtr->pickColor( _colorType, gid, rgb );
		
		QColor color( rgb[ 0 ], rgb[ 1 ], rgb[ 2 ], 100 );
		itemptr->setPen( QPen( QColor( color.red(), color.green(), color.blue(), 255 ), 2 ) );
		itemptr->setBrush( QBrush( QColor( color.red(), color.green(), color.blue(), 100 ), Qt::SolidPattern ) );
		itemptr->setPolygon( polygon );
		
		//cerr << vertexCoord[vd];
		_scene->addItem( itemptr );
	}
	// cerr << "polygon.size() = " << p.size() << endl;
}

void GraphicsView::_update_item_polygonComplex( void ) {
	
	ForceGraph &s = _levelBorderPtr->skeletonForceGraph();
	
	//vector < vector< Coord2 > > p = _levelBorderPtr->polygons();
	map< unsigned int, Polygon2 > p = _levelBorderPtr->regionBase().polygonComplex();
	map< unsigned int, Polygon2 >::iterator itP = p.begin();
	for( ; itP != p.end(); itP++ ) {
		
		//cerr << " area = " << itP->second.area() << endl;
		QPolygonF polygon;
		for( unsigned int j = 0; j < itP->second.elements().size(); j++ ) {
			polygon.append( QPointF( itP->second.elements()[ j ].x(), -itP->second.elements()[ j ].y() ) );
			// cerr << "x = " << p[i][j].x() << " y = " << p[i][j].y() << endl;
		}
		
		GraphicsPolygonItem *itemptr = itP->second.getItemPtr();
		//vector< double > rgb;
		//ForceGraph::vertex_descriptor vd = vertex( itP->first, s );
		//unsigned int gid = s[vd].initID;
		
		// pickBrewerColor( gid, rgb );
		//_pathwayPtr->pickColor( _colorType, gid, rgb );
		
		//QColor color( rgb[0], rgb[1], rgb[2], 100 );
		//itemptr->setPen( QPen( QColor( color.red(), color.green(), color.blue(), 255 ), 2 ) );
		//itemptr->setBrush( QBrush( QColor( color.red(), color.green(), color.blue(), 100 ), Qt::SolidPattern ) );
		itemptr->setPolygon( polygon );
		
		//cerr << vertexCoord[vd];
		//_scene->addItem( itemptr );
	}
	// cerr << "polygon.size() = " << p.size() << endl;
}

void GraphicsView::_item_polygons( void ) {
	
	ForceGraph &fg = _levelBorderPtr->regionBase().forceGraph();
	vector< Seed > &seedVec = *( _levelBorderPtr->regionBase().force().voronoi().seedVec() );
	
	cerr << "_item_polygons size_seed = " << num_vertices( fg ) << endl;
	BGL_FORALL_VERTICES( vd, fg, ForceGraph ) {
			
			Polygon2 &p = *fg[ vd ].cellPtr;
			
			QPolygonF polygon;
			for( unsigned int j = 0; j < p.elements().size(); j++ ) {
				// cerr << "i = " << i << " x = " << p.elements()[j].x() << " y = " << p.elements()[j].y() << endl;
				polygon.append( QPointF( p.elements()[ j ].x(), -p.elements()[ j ].y() ) );
			}
			
			GraphicsPolygonItem *itemptr = new GraphicsPolygonItem;
			p.setItemPtr( itemptr );
			
			vector< double > rgb;
			// ForceGraph::vertex_descriptor vd = vertex( i, s );
			
			unsigned int gid = fg[ vd ].initID;
			_pathwayPtr->pickColor( _colorType, gid, rgb );
			//cerr << rgb[0] << ", " << rgb[1] << ", " << rgb[2] << endl;
			QColor color( rgb[ 0 ], rgb[ 1 ], rgb[ 2 ], 100 );
			itemptr->setPen( QPen( QColor( color.red(), color.green(), color.blue(), 255 ), 2 ) );
			itemptr->setBrush( QBrush( QColor( color.red(), color.green(), color.blue(), 100 ), Qt::SolidPattern ) );
			itemptr->setPolygon( polygon );
			
			//cerr << vertexCoord[vd];
			_scene->addItem( itemptr );
		}
}

void GraphicsView::_update_item_polygons( void ) {
	
	ForceGraph &fg = _levelBorderPtr->regionBase().forceGraph();
	
	BGL_FORALL_VERTICES( vd, fg, ForceGraph ) {
			
			Polygon2 &p = *fg[ vd ].cellPtr;
			
			QPolygonF polygon;
			for( unsigned int j = 0; j < p.elements().size(); j++ ) {
				// cerr << "i = " << i << " x = " << p.elements()[j].x() << " y = " << p.elements()[j].y() << endl;
				polygon.append( QPointF( p.elements()[ j ].x(), -p.elements()[ j ].y() ) );
			}
			
			GraphicsPolygonItem *itemptr = p.getItemPtr();
			
			//vector< double > rgb;
			// ForceGraph::vertex_descriptor vd = vertex( i, s );
			
			//unsigned int gid = fg[ vd ].initID;
			//_pathwayPtr->pickColor( _colorType, gid, rgb );
			//cerr << rgb[0] << ", " << rgb[1] << ", " << rgb[2] << endl;
			//QColor color( rgb[ 0 ], rgb[ 1 ], rgb[ 2 ], 100 );
			//itemptr->setPen( QPen( QColor( color.red(), color.green(), color.blue(), 255 ), 2 ) );
			//itemptr->setBrush( QBrush( QColor( color.red(), color.green(), color.blue(), 100 ), Qt::SolidPattern ) );
			itemptr->setPolygon( polygon );
			
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
		boundaryVecPtr = &_levelCellPtr->octilinearBoundaryVec();
	}
	else if( *_levelTypePtr == LEVEL_CELLCOMPONENT ) {
		boundaryVecPtr = &_levelCellPtr->octilinearBoundaryVec();
	}
	else if( *_levelTypePtr == LEVEL_DETAIL ) {
		boundaryVecPtr = &_levelDetailPtr->octilinearBoundaryVec();
	}
	else if( *_levelTypePtr == LEVEL_ROUTE ) {
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
				
				itemptr->fontSize() = _font_size;
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
		boundaryVecPtr = &_levelCellPtr->octilinearBoundaryVec();
	}
	else if( *_levelTypePtr == LEVEL_CELLCOMPONENT ) {
		boundaryVecPtr = &_levelCellPtr->octilinearBoundaryVec();
	}
	else if( *_levelTypePtr == LEVEL_DETAIL ) {
		boundaryVecPtr = &_levelDetailPtr->octilinearBoundaryVec();
	}
	else if( *_levelTypePtr == LEVEL_ROUTE ) {
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
				//itemptr->fontSize() = _font_size;
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
			
			ForceGraph &dg = itC->second.componentRegion.forceGraph();
			
			// draw edges
			BGL_FORALL_EDGES( ed, dg, ForceGraph ) {
					
					ForceGraph::vertex_descriptor vdS = source( ed, dg );
					ForceGraph::vertex_descriptor vdT = target( ed, dg );
					
					QPainterPath path;
					path.moveTo( dg[ vdS ].coordPtr->x(), -dg[ vdS ].coordPtr->y() );
					path.lineTo( dg[ vdT ].coordPtr->x(), -dg[ vdT ].coordPtr->y() );
					
					// add path
					GraphicsEdgeItem *itemptr = new GraphicsEdgeItem;
					dg[ ed ].itemPtr = itemptr;
					
					itemptr->setPen( QPen( QColor( 0, 0, 0, 125 ), 2 ) );
					//itemptr->setPen( QPen( QColor( 0, 0, 0, 200 ), 2 ) );
					itemptr->setPath( path );
					_scene->addItem( itemptr );
				}
			
			// draw vertices
			BGL_FORALL_VERTICES( vd, dg, ForceGraph ) {
					
					MetaboliteGraph::vertex_descriptor vdF = vertex( dg[ vd ].initID, subg[ itC->second.groupID ] );
					
					MetaboliteGraph::vertex_descriptor initVD = vertex( subg[ itC->second.groupID ][ vdF ].initID, g );
#ifdef DEBUG
					cerr << " itC->second.groupID = " << itC->second.groupID << " dg[vd].initID = " << dg[vd].initID
					 << " subg[itC->second.groupID][vdF].initID = " << subg[itC->second.groupID][vdF].initID << endl;
#endif // DEBUG
					GraphicsVertexItem *itemptr = new GraphicsVertexItem;
					dg[ vd ].itemPtr = itemptr;
					itemptr->fontSize() = _font_size;
					
					if( g[ initVD ].type == "reaction" ) {
						itemptr->vtype() = TYPE_ONE;
						itemptr->setPen( QPen( QColor( 0, 0, 0, 255 ), 2 ) );
					}
					else if( g[ initVD ].type == "metabolite" ) {
						itemptr->vtype() = TYPE_TWO;
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
					
					itemptr->setRect( QRectF( dg[ vd ].coordPtr->x(), -dg[ vd ].coordPtr->y(), 10, 10 ) );
					itemptr->id() = dg[ vd ].id;
					//itemptr->name() = QString::fromStdString( to_string( dg[vd].id ) );
					itemptr->name() = QString::fromStdString( *g[ initVD ].namePtr );
					
					//cerr << vertexCoord[vd];
					_scene->addItem( itemptr );
				}
		}
	}
}

void GraphicsView::_update_item_subpathways( void ) {
	
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
			
			ForceGraph &dg = itC->second.componentRegion.forceGraph();
			
			// draw edges
			BGL_FORALL_EDGES( ed, dg, ForceGraph ) {
					
					ForceGraph::vertex_descriptor vdS = source( ed, dg );
					ForceGraph::vertex_descriptor vdT = target( ed, dg );
					
					QPainterPath path;
					path.moveTo( dg[ vdS ].coordPtr->x(), -dg[ vdS ].coordPtr->y() );
					path.lineTo( dg[ vdT ].coordPtr->x(), -dg[ vdT ].coordPtr->y() );
					
					// add path
					GraphicsEdgeItem *itemptr = dg[ ed ].itemPtr;
					
					itemptr->setPen( QPen( QColor( 0, 0, 0, 125 ), 2 ) );
					//itemptr->setPen( QPen( QColor( 0, 0, 0, 200 ), 2 ) );
					itemptr->setPath( path );
					//_scene->addItem( itemptr );
				}
			
			// draw vertices
			BGL_FORALL_VERTICES( vd, dg, ForceGraph ) {
					
					MetaboliteGraph::vertex_descriptor vdF = vertex( dg[ vd ].initID, subg[ itC->second.groupID ] );
					
					MetaboliteGraph::vertex_descriptor initVD = vertex( subg[ itC->second.groupID ][ vdF ].initID, g );
#ifdef DEBUG
					cerr << " itC->second.groupID = " << itC->second.groupID << " dg[vd].initID = " << dg[vd].initID
					 << " subg[itC->second.groupID][vdF].initID = " << subg[itC->second.groupID][vdF].initID << endl;
#endif // DEBUG
					GraphicsVertexItem *itemptr = dg[ vd ].itemPtr;
					itemptr->fontSize() = _font_size;
					
					if( g[ initVD ].type == "reaction" ) {
						itemptr->vtype() = TYPE_ONE;
						itemptr->setPen( QPen( QColor( 0, 0, 0, 255 ), 2 ) );
					}
					else if( g[ initVD ].type == "metabolite" ) {
						itemptr->vtype() = TYPE_TWO;
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
					
					itemptr->setRect( QRectF( dg[ vd ].coordPtr->x(), -dg[ vd ].coordPtr->y(), 10, 10 ) );
					itemptr->id() = dg[ vd ].id;
					//itemptr->name() = QString::fromStdString( to_string( dg[vd].id ) );
					itemptr->name() = QString::fromStdString( *g[ initVD ].namePtr );
					
					//cerr << vertexCoord[vd];
					//_scene->addItem( itemptr );
				}
		}
	}
}

void GraphicsView::_item_pathways( void ) {
	
	vector< ForceGraph > &lsubg = _pathwayPtr->lsubG();
	
	// create edge object from the spanning tree and add it to the scene
	for( unsigned int i = 0; i < lsubg.size(); i++ ) {
		
		// draw edges
		BGL_FORALL_EDGES( ed, lsubg[ i ], ForceGraph ) {
				
				ForceGraph::vertex_descriptor vdS = source( ed, lsubg[ i ] );
				ForceGraph::vertex_descriptor vdT = target( ed, lsubg[ i ] );
				
				QPainterPath path;
				path.moveTo( lsubg[ i ][ vdS ].coordPtr->x(), -lsubg[ i ][ vdS ].coordPtr->y() );
				path.lineTo( lsubg[ i ][ vdT ].coordPtr->x(), -lsubg[ i ][ vdT ].coordPtr->y() );
				
				// add path
				GraphicsEdgeItem *itemptr = new GraphicsEdgeItem;
				
				itemptr->setPen( QPen( QColor( 0, 0, 0, 125 ), 2 ) );
				//itemptr->setPen( QPen( QColor( 0, 0, 0, 200 ), 2 ) );
				itemptr->setPath( path );
				_scene->addItem( itemptr );
			}
		
		// draw vertices
		BGL_FORALL_VERTICES( vd, lsubg[ i ], ForceGraph ) {
				
				GraphicsVertexItem *itemptr = new GraphicsVertexItem;
				itemptr->fontSize() = _font_size;
				itemptr->setPen( QPen( QColor( 0, 0, 0, 100 ), 2 ) );
				itemptr->setBrush( QBrush( QColor( 255, 255, 255, 255 ), Qt::SolidPattern ) );
				itemptr->setRect( QRectF( lsubg[ i ][ vd ].coordPtr->x(), -lsubg[ i ][ vd ].coordPtr->y(), 10, 10 ) );
				itemptr->id() = lsubg[ i ][ vd ].id;
				
				_scene->addItem( itemptr );
			}
	}
}

void GraphicsView::_item_centers( void ) {
	
	vector< RegionBase > &centerVec = _levelCellPtr->centerVec();
	
	for( unsigned int i = 0; i < centerVec.size(); i++ ) {
		
		ForceGraph &fg = centerVec[ i ].forceGraph();
		
		// draw edges
		BGL_FORALL_EDGES( ed, fg, ForceGraph ) {
				
				ForceGraph::vertex_descriptor vdS = source( ed, fg );
				ForceGraph::vertex_descriptor vdT = target( ed, fg );
				
				QPainterPath path;
				path.moveTo( fg[ vdS ].coordPtr->x(), -fg[ vdS ].coordPtr->y() );
				path.lineTo( fg[ vdT ].coordPtr->x(), -fg[ vdT ].coordPtr->y() );
				
				// add path
				GraphicsEdgeItem *itemptr = new GraphicsEdgeItem;
				fg[ ed ].itemPtr = itemptr;
				
				itemptr->setPen( QPen( QColor( 0, 0, 255, 255 ), 2 ) );
				//itemptr->setPen( QPen( QColor( 0, 0, 0, 200 ), 2 ) );
				itemptr->setPath( path );
				
				_scene->addItem( itemptr );
			}
		
		// draw vertices
		BGL_FORALL_VERTICES( vd, fg, ForceGraph ) {
				
				ForceGraph::degree_size_type degrees = out_degree( vd, fg );
				
				GraphicsVertexItem *itemptr = new GraphicsVertexItem;
				fg[ vd ].itemPtr = itemptr;
				
				itemptr->fontSize() = 2 * _font_size;
				itemptr->setPen( QPen( QColor( 0, 0, 0, 255 ), 2 ) );
				itemptr->setBrush( QBrush( QColor( 0, 0, 255, 255 ), Qt::SolidPattern ) );
				itemptr->setRect( QRectF( fg[ vd ].coordPtr->x(), -fg[ vd ].coordPtr->y(), 10, 10 ) );
				itemptr->id() = fg[ vd ].id;
				itemptr->text() = QString::fromStdString( to_string( fg[ vd ].id ) );
				//itemptr->textOn() = true;
				
				//cerr << vertexCoord[vd];
				_scene->addItem( itemptr );
			}
	}
}

void GraphicsView::_update_item_centers( void ) {
	
	vector< RegionBase > &centerVec = _levelCellPtr->centerVec();
	
	for( unsigned int i = 0; i < centerVec.size(); i++ ) {
		
		ForceGraph &cb = centerVec[ i ].forceGraph();
		
		// draw edges
		BGL_FORALL_EDGES( ed, cb, ForceGraph ) {
				
				ForceGraph::vertex_descriptor vdS = source( ed, cb );
				ForceGraph::vertex_descriptor vdT = target( ed, cb );
				
				QPainterPath path;
				path.moveTo( cb[ vdS ].coordPtr->x(), -cb[ vdS ].coordPtr->y() );
				path.lineTo( cb[ vdT ].coordPtr->x(), -cb[ vdT ].coordPtr->y() );
				
				// add path
				GraphicsEdgeItem *itemptr = cb[ ed ].itemPtr;
				
				//itemptr->setPen( QPen( QColor( 0, 0, 255, 255 ), 2 ) );
				//itemptr->setPen( QPen( QColor( 0, 0, 0, 200 ), 2 ) );
				itemptr->setPath( path );
				
				//_scene->addItem( itemptr );
			}
		
		// draw vertices
		BGL_FORALL_VERTICES( vd, cb, ForceGraph ) {
				
				ForceGraph::degree_size_type degrees = out_degree( vd, cb );
				
				GraphicsVertexItem *itemptr = cb[ vd ].itemPtr;
				
				itemptr->fontSize() = 2 * _font_size;
				//itemptr->setPen( QPen( QColor( 0, 0, 0, 255 ), 2 ) );
				//itemptr->setBrush( QBrush( QColor( 0, 0, 255, 255 ), Qt::SolidPattern ) );
				itemptr->setRect( QRectF( cb[ vd ].coordPtr->x(), -cb[ vd ].coordPtr->y(), 10, 10 ) );
				//itemptr->id() = cb[vd].id;
				//itemptr->text() = QString::fromStdString( to_string( cb[vd].id ) );
				//itemptr->textOn() = true;
				
				//cerr << vertexCoord[vd];
				//_scene->addItem( itemptr );
			}
	}
}

void GraphicsView::_item_centerPolygons( void ) {
	
	vector< RegionBase > &centerVec = _levelCellPtr->centerVec();
	
	for( unsigned int i = 0; i < centerVec.size(); i++ ) {
		
		ForceGraph &fg = centerVec[ i ].forceGraph();
		
		BGL_FORALL_VERTICES( vd, fg, ForceGraph ) {
				
				Polygon2 &p = *fg[ vd ].cellPtr;
				
				QPolygonF polygon;
				for( unsigned int j = 0; j < p.elements().size(); j++ ) {
					// cerr << "i = " << i << " x = " << p.elements()[j].x() << " y = " << p.elements()[j].y() << endl;
					polygon.append( QPointF( p.elements()[ j ].x(), -p.elements()[ j ].y() ) );
				}
				
				GraphicsPolygonItem *itemptr = new GraphicsPolygonItem;
				p.setItemPtr( itemptr );
				
				vector< double > rgb;
				unsigned int gid = fg[ vd ].groupID;
				_pathwayPtr->pickColor( _colorType, gid, rgb );
				// cerr << rgb[0] << ", " << rgb[1] << ", " << rgb[2] << endl;
				QColor color( rgb[ 0 ], rgb[ 1 ], rgb[ 2 ], 100 );
				itemptr->setPen( QPen( QColor( color.red(), color.green(), color.blue(), 255 ), 2 ) );
				itemptr->setBrush(
						QBrush( QColor( color.red(), color.green(), color.blue(), 100 ), Qt::SolidPattern ) );
				itemptr->setPolygon( polygon );
				
				//cerr << vertexCoord[vd];
				_scene->addItem( itemptr );
				
			}
/*
		vector< Seed > &seedVec = *( centerVec[ i ].force().voronoi().seedVec() );
		
		cerr << "seedVec.size() = " << seedVec.size() << endl;
		for( unsigned int i = 0; i < seedVec.size(); i++ ) {
			
			Polygon2 &p = *seedVec[ i ].voronoiCellPtr;
			
			QPolygonF polygon;
			for( unsigned int j = 0; j < p.elements().size(); j++ ) {
				// cerr << "i = " << i << " x = " << p.elements()[j].x() << " y = " << p.elements()[j].y() << endl;
				polygon.append( QPointF( p.elements()[ j ].x(), -p.elements()[ j ].y() ) );
			}
			
			GraphicsPolygonItem *itemptr = new GraphicsPolygonItem;
			p.setItemPtr( itemptr );
			cerr << "itemptr = " << itemptr;
			vector< double > rgb;
			ForceGraph::vertex_descriptor vd = vertex( i, cb );
			
			unsigned int gid = cb[ vd ].groupID;
			_pathwayPtr->pickColor( _colorType, gid, rgb );
			// cerr << rgb[0] << ", " << rgb[1] << ", " << rgb[2] << endl;
			QColor color( rgb[ 0 ], rgb[ 1 ], rgb[ 2 ], 100 );
			itemptr->setPen( QPen( QColor( color.red(), color.green(), color.blue(), 255 ), 2 ) );
			itemptr->setBrush( QBrush( QColor( color.red(), color.green(), color.blue(), 100 ), Qt::SolidPattern ) );
			itemptr->setPolygon( polygon );
			
			//cerr << vertexCoord[vd];
			_scene->addItem( itemptr );
		}
*/
	}
}

void GraphicsView::_update_item_centerPolygons( void ) {
	
	vector< RegionBase > &centerVec = _levelCellPtr->centerVec();
	
	for( unsigned int i = 0; i < centerVec.size(); i++ ) {
		
		ForceGraph &fg = centerVec[ i ].forceGraph();
		
		BGL_FORALL_VERTICES( vd, fg, ForceGraph ) {
				
				Polygon2 &p = *fg[ vd ].cellPtr;
				
				QPolygonF polygon;
				for( unsigned int j = 0; j < p.elements().size(); j++ ) {
					// cerr << "i = " << i << " x = " << p.elements()[j].x() << " y = " << p.elements()[j].y() << endl;
					polygon.append( QPointF( p.elements()[ j ].x(), -p.elements()[ j ].y() ) );
				}
				
				GraphicsPolygonItem *itemptr = p.getItemPtr();
				
				vector< double > rgb;
				unsigned int gid = fg[ vd ].groupID;
				_pathwayPtr->pickColor( _colorType, gid, rgb );
				// cerr << rgb[0] << ", " << rgb[1] << ", " << rgb[2] << endl;
				QColor color( rgb[ 0 ], rgb[ 1 ], rgb[ 2 ], 100 );
				itemptr->setPen( QPen( QColor( color.red(), color.green(), color.blue(), 255 ), 2 ) );
				itemptr->setBrush(
						QBrush( QColor( color.red(), color.green(), color.blue(), 100 ), Qt::SolidPattern ) );
				itemptr->setPolygon( polygon );
				
				//cerr << vertexCoord[vd];
				//_scene->addItem( itemptr );
			}

/*
		vector< Seed > &seedVec = *( centerVec[ i ].force().voronoi().seedVec() );
		
		for( unsigned int i = 0; i < seedVec.size(); i++ ) {
			
			Polygon2 &p = *seedVec[ i ].voronoiCellPtr;
			//cerr << "p.elements().size() = " << p.elements().size() << endl;
			
			QPolygonF polygon;
			for( unsigned int j = 0; j < p.elements().size(); j++ ) {
				//cerr << "j = " << j << " x = " << p.elements()[j].x() << " y = " << p.elements()[j].y() << endl;
				polygon.append( QPointF( p.elements()[ j ].x(), -p.elements()[ j ].y() ) );
			}
			
			cerr << "update_itemptr = " << p.getItemPtr();
			GraphicsPolygonItem *itemptr = p.getItemPtr();
			
			//vector< double > rgb;
			//ForceGraph::vertex_descriptor vd = vertex( i, cb );
			
			//unsigned int gid = cb[vd].groupID;
			//_pathwayPtr->pickColor( _colorType, gid, rgb );
			// cerr << rgb[0] << ", " << rgb[1] << ", " << rgb[2] << endl;
			//QColor color( rgb[0], rgb[1], rgb[2], 100 );
			//itemptr->setPen( QPen( QColor( color.red(), color.green(), color.blue(), 255 ), 2 ) );
			//itemptr->setBrush( QBrush( QColor( color.red(), color.green(), color.blue(), 100 ), Qt::SolidPattern ) );
			itemptr->setPolygon( polygon );
			
			//cerr << vertexCoord[vd];
			//_scene->addItem( itemptr );
		}
*/
	}
}

void GraphicsView::_item_cells( void ) {
	
	vector< RegionBase > &cellVec = _levelCellPtr->cellVec();
	//vector< ForceGraph > &cellGVec   = _levelCellPtr->forceCellGraphVec();
	
	// create edge object from the spanning tree and add it to the scene
	for( unsigned int i = 0; i < cellVec.size(); i++ ) {
		
		// draw edges
		BGL_FORALL_EDGES( ed, cellVec[ i ].forceGraph(), ForceGraph ) {
				
				ForceGraph::vertex_descriptor vdS = source( ed, cellVec[ i ].forceGraph() );
				ForceGraph::vertex_descriptor vdT = target( ed, cellVec[ i ].forceGraph() );
				
				QPainterPath path;
				path.moveTo( cellVec[ i ].forceGraph()[ vdS ].coordPtr->x(),
				             -cellVec[ i ].forceGraph()[ vdS ].coordPtr->y() );
				path.lineTo( cellVec[ i ].forceGraph()[ vdT ].coordPtr->x(),
				             -cellVec[ i ].forceGraph()[ vdT ].coordPtr->y() );
				
				// add path
				GraphicsEdgeItem *itemptr = new GraphicsEdgeItem;
				cellVec[ i ].forceGraph()[ ed ].itemPtr = itemptr;
				
				itemptr->setPen( QPen( QColor( 0, 0, 255, 255 ), 3 ) );
				//itemptr->setPen( QPen( QColor( 0, 0, 0, 200 ), 2 ) );
				itemptr->setPath( path );
				_scene->addItem( itemptr );
			}
		
		// draw vertices
		BGL_FORALL_VERTICES( vd, cellVec[ i ].forceGraph(), ForceGraph ) {
				
				ForceGraph::degree_size_type degrees = out_degree( vd, cellVec[ i ].forceGraph() );
				
				GraphicsVertexItem *itemptr = new GraphicsVertexItem;
				cellVec[ i ].forceGraph()[ vd ].itemPtr = itemptr;
				
				itemptr->fontSize() = 2 * _font_size;
				itemptr->setPen( QPen( QColor( 0, 0, 0, 255 ), 2 ) );
				itemptr->setBrush( QBrush( QColor( 0, 0, 255, 255 ), Qt::SolidPattern ) );
				itemptr->setRect( QRectF( cellVec[ i ].forceGraph()[ vd ].coordPtr->x(),
				                          -cellVec[ i ].forceGraph()[ vd ].coordPtr->y(), 10, 10 ) );
				itemptr->id() = cellVec[ i ].forceGraph()[ vd ].id;
				itemptr->text() = QString::fromStdString( to_string( cellVec[ i ].forceGraph()[ vd ].id ) );
				//itemptr->textOn() = true;
				
				//cerr << vertexCoord[vd];
				_scene->addItem( itemptr );
			}

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
					itemptr->fontSize() = _font_size;
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
			itemptr->fontSize() = _font_size;
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
			itemptr->fontSize() = _font_size;
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
		
		// draw edges
		BGL_FORALL_EDGES( ed, cellVec[ i ].forceGraph(), ForceGraph ) {
				
				ForceGraph::vertex_descriptor vdS = source( ed, cellVec[ i ].forceGraph() );
				ForceGraph::vertex_descriptor vdT = target( ed, cellVec[ i ].forceGraph() );
				
				QPainterPath path;
				path.moveTo( cellVec[ i ].forceGraph()[ vdS ].coordPtr->x(),
				             -cellVec[ i ].forceGraph()[ vdS ].coordPtr->y() );
				path.lineTo( cellVec[ i ].forceGraph()[ vdT ].coordPtr->x(),
				             -cellVec[ i ].forceGraph()[ vdT ].coordPtr->y() );
				
				// add path
				GraphicsEdgeItem *itemptr = cellVec[ i ].forceGraph()[ ed ].itemPtr;
				
				//itemptr->setPen( QPen( QColor( 0, 0, 255, 255 ), 3 ) );
				//itemptr->setPen( QPen( QColor( 0, 0, 0, 200 ), 2 ) );
				itemptr->setPath( path );
				//_scene->addItem( itemptr );
			}
		
		// draw vertices
		BGL_FORALL_VERTICES( vd, cellVec[ i ].forceGraph(), ForceGraph ) {
				
				ForceGraph::degree_size_type degrees = out_degree( vd, cellVec[ i ].forceGraph() );
				
				GraphicsVertexItem *itemptr = cellVec[ i ].forceGraph()[ vd ].itemPtr;
				//itemptr->fontSize() = 2*_font_size;
				//itemptr->setPen( QPen( QColor( 0, 0, 0, 255 ), 2 ) );
				//itemptr->setBrush( QBrush( QColor( 0, 0, 255, 255 ), Qt::SolidPattern ) );
				itemptr->setRect( QRectF( cellVec[ i ].forceGraph()[ vd ].coordPtr->x(),
				                          -cellVec[ i ].forceGraph()[ vd ].coordPtr->y(), 10, 10 ) );
				//itemptr->id() = cellVec[i].forceGraph()[vd].id;
				//itemptr->text() = QString::fromStdString( to_string( cellVec[i].forceGraph()[vd].id ) );
				//itemptr->textOn() = true;
				
				//cerr << vertexCoord[vd];
				//_scene->addItem( itemptr );
			}

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
					itemptr->fontSize() = _font_size;
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
			itemptr->fontSize() = _font_size;
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
			itemptr->fontSize() = _font_size;
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
	
	for( itC = interCCMap.begin(); itC != interCCMap.end(); itC++ ) {
		
		unsigned int idS = itC->first.p();      // subsystem ID
		unsigned int idT = itC->first.q();      // subsystem ID
		CellComponent &ccS = *itC->second.first;
		CellComponent &ccT = *itC->second.second;
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
		itemptr->setPen( QPen( QColor( 255, 0, 0, 100 ), 3 ) );
		itemptr->setBrush( QBrush( QColor( 255, 255, 255, 255 ), Qt::SolidPattern ) );
		itemptr->setPath( path );
		
		_scene->addItem( itemptr );
		
	}
}


void GraphicsView::_item_cellPolygons( void ) {
	
	vector< RegionBase > &cellVec = _levelCellPtr->cellVec();
	//vector< Force >      & cellFVec     = _levelCellPtr->forceCellVec();
	//vector< ForceGraph > & cellGVec     = _levelCellPtr->forceCellGraphVec();
	
	for( unsigned int k = 0; k < cellVec.size(); k++ ) {
		
		ForceGraph &fg = cellVec[ k ].forceGraph();
		
		BGL_FORALL_VERTICES( vd, fg, ForceGraph ) {
				
				Polygon2 &p = *fg[ vd ].cellPtr;
				
				QPolygonF polygon;
				for( unsigned int j = 0; j < p.elements().size(); j++ ) {
					// cerr << "i = " << i << " x = " << p.elements()[j].x() << " y = " << p.elements()[j].y() << endl;
					polygon.append( QPointF( p.elements()[ j ].x(), -p.elements()[ j ].y() ) );
				}
				
				GraphicsPolygonItem *itemptr = new GraphicsPolygonItem;
				p.setItemPtr( itemptr );
				
				vector< double > rgb;
				unsigned int gid = cellVec[ k ].forceGraph()[ vd ].groupID;
				_pathwayPtr->pickColor( _colorType, gid, rgb );
				QColor color( rgb[ 0 ], rgb[ 1 ], rgb[ 2 ], 100 );
				itemptr->setPen( QPen( QColor( color.red(), color.green(), color.blue(), 255 ), 2 ) );
				itemptr->setBrush(
						QBrush( QColor( color.red(), color.green(), color.blue(), 100 ), Qt::SolidPattern ) );
				itemptr->setPolygon( polygon );
				
				//cerr << vertexCoord[vd];
				_scene->addItem( itemptr );
			}

/*
		vector< Seed > &seedVec = *cellVec[ k ].force().voronoi().seedVec();
		
		for( unsigned int i = 0; i < seedVec.size(); i++ ) {
			
			Polygon2 &p = *seedVec[ i ].voronoiCellPtr;
			
			QPolygonF polygon;
			for( unsigned int j = 0; j < p.elements().size(); j++ ) {
				polygon.append( QPointF( p.elements()[ j ].x(), -p.elements()[ j ].y() ) );
				// cerr << "x = " << p[i][j].x() << " y = " << p[i][j].y() << endl;
			}
			
			GraphicsPolygonItem *itemptr = new GraphicsPolygonItem;
			p.setItemPtr( itemptr );
			
			vector< double > rgb;
			ForceGraph::vertex_descriptor vd = vertex( i, cellVec[ k ].forceGraph() );
			
			unsigned int gid = cellVec[ k ].forceGraph()[ vd ].groupID;
			_pathwayPtr->pickColor( _colorType, gid, rgb );
			QColor color( rgb[ 0 ], rgb[ 1 ], rgb[ 2 ], 100 );
			itemptr->setPen( QPen( QColor( color.red(), color.green(), color.blue(), 255 ), 2 ) );
			itemptr->setBrush( QBrush( QColor( color.red(), color.green(), color.blue(), 100 ), Qt::SolidPattern ) );
			itemptr->setPolygon( polygon );
			
			//cerr << vertexCoord[vd];
			_scene->addItem( itemptr );
		}
*/
	}
}

void GraphicsView::_update_item_cellPolygons( void ) {
	
	vector< RegionBase > &cellVec = _levelCellPtr->cellVec();
	//vector< Force >      & cellFVec     = _levelCellPtr->forceCellVec();
	//vector< ForceGraph > & cellGVec     = _levelCellPtr->forceCellGraphVec();
	
	for( unsigned int k = 0; k < cellVec.size(); k++ ) {
		
		ForceGraph &fg = cellVec[ k ].forceGraph();
		
		BGL_FORALL_VERTICES( vd, fg, ForceGraph ) {
				
				Polygon2 &p = *fg[ vd ].cellPtr;
				
				QPolygonF polygon;
				for( unsigned int j = 0; j < p.elements().size(); j++ ) {
					// cerr << "i = " << i << " x = " << p.elements()[j].x() << " y = " << p.elements()[j].y() << endl;
					polygon.append( QPointF( p.elements()[ j ].x(), -p.elements()[ j ].y() ) );
				}
				
				GraphicsPolygonItem *itemptr = p.getItemPtr();
				
				//vector< double > rgb;
				//unsigned int gid = cellVec[ k ].forceGraph()[ vd ].groupID;
				//_pathwayPtr->pickColor( _colorType, gid, rgb );
				//QColor color( rgb[ 0 ], rgb[ 1 ], rgb[ 2 ], 100 );
				//itemptr->setPen( QPen( QColor( color.red(), color.green(), color.blue(), 255 ), 2 ) );
				//itemptr->setBrush(
				//		QBrush( QColor( color.red(), color.green(), color.blue(), 100 ), Qt::SolidPattern ) );
				itemptr->setPolygon( polygon );
				
				//cerr << vertexCoord[vd];
				//_scene->addItem( itemptr );
			}
/*
		vector< Seed > &seedVec = *cellVec[ k ].force().voronoi().seedVec();
		
		for( unsigned int i = 0; i < seedVec.size(); i++ ) {
			
			Polygon2 &p = *seedVec[ i ].voronoiCellPtr;
			
			QPolygonF polygon;
			for( unsigned int j = 0; j < p.elements().size(); j++ ) {
				polygon.append( QPointF( p.elements()[ j ].x(), -p.elements()[ j ].y() ) );
				// cerr << "x = " << p[i][j].x() << " y = " << p[i][j].y() << endl;
			}
			
			GraphicsPolygonItem *itemptr = p.getItemPtr();
			//vector< double > rgb;
			//ForceGraph::vertex_descriptor vd = vertex( i, cellVec[k].forceGraph() );
			
			//unsigned int gid = cellVec[k].forceGraph()[vd].groupID;
			//_pathwayPtr->pickColor( _colorType, gid, rgb );
			//QColor color( rgb[0], rgb[1], rgb[2], 100 );
			//itemptr->setPen( QPen( QColor( color.red(), color.green(), color.blue(), 255 ), 2 ) );
			//itemptr->setBrush( QBrush( QColor( color.red(), color.green(), color.blue(), 100 ), Qt::SolidPattern ) );
			itemptr->setPolygon( polygon );
			
			//cerr << vertexCoord[vd];
			//_scene->addItem( itemptr );
		}
*/
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
			QPolygonF polygon;
			
			for( unsigned int j = 0; j < c->elements().size(); j++ ) {
				polygon.append( QPointF( c->elements()[ j ].x(), -c->elements()[ j ].y() ) );
			}
			
			GraphicsPolygonItem *itemptr = new GraphicsPolygonItem;
			c->setItemPtr( itemptr );
			vector< double > rgb;
			ForceGraph::vertex_descriptor vd = vertex( itC->second.id, cellVec[ k ].forceGraph() );
			
			if( vd != NULL ) {
				
				unsigned int gid = cellVec[ k ].forceGraph()[ vd ].groupID;
				_pathwayPtr->pickColor( _colorType, gid, rgb );
				QColor color( rgb[ 0 ], rgb[ 1 ], rgb[ 2 ], 100 );
				itemptr->setPen( QPen( QColor( color.red(), color.green(), color.blue(), 255 ), 6 ) );
				//itemptr->setPen( QPen( QColor( color.red(), color.green(), color.blue(), 255 ), 8 ) );
				itemptr->setBrush(
						QBrush( QColor( color.red(), color.green(), color.blue(), 100 ), Qt::SolidPattern ) );
				itemptr->setPolygon( polygon );
				//itemptr->id() = cellVec[k].forceGraph()[vd].id;
				//itemptr->textOn() = true;
				
				//cerr << vertexCoord[vd];
				_scene->addItem( itemptr );
			}

#ifdef DEBUG
			for( unsigned int j = 0; j < c.elements().size(); j++ ){

				GraphicsVertexItem *itemptr = new GraphicsVertexItem;
				itemptr->fontSize() = _font_size;
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
			QPolygonF polygon;
			
			for( unsigned int j = 0; j < c->elements().size(); j++ ) {
				polygon.append( QPointF( c->elements()[ j ].x(), -c->elements()[ j ].y() ) );
			}
			
			GraphicsPolygonItem *itemptr = c->getItemPtr();
			
			vector< double > rgb;
			ForceGraph::vertex_descriptor vd = vertex( itC->second.id, cellVec[ k ].forceGraph() );
			
			if( vd != NULL ) {
				
				//unsigned int gid = cellVec[k].forceGraph()[vd].groupID;
				//_pathwayPtr->pickColor( _colorType, gid, rgb );
				//QColor color( rgb[0], rgb[1], rgb[2], 100 );
				//itemptr->setPen( QPen( QColor( color.red(), color.green(), color.blue(), 255 ), 6 ) );
				//itemptr->setPen( QPen( QColor( color.red(), color.green(), color.blue(), 255 ), 8 ) );
				//itemptr->setBrush( QBrush( QColor( color.red(), color.green(), color.blue(), 100 ), Qt::SolidPattern ) );
				itemptr->setPolygon( polygon );
				//itemptr->id() = cellVec[k].forceGraph()[vd].id;
				//itemptr->textOn() = true;
				
				//cerr << vertexCoord[vd];
				//_scene->addItem( itemptr );
			}

#ifdef DEBUG
			for( unsigned int j = 0; j < c.elements().size(); j++ ){

				GraphicsVertexItem *itemptr = new GraphicsVertexItem;
				itemptr->fontSize() = _font_size;
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

/*
void GraphicsView::_item_mclPolygons( void )
{
    vector< multimap< int, CellComponent > > &cellCVec    = _levelCellPtr->cellComponentVec();

    for( unsigned int k = 0; k < cellCVec.size(); k++ ){

        multimap< int, CellComponent > &componentMap = cellCVec[k];
        multimap< int, CellComponent >::iterator itC = componentMap.begin();
        for( ; itC != componentMap.end(); itC++ ){

            // draw polygons
            Force &f = itC->second.mcl.force();

            if( f.voronoi().seedVec() != NULL ){
                vector< Seed > &seedVec = *f.voronoi().seedVec();
                for( unsigned int i = 0; i < seedVec.size(); i++ ) {

                    Polygon2 &p = seedVec[i].voronoiCellPtr;

                    QPolygonF polygon;
                    for( unsigned int j = 0; j < p.elements().size(); j++ ){
                        polygon.append( QPointF( p.elements()[j].x(), -p.elements()[j].y() ) );
                        // cerr << "x = " << p[i][j].x() << " y = " << p[i][j].y() << endl;
                    }

                    GraphicsPolygonItem *itemptr = new GraphicsPolygonItem;
                    vector< double > rgb;

                    _pathwayPtr->pickColor( _colorType, k, rgb );
                    QColor color( rgb[0], rgb[1], rgb[2], 100 );
                    itemptr->setPen( QPen( QColor( color.red(), color.green(), color.blue(), 255 ), 2 ) );
                    itemptr->setBrush( QBrush( QColor( color.red(), color.green(), color.blue(), 100 ), Qt::SolidPattern ) );
                    itemptr->setPolygon( polygon );

                    //cerr << vertexCoord[vd];
                    _scene->addItem( itemptr );
                }
            }
        }
    }
}
*/

void GraphicsView::_item_pathwayPolygons( void ) {
	
	vector< multimap< int, CellComponent > > &cellCVec = _levelCellPtr->cellComponentVec();
	
	for( unsigned int k = 0; k < cellCVec.size(); k++ ) {
		
		
		multimap< int, CellComponent > &componentMap = cellCVec[ k ];
		multimap< int, CellComponent >::iterator itC = componentMap.begin();
		
		
		for( ; itC != componentMap.end(); itC++ ) {
			
			ForceGraph &fg = itC->second.componentRegion.forceGraph();
			
			BGL_FORALL_VERTICES( vd, fg, ForceGraph ) {
					
					Polygon2 &p = *fg[ vd ].cellPtr;
					
					QPolygonF polygon;
					for( unsigned int j = 0; j < p.elements().size(); j++ ) {
						polygon.append( QPointF( p.elements()[ j ].x(), -p.elements()[ j ].y() ) );
						// cerr << "x = " << p[i][j].x() << " y = " << p[i][j].y() << endl;
					}
					
					GraphicsPolygonItem *itemptr = new GraphicsPolygonItem;
					p.setItemPtr( itemptr );
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
					itemptr->setPolygon( polygon );
					
					//cerr << vertexCoord[vd];
					_scene->addItem( itemptr );
				}

/*
			Force &f = itC->second.componentRegion.force();
			
			if( f.voronoi().seedVec() != NULL ) {
				// draw polygons
				vector< Seed > &seedVec = *f.voronoi().seedVec();
				for( unsigned int i = 0; i < seedVec.size(); i++ ) {
					
					Polygon2 &p = *seedVec[ i ].voronoiCellPtr;
					
					QPolygonF polygon;
					for( unsigned int j = 0; j < p.elements().size(); j++ ) {
						polygon.append( QPointF( p.elements()[ j ].x(), -p.elements()[ j ].y() ) );
						// cerr << "x = " << p[i][j].x() << " y = " << p[i][j].y() << endl;
					}
					
					GraphicsPolygonItem *itemptr = new GraphicsPolygonItem;
					p.setItemPtr( itemptr );
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
					itemptr->setPolygon( polygon );
					
					//cerr << vertexCoord[vd];
					_scene->addItem( itemptr );
				}
			}
*/
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
					
					QPolygonF polygon;
					for( unsigned int j = 0; j < p.elements().size(); j++ ) {
						polygon.append( QPointF( p.elements()[ j ].x(), -p.elements()[ j ].y() ) );
						// cerr << "x = " << p[i][j].x() << " y = " << p[i][j].y() << endl;
					}
					
					GraphicsPolygonItem *itemptr = p.getItemPtr();
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
					itemptr->setPolygon( polygon );
					
					//cerr << vertexCoord[vd];
					//_scene->addItem( itemptr );
				}
/*
			Force &f = itC->second.componentRegion.force();
			
			if( f.voronoi().seedVec() != NULL ) {
				// draw polygons
				vector< Seed > &seedVec = *f.voronoi().seedVec();
				for( unsigned int i = 0; i < seedVec.size(); i++ ) {
					
					Polygon2 &p = *seedVec[ i ].voronoiCellPtr;
					
					QPolygonF polygon;
					for( unsigned int j = 0; j < p.elements().size(); j++ ) {
						polygon.append( QPointF( p.elements()[ j ].x(), -p.elements()[ j ].y() ) );
						// cerr << "x = " << p[i][j].x() << " y = " << p[i][j].y() << endl;
					}
					
					GraphicsPolygonItem *itemptr = p.getItemPtr();
					//vector< double > rgb;
					
					//_pathwayPtr->pickColor( _colorType, k, rgb );
					//QColor color( rgb[0], rgb[1], rgb[2], 100 );
					//itemptr->setPen( QPen( QColor( 220, 220, 220, 100 ), 4 ) );
					//if( _is_pathwayPolygonContourFlag == true )
					//    itemptr->setPen( QPen( QColor( color.red(), color.green(), color.blue(), 0 ), 2 ) );
					//else
					//    itemptr->setPen( QPen( QColor( color.red(), color.green(), color.blue(), 255 ), 2 ) );
					//itemptr->setBrush( QBrush( QColor( color.red(), color.green(), color.blue(), 100 ), Qt::SolidPattern ) );
					itemptr->setPolygon( polygon );
					
					//cerr << vertexCoord[vd];
					//_scene->addItem( itemptr );
				}
			}
*/
		}
	}
}

void GraphicsView::_item_road( void ) {
	vector< MetaboliteGraph > &subg = _pathwayPtr->subG();
	UndirectedBaseGraph &road = ( *_roadPtr )[ 0 ].road();
	vector< vector< Highway > > &highwayMat = ( *_roadPtr )[ 0 ].highwayMat();
	// vector< vector< Coord2 > > & roadChaikinCurveVec = _roadPtr->roadChaikinCurve();
	vector< Contour2 > &subsysContour = ( *_roadPtr )[ 0 ].subsysContour();

#ifdef SKIP
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
		itemptr->setPen( QPen( QColor( 0, 0, 0, 200 ), 3 ) );
		itemptr->setPath( path );
		_scene->addItem( itemptr );
	}
#endif // SKIP

#ifdef SKIP
	// draw road path
	vector< Contour2 > & subsysContour = _roadPtr->subsysContour();
	for( unsigned int i = 0; i < subsysContour.size(); i++ ){

		Polygon2 &simpleContour = subsysContour[i].simpleContour();
		QPainterPath path;
		for( unsigned int j = 0; j < componentRegion.fineOutputContour().elements().size(); j++ ){

			Coord2 & coord = componentRegion.fineOutputContour().elements()[j];

			if( j == 0 )
				path.moveTo( coord.x(), -coord.y() );
			else
				path.lineTo( coord.x(), -coord.y() );
			if( j == componentRegion.fineOutputContour().elements().size()-1 )
				path.lineTo( componentRegion.fineOutputContour().elements()[0].x(), -componentRegion.fineOutputContour().elements()[0].y() );
		}

		// add path
		GraphicsEdgeItem *itemptr = new GraphicsEdgeItem;

		//itemptr->setPen( QPen( QColor( 0, 0, 255, 255 ), 3 ) );
		itemptr->setPen( QPen( QColor( 0, 0, 0, 200 ), 3 ) );
		itemptr->setPath( path );
		_scene->addItem( itemptr );
	}
#endif // SKIP
	
	// draw background
	for( unsigned int i = 0; i < subsysContour.size(); i++ ) {
		
		//vector< Coord2 > &p = subsysContour[i].simpleContour().elements();
		vector< Coord2 > &p = subsysContour[ i ].fineContour().elements();
		
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
		
		//vector< Coord2 > &p = subsysContour[i].simpleContour().elements();
		vector< Coord2 > &p = subsysContour[ i ].fineContour().elements();
		
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

#ifdef SKIP
	// draw vertices
	BGL_FORALL_VERTICES( vd, road, UndirectedBaseGraph ) {

		GraphicsVertexItem *itemptr = new GraphicsVertexItem;
		itemptr->fontSize() = _font_size;
		itemptr->setPen( QPen( QColor( 0, 0, 0, 255 ), 2 ) );
		itemptr->setBrush( QBrush( QColor( 0, 0, 255, 255 ), Qt::SolidPattern ) );
		itemptr->setRect( QRectF( road[vd].coordPtr->x(), -road[vd].coordPtr->y(), 10, 10 ) );
		itemptr->id() = road[ vd ].id;

		//cerr << vertexCoord[vd];
		_scene->addItem( itemptr );
	}

	// draw paths
	for( unsigned int i = 0; i < highwayMat.size(); i++ ) {
		for( unsigned int j = 0; j < highwayMat[i].size(); j++ ) {

			if( i != j && ( highwayMat[i][j].common.size() > 0 ) && ( highwayMat[j][i].common.size() > 0 ) ){
				vector< UndirectedBaseGraph::vertex_descriptor > &vdVec = highwayMat[i][j].path;

				QPainterPath path;
				for( unsigned int k = 0; k < vdVec.size(); k++ ){

					if( k == 0 )
						path.moveTo( road[ vdVec[k] ].coordPtr->x(), -road[ vdVec[k] ].coordPtr->y() );
					else
						path.lineTo( road[ vdVec[k] ].coordPtr->x(), -road[ vdVec[k] ].coordPtr->y() );
				}

				// add path
				GraphicsEdgeItem *itemptr = new GraphicsEdgeItem;

				//itemptr->setPen( QPen( QColor( 0, 0, 255, 255 ), 3 ) );
				itemptr->setPen( QPen( QColor( 255, 0, 0, 150 ), 4 ) );
				itemptr->setPath( path );
				_scene->addItem( itemptr );
			}
		}
	}
#endif // SKIP

#ifdef SKIP
	// draw routers
	for( unsigned int i = 0; i < highwayMat.size(); i++ ) {
		for( unsigned int j = 0; j < highwayMat[i].size(); j++ ) {
			if( i != j && ( highwayMat[i][j].common.size() > 0 ) && ( highwayMat[j][i].common.size() > 0 ) ){

				// background
				GraphicsVertexItem *itemptrB = new GraphicsVertexItem;

				vector< double > rgb;
				_pathwayPtr->pickColor( _colorType, i, rgb );
				QColor colorB( rgb[0], rgb[1], rgb[2], 255 );
				itemptrB->fontSize() = _font_size;
				itemptrB->setPen( QPen( QColor( colorB.red(), colorB.green(), colorB.blue(), 255 ), 10 ) );
				itemptrB->setBrush( QBrush( QColor( colorB.red(), colorB.green(), colorB.blue(), 255 ), Qt::SolidPattern ) );
				itemptrB->setRect( QRectF(  road[ highwayMat[i][j].routerVD ].coordPtr->x(),
										  -road[ highwayMat[i][j].routerVD ].coordPtr->y(), 10, 10 ) );
				itemptrB->id() = highwayMat[i][j].id;

				_scene->addItem( itemptrB );

				// foreground
				GraphicsVertexItem *itemptrF = new GraphicsVertexItem;

				_pathwayPtr->pickColor( _colorType, j, rgb );
				QColor colorF( rgb[0], rgb[1], rgb[2], 255 );
				itemptrF->fontSize() = _font_size;
				itemptrF->setPen( QPen( QColor( colorF.red(), colorF.green(), colorF.blue(), 255 ), 3 ) );
				itemptrF->setBrush( QBrush( QColor( colorF.red(), colorF.green(), colorF.blue(), 255 ), Qt::SolidPattern ) );
				itemptrF->setRect( QRectF(  road[ highwayMat[i][j].routerVD ].coordPtr->x(),
										   -road[ highwayMat[i][j].routerVD ].coordPtr->y(), 10, 10 ) );
				itemptrF->id() = highwayMat[j][i].id;

				_scene->addItem( itemptrF );
			}
		}
	}
#endif // SKIP

#ifdef SKIP
	// draw local edges
	for( unsigned int i = 0; i < highwayMat.size(); i++ ) {
		for( unsigned int j = 0; j < highwayMat[i].size(); j++ ) {

			if( i != j && ( highwayMat[i][j].common.size() > 0 ) && ( highwayMat[j][i].common.size() > 0 ) ) {

				map< MetaboliteGraph::vertex_descriptor,
					 MetaboliteGraph::vertex_descriptor > &commonMap = highwayMat[i][j].common;
				map< MetaboliteGraph::vertex_descriptor,
					 MetaboliteGraph::vertex_descriptor >::iterator itC;

				for( itC = commonMap.begin(); itC != commonMap.end(); itC++ ){

					QPainterPath path;
					path.moveTo( subg[i][ itC->first ].coordPtr->x(), -subg[i][ itC->first ].coordPtr->y() );
					path.lineTo( road[ highwayMat[i][j].routerVD ].coordPtr->x(),
								 -road[ highwayMat[i][j].routerVD ].coordPtr->y() );
#ifdef DEBUG
					cerr << "local = " << *subg[i][ itC->first ].coordPtr;
					cerr << "router = " << *road[ highwayMat[i][j].routerVD ].coordPtr << endl;
#endif // DEBUG
					// add path
					GraphicsEdgeItem *itemptr = new GraphicsEdgeItem;

					//itemptr->setPen( QPen( QColor( 0, 0, 255, 255 ), 3 ) );
					itemptr->setPen( QPen( QColor( 0, 255, 0, 150 ), 3 ) );
					itemptr->setPath( path );
					_scene->addItem( itemptr );
				}
			}
		}
	}
#endif // SKIP

#ifdef SKIP
	for( unsigned int i = 0; i < subg.size(); i++ ){

		BGL_FORALL_VERTICES( vd, subg[i], MetaboliteGraph ) {

			if( subg[i][vd].isAlias == true ){

				for( unsigned int j = 0; j < highwayMat[i].size(); j++ ){

					if( i != j && ( highwayMat[j][i].common.size() > 0 ) && ( highwayMat[i][j].common.size() > 0 ) ){
						QPainterPath path;
						path.moveTo( subg[i][vd].coordPtr->x(), -subg[i][vd].coordPtr->y() );

						path.lineTo( road[ highwayMat[i][j].routerVD ].coordPtr->x(),
									 -road[ highwayMat[i][j].routerVD ].coordPtr->y() );

						// add path
						GraphicsEdgeItem *itemptr = new GraphicsEdgeItem;

						//itemptr->setPen( QPen( QColor( 0, 0, 255, 255 ), 3 ) );
						itemptr->setPen( QPen( QColor( 0, 255, 0, 150 ), 3 ) );
						itemptr->setPath( path );
						_scene->addItem( itemptr );
					}
				}
			}
		}
	}
#endif // SKIP
	
	// cerr << "end" << endl;
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
			itemptr->fontSize() = _font_size;
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
				itemptr->fontSize() = _font_size;
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
		if( _is_polygonFlag == true ) _item_polygons();
		if( _is_polygonComplexFlag == true ) _item_polygonComplex();
		if( _is_compositeFlag == true ) _item_composite();
		if( _is_centerFlag == true ) _item_centers();
		if( _is_boundaryFlag == true ) _item_boundary();
	}
	else if( *_levelTypePtr == LEVEL_CELLCENTER ) {
		//if( _is_centerPolygonFlag == true )
		_item_centerPolygons();
		if( _is_centerFlag == true ) _item_centers();
		if( _is_boundaryFlag == true ) _item_boundary();
	}
	else if( *_levelTypePtr == LEVEL_CELLCOMPONENT ) {
		if( _is_cellPolygonComplexFlag == true ) {
			if( _is_laneFlag == true )
				_item_cellPolygonComplex( true );
			else
				_item_cellPolygons();
		}
		else{
		
		}
		_item_cells();
	}
	else if( *_levelTypePtr == LEVEL_DETAIL ) {
		if( _is_cellPolygonComplexFlag == true ) {
			if( _is_laneFlag == true )
				_item_cellPolygonComplex( true );
			else
				_item_cellPolygonComplex( false );
		}
		else{
			_item_pathwayPolygons();
		}
		_item_subpathways();
		if( _is_roadFlag == true ) _item_road();        // cluster boundary
		if( _is_laneFlag == true ) _item_lane();        // route connecting duplicated nodes
	}

/*
	if( _is_polygonFlag == true ) _item_polygons();
	if( _is_polygonComplexFlag == true ) _item_polygonComplex();
	if( _is_compositeFlag == true ) _item_composite();  // circular
	if( _is_skeletonFlag == true ) _item_skeleton();    // three
	// if( _is_polygonFlag == true ) _item_seeds();
	
	if( _is_centerPolygonFlag == true ) _item_centerPolygons();
	
	if( _is_centerFlag == true ) {
		_item_centers();
		_item_interCellComponents();
		_item_cells();
	}
	if( _is_cellPolygonFlag == true ) _item_cellPolygons();
	
	if( _is_cellPolygonComplexFlag == true ) {
		if( _is_laneFlag == true )
			_item_cellPolygonComplex( true );
		else
			_item_cellPolygonComplex( false );
	}
	
	// if( _is_mclPolygonFlag == true ) _item_mclPolygons();
	if( _is_pathwayPolygonFlag == true ) _item_pathwayPolygons();
	if( _is_boundaryFlag == true ) _item_boundary();
	if( _is_roadFlag == true ) _item_road();        // cluster boundary
	if( _is_laneFlag == true ) _item_lane();        // route connecting duplicated nodes
	
	if( _is_subPathwayFlag == true ) _item_subpathways();
	// cerr << "_scene.size = " << _scene->items().size() << endl;
*/
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
		itemptr->fontSize() = _font_size;
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

	polygon.append( QPointF( 310.792, 328.956 ) );
	polygon.append( QPointF( 310.792, 237.622 ) );
	polygon.append( QPointF( 242.897, 169.727 ) );
	polygon.append( QPointF( 242.897, 101.929 ) );
	polygon.append( QPointF( 197.856, 56.8884 ) );
	polygon.append( QPointF( 197.856, 40.2206 ) );
	polygon.append( QPointF( 141.324, -16.3117 ) );
	polygon.append( QPointF( 47.915, -16.3117 ) );
	polygon.append( QPointF( 18.0214, -46.2052 ) );
	polygon.append( QPointF( -60.4159, -46.203 ) );
	polygon.append( QPointF( -122.3, 15.6823 ) );
	polygon.append( QPointF( -200.527, -62.5449 ) );
	polygon.append( QPointF( -60.4159, -46.203 ) );
	polygon.append( QPointF( -120.294, -106.078 ) );
	polygon.append( QPointF( -176.043, -106.078 ) );
	polygon.append( QPointF( -226.514, -156.549 ) );
	polygon.append( QPointF( -323.773, -156.549 ) );
	polygon.append( QPointF( -323.773, -244.297 ) );
	polygon.append( QPointF( -455.697, -244.297 ) );
	polygon.append( QPointF( -455.697, 328.956 ) );

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
		if( _is_polygonFlag == true ) _update_item_polygons();
		if( _is_polygonComplexFlag == true ) _update_item_polygonComplex();
		if( _is_compositeFlag == true ) _update_item_composite();
		if( _is_centerFlag == true ) _update_item_centers();
		if( _is_boundaryFlag == true ) _update_item_boundary();
	}
	else if( *_levelTypePtr == LEVEL_CELLCENTER ) {
		if( _is_centerPolygonFlag == true ) _update_item_centerPolygons();
		if( _is_centerFlag == true ) _update_item_centers();
		if( _is_boundaryFlag == true ) _update_item_boundary();
	}
	else if( *_levelTypePtr == LEVEL_CELLCOMPONENT ) {
		if( _is_cellPolygonComplexFlag == true ) {
			if( _is_laneFlag == true )
				_update_item_cellPolygonComplex( true );
			else
				_update_item_cellPolygons();
		}
		else{
		}
		_update_item_cells();
	}
	else if( *_levelTypePtr == LEVEL_DETAIL ) {
		if( _is_cellPolygonComplexFlag == true ) {
			if( _is_laneFlag == true )
				_update_item_cellPolygonComplex( true );
			else
				_update_item_cellPolygonComplex( false );
		}
		else{
			_update_item_pathwayPolygons();
		}
		_update_item_subpathways();
	}

/*
	if( _is_polygonFlag == true ) _update_item_polygons();
	if( _is_polygonComplexFlag == true ) _update_item_polygonComplex();
	if( _is_compositeFlag == true ) _update_item_composite();
	if( _is_skeletonFlag == true ) _update_item_skeleton();
	// if( _is_polygonFlag == true ) _item_seeds();
	
	if( _is_centerPolygonFlag == true ) _update_item_centerPolygons();
	if( _is_centerFlag == true ) {
		_item_centers();
		_item_interCellComponents();
		_update_item_cells();
	}
	if( _is_cellPolygonFlag == true ) _update_item_cellPolygons();
	
	if( _is_cellPolygonComplexFlag == true ) {
		if( _is_laneFlag == true )
			_update_item_cellPolygonComplex( true );
		else
			_update_item_cellPolygonComplex( false );
	}
	
	// if( _is_mclPolygonFlag == true ) _item_mclPolygons();
	if( _is_pathwayPolygonFlag == true ) _update_item_pathwayPolygons();
	if( _is_boundaryFlag == true ) _update_item_boundary();
	if( _is_roadFlag == true ) _item_road();        // cluster boundary
	if( _is_laneFlag == true ) _item_lane();        // route connecting duplicated nodes
	
	if( _is_subPathwayFlag == true ) _update_item_subpathways();
*/
}

//------------------------------------------------------------------------------
//	Event handlers
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Public functions
//------------------------------------------------------------------------------
void GraphicsView::exportPNG( double x, double y, double w, double h ) {
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
	
	if( newPath.isEmpty() ) return;
	
	QImage screenshot( w, h, QImage::Format_RGB32 ); // maximum 32767x32767
	
	//int dpm = 300 / 0.0254; // ~300 DPI
	//screenshot.setDotsPerMeterX( dpm );
	//screenshot.setDotsPerMeterY( dpm );
	
	QPainter painter( &screenshot );
	painter.setRenderHint( QPainter::Antialiasing );
	painter.fillRect( 0, 0, w, h, Qt::white );
	_scene->render( &painter );
	screenshot.save( newPath );
	
	id++;
}

void GraphicsView::exportSVG( double x, double y, double w, double h ) {
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
// configuration file
//----------------------------------------------------------
	int default_width = 0, default_height = 0;
	string configFilePath = "config/common.conf";
	Base::Config conf( configFilePath );
	
	if( conf.has( "font_size" ) ) {
		string paramFont = conf.gets( "font_size" );
		_font_size = Common::stringToDouble( paramFont );
	}
	if( conf.has( "vertex_edge_ratio" ) ) {
		string paramVERatio = conf.gets( "vertex_edge_ratio" );
		_vertex_edge_ratio = Common::stringToDouble( paramVERatio );
	}
	if( conf.has( "default_width" ) ) {
		string paramWidth = conf.gets( "default_width" );
		default_width = stoi( paramWidth );
	}
	if( conf.has( "default_height" ) ) {
		string paramHeight = conf.gets( "default_height" );
		default_height = stoi( paramHeight );
	}
	setGeometry( 0, 0, default_width, default_height );
	
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
	cerr << "font_size: " << _font_size << endl;
	cerr << "vertex_edge_ratio: " << _vertex_edge_ratio << endl;
	cerr << "default_width: " << default_width << endl;
	cerr << "default_height: " << default_height << endl;
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
	
	//setHorizontalScrollBarPolicy ( Qt::ScrollBarAlwaysOn );
	//setVerticalScrollBarPolicy ( Qt::ScrollBarAlwaysOn );
	//viewport()->scroll( 500, 500 );
	
	this->setScene( _scene );
}

GraphicsView::~GraphicsView() {
}