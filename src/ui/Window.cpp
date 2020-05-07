#include "ui/Window.h"

//----------------------------------------------------------
// Window
//----------------------------------------------------------
Window::Window( QWidget *parent )
		: QMainWindow( parent ) {
	
	_gv = new GraphicsView( this );
	setCentralWidget( _gv );
	
	setGeometry( 0, 0, _gv->width(), _gv->height() );
	_levelType = LEVEL_BORDER;
	
	//_octilinearVecPtr = new vector< Octilinear * >;

#ifdef SKIP
	createActions();
	createMenus();
#endif // SKIP

#ifdef RECORD_VIDEO
	_timerVideoStart();
#endif // RECORD_VIDEO
}

Window::~Window() {
}

Window::Window( const Window &obj ) {
	
	_gv = obj._gv;
	_levelBorderPtr = obj._levelBorderPtr;
	_octilinearBoundaryVecPtr = obj._octilinearBoundaryVecPtr;
	
	// cells of subgraphs
	_levelCellPtr = obj._levelCellPtr;
	_roadPtr = obj._roadPtr;
	_lanePtr = obj._lanePtr;
	
	// display
	_content_width = obj._content_width;
	_content_height = obj._content_height;
}

void Window::_init( void ) {
	
	_content_width = width() - LEFTRIGHT_MARGIN;
	_content_height = height() - TOPBOTTOM_MARGIN;
	
	// initialization
	_levelBorderPtr = new LevelBorder;
	_levelCellPtr = new LevelCell;
	_levelDetailPtr = new LevelDetail;
	
	_roadPtr = new vector< Road >;
	_lanePtr = new vector< Road >;
	
	_levelCellPtr->setPathwayData( _pathwayPtr, *_pathwayPtr->width(), *_pathwayPtr->height() );
	
	_gv->setPathwayData( _pathwayPtr, *_pathwayPtr->width(), *_pathwayPtr->height() );
	_gv->setRegionData( &_levelType, _octilinearBoundaryVecPtr,
	                    _levelBorderPtr, _levelCellPtr, _levelDetailPtr,
	                    _roadPtr, _lanePtr );
	
}


void Window::_simulateKey( Qt::Key key ) {
	
	QKeyEvent eventP( QEvent::KeyPress, key, Qt::NoModifier );
	QApplication::sendEvent( this, &eventP );
	QKeyEvent eventR( QEvent::KeyRelease, key, Qt::NoModifier );
	QApplication::sendEvent( this, &eventR );
}


void Window::redrawAllScene( void ) {
	
	_pathwayPtr->pathwayMutex().lock();
	_gv->initSceneItems();
	_pathwayPtr->pathwayMutex().unlock();
	
	_gv->update();
	_gv->scene()->update();
	update();
	repaint();

#ifdef DEBUG
	cerr << "_is_polygonFlag = " << _gv->isPolygonFlag() << endl;
	cerr << "_is_polygonComplexFlag = " << _gv->isPolygonComplexFlag() << endl;
#endif // DEBUG
	
	QCoreApplication::processEvents();
}

void Window::updateAllScene( void ) {
	_pathwayPtr->pathwayMutex().lock();
	_gv->updateSceneItems();
	_pathwayPtr->pathwayMutex().unlock();
	
	// _gv->update();
	_gv->scene()->update();
	// update();
	// repaint();
	
	QCoreApplication::processEvents();
}

#ifdef RECORD_VIDEO
void Window::_timerVideoStart( void )
{
	_timerVideo.start( 3000, this );
	_timerVideoID = _timerVideo.timerId();
	cerr << "_timerVideo.timerId = " << _timerVideo.timerId() << endl;
}

void Window::_timerVideoStop( void )
{
	_timerVideo.stop();
}

void Window::timerVideo( void )
{
	cerr << "processing events..." << endl;
	QCoreApplication::processEvents();
	_simulateKey( Qt::Key_E );
}
#endif // RECORD_VIDEO


void Window::_threadBoundaryForce( void ) {
	
	// initialization
	ctpl::thread_pool pool( _gv->maxThread() ); // limited thread number in the pool
	
	// create a new thread
	ThreadLevelBorder tlh;
	//vector < unsigned int > indexVec;
	
	tlh.setPathwayData( _pathwayPtr, *_pathwayPtr->width(), *_pathwayPtr->height() );
	tlh.setRegionData( &_levelType, _octilinearBoundaryVecPtr,
	                   _levelBorderPtr, _levelCellPtr, _levelDetailPtr,
	                   _roadPtr, _lanePtr );
	tlh.init( THREAD_BOUNDARY, _gv->energyType(), 0, 0, _levelBorderPtr->regionBase().force().paramForceLoop() );
	
	// signle thread
	//tlh.run( 0 );
	// multi-thread
	pool.push( []( int id, ThreadLevelBorder *tlh ) { tlh->run( id ); }, &tlh );
	
	// rendering
#ifdef DEBUG
	redrawAllScene();
	while( pool.n_idle() != _gv->maxThread() ) {

#ifdef DEBUG
		cerr << "pool.n_idle() = " << pool.n_idle() << " _gv->maxThread() = " << _gv->maxThread() << endl;
#endif // DEBUG
		this_thread::sleep_for( chrono::milliseconds( SLEEP_TIME ) );
		updateAllScene();
	}
#endif // DEBUG
	
	// wait for all computing threads to finish and stop all threads
	pool.stop( true );
	cerr << "End of BorderForce..." << endl;
}

void Window::_threadCellCenterForce( void ) {
	
	// initialization
	ctpl::thread_pool pool( _gv->maxThread() ); // limited thread number in the pool
	
	vector< ThreadLevelCellCenter * > tlc;
	tlc.resize( _levelCellPtr->centerVec().size() );
	//cerr << "_levelCellPtr->centerVec().size() = " << _levelCellPtr->centerVec().size() << endl;
	//for( unsigned int i = 0; i < 2; i++ ){
	for( unsigned int i = 0; i < tlc.size(); i++ ) {
		
		// create a new thread
		tlc[ i ] = new ThreadLevelCellCenter;
		
		tlc[ i ]->setPathwayData( _pathwayPtr, *_pathwayPtr->width(), *_pathwayPtr->height() );
		tlc[ i ]->setRegionData( &_levelType, _octilinearBoundaryVecPtr,
				_levelBorderPtr, _levelCellPtr, _levelDetailPtr,
				_roadPtr, _lanePtr );
		
		tlc[ i ]->init( THREAD_CENTER, _gv->energyType(), i, 0,
		                _levelCellPtr->centerVec()[ i ].force().paramForceLoop() );
		//tlc[ i ]->run( 0 );
		pool.push( []( int id, ThreadLevelCellCenter *t ) { t->run( id ); }, tlc[ i ] );
	}

#ifdef DEBUG
	// rendering
	redrawAllScene();
	while( pool.n_idle() != _gv->maxThread() ) {
		
		//cerr << "pool.n_idle() = " << pool.n_idle() << endl;
		this_thread::sleep_for( chrono::milliseconds( SLEEP_TIME ) );
		updateAllScene();
	}
#endif // DEBUG

	// wait for all computing threads to finish and stop all threads
	pool.stop( true );
	
	// clear the memory
	for( unsigned int i = 0; i < _levelCellPtr->centerVec().size(); i++ ) {
		delete tlc[ i ];
	}

	cerr << "End of CellCenterForce..." << endl;
}

void Window::_threadCellComponentForce( void ) {
	
	// initialization
	ctpl::thread_pool pool( _gv->maxThread() ); // limited thread number in the pool
	
	// cerr << "size = " << _levelCellPtr->cellVec().size() << endl;
	vector< ThreadLevelCellComponent * > tlm;
	tlm.resize( _levelCellPtr->cellVec().size() );
	
	//for( unsigned int i = 0; i < 2; i++ ){
	for( unsigned int i = 0; i < tlm.size(); i++ ) {
		
		// create a new thread
		tlm[ i ] = new ThreadLevelCellComponent;
		tlm[ i ]->setPathwayData( _pathwayPtr, *_pathwayPtr->width(), *_pathwayPtr->height() );
		tlm[ i ]->setRegionData( &_levelType, _octilinearBoundaryVecPtr,
				_levelBorderPtr, _levelCellPtr, _levelDetailPtr,
				_roadPtr, _lanePtr );
		tlm[ i ]->init( THREAD_CELL, _gv->energyType(), i, 0, _levelCellPtr->cellVec()[ i ].force().paramForceLoop() );
		//tlm[ i ]->run( 0 );
		pool.push( []( int id, ThreadLevelCellComponent *t ) { t->run( id ); }, tlm[ i ] );
	}
	
	// rendering
#ifdef DEBUG
	redrawAllScene();
	while( pool.n_idle() != _gv->maxThread() ) {
		
		cerr << "pool.n_idle() = " << pool.n_idle() << endl;
		this_thread::sleep_for( chrono::milliseconds( SLEEP_TIME ) );
		updateAllScene();
	}
#endif // DEBUG
	
	// wait for all computing threads to finish and stop all threads
	pool.stop( true );
	
	// clear the memory
	for( unsigned int i = 0; i < _levelCellPtr->cellVec().size(); i++ ) {
		delete tlm[ i ];
	}
	cerr << "End of CellComponentForce..." << endl;
	
	//_simulateKey( Qt::Key_W );
}

void Window::_threadPathwayForce( void ) {
	
	_gv->isCellPolygonComplexFlag() = false;
	_gv->isPathwayPolygonFlag() = true;
	
	// initialization
	vector< multimap< int, CellComponent > > &cellComponentVec = _levelCellPtr->cellComponentVec();
	ctpl::thread_pool pool( _gv->maxThread() ); // limited thread number in the pool
	vector< vector< ThreadLevelDetail * > > tld;
	
	vector< vector< unsigned int > > idMat;
	idMat.resize( cellComponentVec.size() );
	tld.resize( idMat.size() );
	unsigned int idD = 0;
	unsigned int threadNo = 0;
	for( unsigned int i = 0; i < idMat.size(); i++ ) {
		
		multimap< int, CellComponent > &cellComponentMap = cellComponentVec[ i ];
		idMat[ i ].resize( cellComponentMap.size() );
		tld[ i ].resize( idMat[ i ].size() );
		for( unsigned int j = 0; j < idMat[ i ].size(); j++ ) {
			
			idMat[ i ][ j ] = idD;
			idD++;
			threadNo++;
		}
		//cerr << "no. of component = " << idMat[ i ].size() << endl;
	}
	//cerr << "no. of threads = " << threadNo << endl;
	
	unsigned int idC = 0;
	for( unsigned int i = 0; i < cellComponentVec.size(); i++ ) {
		
		multimap< int, CellComponent > &cellComponentMap = cellComponentVec[ i ];
		
		for( unsigned int j = 0; j < cellComponentMap.size(); j++ ) {
			
			multimap< int, CellComponent >::iterator itC = cellComponentMap.begin();
			advance( itC, j );
			CellComponent &c = itC->second;
			
			c.componentRegion.force().id() = idC;
			c.componentRegion.force().init( &c.componentRegion.forceGraph(),
			                                &c.componentRegion.fineOutputContour().contour(), &_levelType,
			                                "config/pathway.conf" );
			
			tld[ i ][ j ] = new ThreadLevelDetail;
			tld[ i ][ j ]->setPathwayData( _pathwayPtr, *_pathwayPtr->width(), *_pathwayPtr->height() );
			tld[ i ][ j ]->setRegionData( &_levelType, _octilinearBoundaryVecPtr,
					_levelBorderPtr, _levelCellPtr, _levelDetailPtr,
					_roadPtr, _lanePtr );
			tld[ i ][ j ]->init( THREAD_PATHWAY, _gv->energyType(), i, j, c.componentRegion.force().paramForceLoop() );
			//tld[ i ][ j ]->run( 0 );
			pool.push( []( int id, ThreadLevelDetail *t ) { t->run( id ); }, tld[ i ][ j ] );
			
			idC++;
		}
	}
	
	// rendering
//#ifdef DEBUG
	redrawAllScene();
	while( pool.n_idle() != _gv->maxThread() ) {
		
		// cerr << "pool.n_idle() = " << pool.n_idle() << endl;
		this_thread::sleep_for( chrono::milliseconds( SLEEP_TIME ) );
		updateAllScene();
	}
//#endif // DEBUG
	
	// wait for all computing threads to finish and stop all threads
	//pool.stop( true );
	
	// clear the memory
	for( unsigned int i = 0; i < cellComponentVec.size(); i++ ) {
		
		multimap< int, CellComponent > &cellComponentMap = cellComponentVec[ i ];
		
		for( unsigned int j = 0; j < cellComponentMap.size(); j++ ) {
			delete tld[ i ][ j ];
		}
	}
	cerr << "End of PathwayForce..." << endl;
}

void Window::steinertree( void ) {
	
	// select a target metabolite
	MetaboliteGraph &g = _pathwayPtr->g();
	unsigned int treeSize = 1;
	
	// highlight
	BGL_FORALL_VERTICES( vd, g, MetaboliteGraph ) {
			
			if( g[ vd ].type == "metabolite" ) {
				//if( *g[ vd ].namePtr == "Glucose" ){          // KEGG
				//if( *g[ vd ].namePtr == "coke[r]" ) {            // tiny
					//if( (*g[ vd ].namePtr == "glx[m]") || (*g[ vd ].namePtr == "coke[r]") ){            // tiny
					if( *g[ vd ].namePtr == "glu_L[c]" ){         // VHM
					//if( *g[ vd ].namePtr == "Soy_Sauce" ){        // food
					//if( *g[ vd ].namePtr == "Beans" ){        // food
					//if( *g[ vd ].namePtr == "Prawns" ){        // food
					//if( *g[ vd ].namePtr == "Bay_Leaf" ){        // food
					//if( *g[ vd ].namePtr == "Sunflower_Oil" ){    // food
					*g[ vd ].isSelectedPtr = true;
					*g[ vd ].selectedIDPtr = 0;
				}
#ifdef SKIP
				if( *g[ vd ].namePtr == "Worcestershire_Sauce" ){
					*g[ vd ].isSelectedPtr = true;
					*g[ vd ].selectedIDPtr = 1;
				}
				if(	*g[ vd ].namePtr == "Puff_Pastry" ){
					*g[ vd ].isSelectedPtr = true;
					*g[ vd ].selectedIDPtr = 2;
				}
				if(	*g[ vd ].namePtr == "Egg_White" ) {            // tiny
					*g[ vd ].isSelectedPtr = true;
					*g[ vd ].selectedIDPtr = 3;
				}
#endif // SKIP
			}
		}
	
	// initialization
	_roadPtr->clear();
	_roadPtr->resize( treeSize );
	_lanePtr->clear();
	_lanePtr->resize( treeSize );
	
	for( int i = 0; i < treeSize; i++ ) {
		
		// compute steiner tree
		( *_roadPtr )[ i ].setPathwayData( _pathwayPtr, *_pathwayPtr->width(), *_pathwayPtr->height() );
		( *_roadPtr )[ i ].initRoad( _levelCellPtr, i );
		( *_roadPtr )[ i ].buildRoad();
		// cerr << "road built..." << endl;
		
		( *_lanePtr )[ i ].setPathwayData( _pathwayPtr, *_pathwayPtr->width(), *_pathwayPtr->height() );
		( *_lanePtr )[ i ].initSteinerNet( _levelCellPtr->cellComponentVec(), i );
		( *_lanePtr )[ i ].steinerTree();
	}

#ifdef SKIP
	// compute steiner tree
	_roadPtr->initRoad( _levelCellPtr );
	// _roadPtr->initRoad( _levelCellPtr->cellComponentVec() );
	_roadPtr->buildRoad();
	// cerr << "road built..." << endl;
	_gv->isRoadFlag() = true;

	_lanePtr->clear();
	_lanePtr->resize( 1 );
	for( unsigned int i = 0; i < _lanePtr->size(); i++ ){
		vector < Highway > &highwayVec = _roadPtr->highwayMat()[i];
		(*_lanePtr)[0].setPathwayData( _pathwayPtr, *_pathwayPtr->width(), *_pathwayPtr->height() );
		(*_lanePtr)[0].initLane( 0, _levelCellPtr->cellComponentVec()[0], &highwayVec );
		(*_lanePtr)[0].steinerTree();
	}
#endif // SKIP
}

void Window::_threadOctilinearBoundary( void ) {
	
	// initialization
	const int iter = 50;
	ctpl::thread_pool pool( _gv->maxThread() ); // limited thread number in the pool
	
	vector< Octilinear * > &boundaryVec = *_octilinearBoundaryVecPtr;
	unsigned int size = boundaryVec.size();
	vector< ThreadOctilinearBoundary * > tob;
	tob.resize( size );
	
	//cerr << "octilinearBoundary: iter = " << iter << endl;
	for( unsigned int i = 0; i < size; i++ ) {
		
		//cerr << " boundaryVec[i].nVertices() = " << num_vertices( boundaryVec[ i ]->boundary() ) << endl;
		
		// create a new thread
		tob[ i ] = new ThreadOctilinearBoundary;
		tob[ i ]->setRegionData( &_levelType, _octilinearBoundaryVecPtr,
				_levelBorderPtr, _levelCellPtr, _levelDetailPtr,
				_roadPtr, _lanePtr );
		tob[ i ]->init( boundaryVec[ i ], iter, boundaryVec[ i ]->opttype(), 10 );
		
		//tob[ i ]->run( 0 );
		pool.push([]( int id, ThreadOctilinearBoundary *t ){ t->run( id ); }, tob[i] );
	}
	
	// rendering
#ifdef DEBUG
	while( pool.n_idle() != _gv->maxThread() ) {
		
		//cerr << "pool.n_idle() = " << pool.n_idle() << endl;
		this_thread::sleep_for( chrono::milliseconds( SLEEP_TIME ) );
		redrawAllScene();
	}
#endif // DEBUG
	
	// wait for all computing threads to finish and stop all threads
	pool.stop( true );
	
	// clear the memory
	for( unsigned int i = 0; i < size; i++ ) {
		delete tob[ i ];
	}
	
	cerr << "End of OctilinearBoundary..." << endl;
}

//
//  Window::selectLevelDetailBuildBoundary --    build the boundary from the voronoi cell
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Window::selectLevelDetailBuildBoundary( void ) {
	
	buildLevelDetailBoundaryGraph();
	_gv->isSimplifiedFlag() = false;
	_gv->isBoundaryFlag() = true;
	redrawAllScene();
}

//
//  Window::buildLevelDetailBoundaryGraph --    build the boundary from the voronoi cell
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Window::buildLevelDetailBoundaryGraph( void ) {
	
	vector< multimap< int, CellComponent > > &cellCVec = _levelCellPtr->cellComponentVec();
	
	// initialization
	vector< Octilinear * > &boundaryVec = *_octilinearBoundaryVecPtr;
	_levelBorderPtr->regionBase().polygonComplexVD().clear();
	boundaryVec.clear();
	
	unsigned int index = 0;
	for( unsigned int m = 0; m < cellCVec.size(); m++ ) {
		
		multimap< int, CellComponent > &componentMap = cellCVec[ m ];
		multimap< int, CellComponent >::iterator itC = componentMap.begin();
		
		index += componentMap.size();
	}
	boundaryVec.resize( index );
	
	index = 0;
	for( unsigned int m = 0; m < cellCVec.size(); m++ ) {
		
		multimap< int, CellComponent > &componentMap = cellCVec[ m ];
		multimap< int, CellComponent >::iterator itC = componentMap.begin();
		
		for( ; itC != componentMap.end(); itC++ ) {
			
			boundaryVec[ index ] = new Octilinear;
			
			BoundaryGraph &bg = boundaryVec[ index ]->boundary();
			unsigned int nVertices = num_vertices( bg );
			unsigned int nEdges = num_edges( bg );
			//unsigned int &nLines = boundaryVec[ index ]->nLines();
			
			// initialization
			bg.clear();
			nVertices = nEdges = 0;//nLines = 0;
			resetVisitedTimes( bg );
			Force &f = itC->second.componentRegion.force();
			
			// draw polygons
			vector< Seed > &seedVec = *f.voronoi().seedVec();
			for( unsigned int i = 0; i < seedVec.size(); i++ ) {
				
				Polygon2 &polygon = *seedVec[ i ].voronoiCellPtr;
				vector< ForceGraph::vertex_descriptor > vdVec;
				
				unsigned int size = polygon.elements().size();
				for( unsigned int j = 1; j < size + 1; j++ ) {
					
					// Check if the station exists or not
					BoundaryGraph::vertex_descriptor curVDS = NULL;
					BoundaryGraph::vertex_descriptor curVDT = NULL;
					
					// add vertices
					for( unsigned int k = 0; k < 2; k++ ) {
						
						BoundaryGraph::vertex_descriptor curVD = NULL;
						
						// Check if the station exists or not
						BGL_FORALL_VERTICES( vd, bg, BoundaryGraph ) {
								Coord2 &c = *bg[ vd ].coordPtr;
								if( fabs( ( polygon.elements()[ ( j - 1 + k ) % ( int ) size ] - c ).norm() ) < 1e-2 ) {

#ifdef DEBUG
									cerr << "The node has been in the database." << endl;
#endif  // DEBUG
									if( k == 0 ) curVD = curVDS = vd;
									if( k == 1 ) curVD = curVDT = vd;
									break;
								}
							}
						
						if( curVD == NULL ) {
							
							curVD = add_vertex( bg );
							//vector< unsigned int > lineID = bg[ curVD ].lineID;
							//lineID.push_back( nLines );
							
							double x = polygon.elements()[ j - 1 + k ].x();
							double y = polygon.elements()[ j - 1 + k ].y();
							bg[ curVD ].geoPtr = new Coord2( x, y );
							bg[ curVD ].smoothPtr = new Coord2( x, y );
							bg[ curVD ].coordPtr = new Coord2( x, y );
							
							bg[ curVD ].id = bg[ curVD ].initID = nVertices;
							bg[ curVD ].namePtr = new string( to_string( bg[ curVD ].id ) );
							bg[ curVD ].weight = 1.0;
							//bg[ curVD ].lineID.push_back( nLines );
							
							if( k == 0 ) curVDS = curVD;
							if( k == 1 ) curVDT = curVD;
							nVertices++;
						}
						else {
							//bg[ curVD ].lineID.push_back( nLines );
#ifdef DEBUG
							cerr << "[Existing] curV : lineID = " << endl;
							for ( unsigned int k = 0; k < vertexLineID[ curVD ].size(); ++k )
								cerr << "lineID[ " << setw( 3 ) << k << " ] = " << vertexLineID[ curVD ][ k ] << endl;
#endif  // DEBUG
						}
					}
					vdVec.push_back( curVDS );
					// cerr << _octilinearBoundaryVec[ curVDS ].id << " ";
					// cerr << "( " << _octilinearBoundaryVec[ curVDS ].id << ", " << _octilinearBoundaryVec[ curVDT ].id << " )" << endl;
					
					// add edges
					// search previous edge
					bool found = false;
					BoundaryGraph::edge_descriptor oldED;
					//BoundaryGraph::vertex_descriptor oldVS = ptrSta[ origID ];
					//BoundaryGraph::vertex_descriptor oldVT = ptrSta[ destID ];
					//unsigned int indexS = _octilinearBoundaryVec[ curVDS ].initID;
					//unsigned int indexT = _octilinearBoundaryVec[ curVDT ].initID;
					tie( oldED, found ) = edge( curVDS, curVDT, bg );
					
					
					if( found == true ) {
						
						//bg[ oldED ].lineID.push_back( nLines );
						bg[ oldED ].visitedTimes += 1;
						//eachline.push_back( oldED );
						//bool test = false;
						//tie( oldED, test ) = edge( oldVT, oldVS, _octilinearBoundaryVec );
					}
					else {
						
						// handle fore edge
						pair< BoundaryGraph::edge_descriptor, unsigned int > foreE = add_edge( curVDS, curVDT, bg );
						BoundaryGraph::edge_descriptor foreED = foreE.first;
						
						// calculate geographical angle
						Coord2 coordO;
						Coord2 coordD;
						if( bg[ curVDS ].initID < bg[ curVDT ].initID ) {
							coordO = *bg[ curVDS ].coordPtr;
							coordD = *bg[ curVDT ].coordPtr;
						}
						else {
							coordO = *bg[ curVDT ].coordPtr;
							coordD = *bg[ curVDS ].coordPtr;
						}
						double diffX = coordD.x() - coordO.x();
						double diffY = coordD.y() - coordO.y();
						double angle = atan2( diffY, diffX );
						
						bg[ foreED ].initID = bg[ foreED ].id = nEdges;
						bg[ foreED ].weight = 1.0;
						bg[ foreED ].geoAngle = angle;
						bg[ foreED ].smoothAngle = angle;
						bg[ foreED ].angle = angle;
						//bg[ foreED ].lineID.push_back( nLines );
						bg[ foreED ].visitedTimes = 0;
						
						//eachline.push_back( foreED );
#ifdef  DEBUG
						cout << "nEdges = " << _nEdges << " angle = " << angle << endl;
#endif  // DEBUG
						nEdges++;
					}
				}
				//map< unsigned int, vector< BoundaryGraph::vertex_descriptor > >::iterator itP;
				itC->second.componentRegion.polygonComplexVD().insert(
						pair< unsigned int, vector< BoundaryGraph::vertex_descriptor > >
								( i, vdVec ) );
#ifdef DEBUG
				cerr << i << ", " << boundaryVec.size() << endl;
				cerr << " vdVec.size() = " << vdVec.size() << " ?= " << size << endl;
				for( unsigned int q = 0; q < vdVec.size(); q++ ) {
					cerr << "id = " << bg[ vdVec[ q ] ].id << endl;
				}
				//id++;
#endif // DEBUG
			}
			
			// update the fixed flag
			BGL_FORALL_EDGES( ed, bg, BoundaryGraph ) {
					if( bg[ ed ].visitedTimes == 0 ) {
						
						BoundaryGraph::vertex_descriptor vdS = source( ed, bg );
						BoundaryGraph::vertex_descriptor vdT = target( ed, bg );
						
						bg[ vdS ].isFixed = true;
						bg[ vdT ].isFixed = true;
						
						// cerr << "eid = " << bg[ ed ].id << " node = " << bg[ vdS ]. id << " ," << bg[ vdT ].id << endl;
					}
				}
			
			boundaryVec[ index ]->prepare( _content_width / 2.0, _content_height / 2.0 );
			index++;
			
			//printGraph( bg );
			//cerr << "DetailBuildBoundary::nV = " << num_vertices( bg ) << endl;
		}
	}
	
	//cerr << "finishing building the detailed graph..." << endl;
}

//
//  Window::updateLevelDetailPolygonComplex --    update the ploygon complex
//
//  Inputs
//  none
//
//  Outputs
//  none
//
void Window::updateLevelDetailPolygonComplex( void ) {
	
	//cerr << "updating componentRegion polygonComplex after optimization ..." << endl;
	vector< multimap< int, CellComponent > > &cellCVec = _levelCellPtr->cellComponentVec();
	vector< Octilinear * > &boundaryVec = *_octilinearBoundaryVecPtr;
	// cerr << "boundaryVec.size() = " << boundaryVec.size() << endl;
	
	unsigned int index = 0;
	for( unsigned int m = 0; m < cellCVec.size(); m++ ) {
		
		multimap< int, CellComponent > &componentMap = cellCVec[ m ];
		multimap< int, CellComponent >::iterator itC = componentMap.begin();
		
		//for (unsigned int n = 0; n < 1; n++) {
		for( ; itC != componentMap.end(); itC++ ) {
			
			//cerr << "index = " << index << endl;
			Octilinear &b = *boundaryVec[ index ];
			BoundaryGraph &bg = b.boundary();
			
			// printGraph( bg );
			// cerr << endl << endl << endl;
			
			RegionBase &detail = itC->second.componentRegion;
			Force &force = detail.force();
			vector< Seed > &seedVec = *force.voronoi().seedVec();
			for( unsigned int i = 0; i < seedVec.size(); i++ ) {
				
				map< unsigned int, vector< BoundaryGraph::vertex_descriptor > >::iterator itP = detail.polygonComplexVD().begin();
				advance( itP, i );
				
				Polygon2 &polygon = *seedVec[ i ].voronoiCellPtr;
				// cerr << "polygon.size() = " << polygon.elements().size() << endl;
				// cerr << "vd.size() = " << itP->second.size() << endl;
				for( unsigned int j = 0; j < polygon.elements().size(); j++ ) {
					// cerr << "i = " << i << " j = " << j << endl;
					// cerr << " " << polygon.elements()[j];
					// cerr << "id = " << bg[ itP->second[j] ].id << endl;
					polygon.elements()[ j ].x() = bg[ itP->second[ j ] ].coordPtr->x();
					polygon.elements()[ j ].y() = bg[ itP->second[ j ] ].coordPtr->y();
				}
				polygon.updateCentroid();
				ForceGraph::vertex_descriptor vd = vertex( i, detail.forceGraph() );
				detail.forceGraph()[ vd ].coordPtr->x() = polygon.centroid().x();
				detail.forceGraph()[ vd ].coordPtr->y() = polygon.centroid().y();
			}
			index++;
		}
	}
	
	// collect cell boundary simpleContour polygons
	for( unsigned int i = 0; i < cellCVec.size(); i++ ) {
		
		multimap< int, CellComponent > &cellComponentMap = cellCVec[ i ];
		multimap< int, CellComponent >::iterator itC;
		
		// cerr << "i = " << i << " size = " << cellComponentMap.size() << endl;
		for( itC = cellComponentMap.begin(); itC != cellComponentMap.end(); itC++ ) {
			
			CellComponent &component = itC->second;
			unsigned int subsysID = component.groupID;
			Polygon2 &c = component.componentRegion.fineOutputContour().contour();
		}
	}
	
	// update cell simpleContour
	// _levelCellPtr->createPolygonComplexFromDetailGraph();
	
	// update simpleContour
	// _levelCellPtr->updatePolygonComplexFromDetailGraph();
}


double computeCV( vector< double > data ) {
	
	double sum = 0.0, mean, standardDeviation = 0.0;
	
	for( unsigned int i = 0; i < data.size(); i++ ) {
		sum += data[ i ];
	}
	mean = sum / ( double ) data.size();
	
	for( unsigned int i = 0; i < data.size(); i++ ) {
		standardDeviation += pow( data[ i ] - mean, 2 );
	}
	
	return sqrt( standardDeviation / ( double ) data.size() ) / mean;
}

#ifdef GMAP
void Window::spaceCoverage( void )
{
	cerr << "HERE" << endl;
	UndirectedPropertyGraph g;
	//_pathwayPtr->loadDot( g, "dot/small-gmap.dot" );
	//_pathwayPtr->loadDot( g, "dot/small-bubblesets.dot" );
	//_pathwayPtr->loadDot( g, "dot/small-mapsets.dot" );
	//_pathwayPtr->loadDot( g, "dot/metabolic-gmap.dot" );
	//_pathwayPtr->loadDot( g, "dot/metabolic-bubblesets.dot" );
	_pathwayPtr->loadDot( g, "dot/metabolic-mapsets.dot" );
	//_pathwayPtr->loadDot( g, "dot/recipe-gmap.dot" );
	//_pathwayPtr->loadDot( g, "dot/recipe-bubblesets.dot" );
	//_pathwayPtr->loadDot( g, "dot/recipe-mapsets.dot" );

	VertexIndexMap              vertexIndex     = get( vertex_index, g );
	VertexPosMap                vertexPos       = get( vertex_mypos, g );
	VertexLabelMap              vertexLabel     = get( vertex_mylabel, g );
	VertexColorMap              vertexColor     = get( vertex_mycolor, g );
	VertexXMap                  vertexX         = get( vertex_myx, g );
	VertexYMap                  vertexY         = get( vertex_myy, g );
	EdgeIndexMap                edgeIndex       = get( edge_index, g );
	EdgeWeightMap               edgeWeight      = get( edge_weight, g );


	int neighborNo = 5;
	vector< double > neighbor;
	vector< double > area;
	Polygon2 fineOutputContour;

	double minX = INFINITY, maxX = -INFINITY, minY = INFINITY, maxY = -INFINITY;
	BGL_FORALL_VERTICES( vd, g, UndirectedPropertyGraph ) {
		if( minX > vertexX[vd] ) minX = vertexX[vd];
		if( minY > vertexY[vd] ) minY = vertexY[vd];
		if( maxX < vertexX[vd] ) maxX = vertexX[vd];
		if( maxY < vertexY[vd] ) maxY = vertexY[vd];
	}

	cerr << "mixX = " << minX << " minY = " << minY << endl;
	cerr << "maxX = " << maxX << " maxY = " << maxY << endl;

	// pathway
	//componentRegion.fineOutputContour().elements().push_back( Coord2( -13.56, 35.725 ) );
	//componentRegion.fineOutputContour().elements().push_back( Coord2( 2226.7, 35.725 ) );
	//componentRegion.fineOutputContour().elements().push_back( Coord2( 2226.7, 1668.6 ) );
	//componentRegion.fineOutputContour().elements().push_back( Coord2( -13.56, 1668.6 ) );
	// metabolic
	componentRegion.fineOutputContour().elements().push_back( Coord2( 84.629, 210.51 ) );
	componentRegion.fineOutputContour().elements().push_back( Coord2( 4430.1 , 210.51 ) );
	componentRegion.fineOutputContour().elements().push_back( Coord2( 4430.1, 3889.4 ) );
	simpleContour.elements().push_back( Coord2( 84.6297 , 3889.4 ) );
	// recipe
	//componentRegion.fineOutputContour().elements().push_back( Coord2( 0.45266, 9.6315 ) );
	//simpleContour.elements().push_back( Coord2( 5199.9, 9.6315 ) );
	//contourPtr.elements().push_back( Coord2( 5199.9, 4588.6 ) );
	//contourPtr.elements().push_back( Coord2( 0.45266, 4588.69 ) );

	// voronoi
	Voronoi v;
	vector< Seed > seedVec;
	BGL_FORALL_VERTICES( vd, g, UndirectedPropertyGraph ) {

		Seed seed;
		seed.id = vertexIndex[vd];
		seed.weight = 1.0;
		seed.coordPtr = Coord2( vertexX[vd], vertexY[vd] );
		seedVec.push_back( seed );
	}
	v.init( seedVec, fineOutputContour );
	v.id() = 0;
	cerr << "HERE" << endl;
	v.createVoronoiDiagram( false );  // true: weighted, false: uniformed
	cerr << "HERE" << endl;
	for( unsigned int i = 0; i < seedVec.size(); i++ ){
		Polygon2 &p = seedVec[i].voronoiCellPtr;
		p.updateCentroid();
		area.push_back( p.area() );
		cerr << "area = " << p.area() << endl;
	}

	// neighbor
	vector< Coord2 > coordVec;
	BGL_FORALL_VERTICES( vd, g, UndirectedPropertyGraph ) {

		coordVec.push_back( Coord2( vertexX[vd], vertexY[vd] ) );
	}

	for( unsigned int i = 0; i < coordVec.size(); i++ ){

		multimap< double, double > sortedDist;
		for( unsigned int j = 0; j < coordVec.size(); j++ ){

			double dist = (coordVec[i] - coordVec[j]).norm();
			sortedDist.insert( pair< double, double >( dist, dist ) );
		}
		double sum = 0;
		for( unsigned int j = 0; j < neighborNo; j++ ){
			multimap< double, double >::iterator it = sortedDist.begin();
			advance( it, j );
			sum += it->first;
		}
		neighbor.push_back( sum );
	}

	cerr << "neighbor CV = " << computeCV( neighbor ) << endl;
	cerr << "area CV = " << computeCV( area ) << endl;
}
#endif // GMAP


void Window::spaceCoverage( void ) {
	
	int neighborNo = 5;
	vector< double > neighbor;
	vector< double > area;
	vector< ForceGraph > &lsubg = _pathwayPtr->lsubG();
	Polygon2 contour;
	
	contour.elements().push_back( Coord2( -0.5 * _content_width, -0.5 * _content_height ) );
	contour.elements().push_back( Coord2( +0.5 * _content_width, -0.5 * _content_height ) );
	contour.elements().push_back( Coord2( +0.5 * _content_width, +0.5 * _content_height ) );
	contour.elements().push_back( Coord2( -0.5 * _content_width, +0.5 * _content_height ) );
	// cerr << "content_width = " << _content_width << " content_height = " << _content_height << endl;
	
	// voronoi
	Voronoi v;
	vector< Seed > seedVec;
	unsigned int index = 0;
	for( unsigned int i = 0; i < lsubg.size(); i++ ) {
		
		BGL_FORALL_VERTICES( vd, lsubg[ i ], ForceGraph ) {
				
				Seed seed;
				seed.id = lsubg[ i ][ vd ].id + index;
				seed.weight = 1.0;
				seed.coordPtr = lsubg[ i ][ vd ].coordPtr;
				seed.voronoiCellPtr = new Polygon2;
				seedVec.push_back( seed );
			}
		index += num_vertices( lsubg[ i ] );
	}
	v.init( seedVec, contour );
	v.id() = 0;
	v.createVoronoiDiagram( false );  // true: weighted, false: uniformed
	
	for( unsigned int i = 0; i < seedVec.size(); i++ ) {
		Polygon2 &p = *seedVec[ i ].voronoiCellPtr;
		p.updateCentroid();
		area.push_back( p.area() );
		//cerr << "area = " << p.area() << endl;
	}
	
	// neighbor
	vector< Coord2 > coordVec;
	for( unsigned int i = 0; i < lsubg.size(); i++ ) {
		
		BGL_FORALL_VERTICES( vd, lsubg[ i ], ForceGraph ) {
				
				coordVec.push_back( *lsubg[ i ][ vd ].coordPtr );
			}
	}
	for( unsigned int i = 0; i < coordVec.size(); i++ ) {
		
		multimap< double, double > sortedDist;
		for( unsigned int j = 0; j < coordVec.size(); j++ ) {
			
			double dist = ( coordVec[ i ] - coordVec[ j ] ).norm();
			sortedDist.insert( pair< double, double >( dist, dist ) );
		}
		double sum = 0;
		for( unsigned int j = 0; j < neighborNo; j++ ) {
			multimap< double, double >::iterator it = sortedDist.begin();
			advance( it, j );
			sum += it->first;
		}
		neighbor.push_back( sum );
	}

#ifdef DEBUG
	cerr << "neighbor CV = " << computeCV( neighbor ) << endl;
	cerr << "area CV = " << computeCV( area ) << endl;
#endif // DEBUG
}


void Window::timerEvent( QTimerEvent *event ) {
	Q_UNUSED( event );

#ifdef RECORD_VIDEO
	if( event->timerId() == _timerVideoID ){
		// cerr << "event->timerId() = " << event->timerId() << endl;
		timerVideo();
	}
#endif // RECORD_VIDEO
	
	redrawAllScene();
}


void Window::keyPressEvent( QKeyEvent *event ) {
	
	switch( event->key() ) {
	
	case Qt::Key_1: {
		
		//****************************************
		// initialization
		//****************************************
		_levelType = LEVEL_BORDER;
		_gv->isPolygonFlag() = true;
		
		//****************************************
		// optimization
		//****************************************
		_threadBoundaryForce();
		//_simulateKey( Qt::Key_2 );
		
		_simulateKey( Qt::Key_E );
		
		break;
	}
	case Qt::Key_2: {
		
		//****************************************
		// initialization
		//****************************************
		_octilinearBoundaryVecPtr = &_levelBorderPtr->octilinearBoundaryVec();
		_levelBorderPtr->prepare();
		_gv->isSimplifiedFlag() = false;
		_gv->isBoundaryFlag() = true;
		_gv->isPolygonFlag() = false;
		_gv->isCompositeFlag() = false;
		_gv->isPolygonComplexFlag() = true;
		_gv->isCenterFlag() = true;
		
		//****************************************
		// optimization
		//****************************************
		_threadOctilinearBoundary();
		_levelBorderPtr->updatePolygonComplex();
		
		_simulateKey( Qt::Key_L );
		
		//****************************************
		// rendering
		//****************************************
		_simulateKey( Qt::Key_E );
		
		break;
	}
	case Qt::Key_Q: {
		
		//----------------------------------------
		// initialization
		//----------------------------------------
		_levelType = LEVEL_CELLCENTER;
		_gv->isPolygonComplexFlag() = false;
		_gv->isCenterPolygonFlag() = true;
		_gv->isCenterFlag() = true;
		
		_simulateKey( Qt::Key_E );
		
		//----------------------------------------
		// optimization
		//----------------------------------------
		_threadCellCenterForce();
		//_simulateKey( Qt::Key_W );
		
		//----------------------------------------
		// rendering
		//----------------------------------------
		_simulateKey( Qt::Key_E );
		
		break;
	}
	case Qt::Key_W: {
		
		//----------------------------------------
		// initialization
		//----------------------------------------
		_octilinearBoundaryVecPtr = &_levelCellPtr->octilinearBoundaryVec();
		_gv->isPolygonComplexFlag() = true;
		_gv->isCenterPolygonFlag() = false;
		_gv->isCenterFlag() = false;
		_gv->isCellFlag() = true;
		_gv->isCompositeFlag() = false;
		_gv->isPolygonFlag() = false;
		_gv->isCellPolygonComplexFlag() = true;
		// _gv->isBoundaryFlag() = false;
		
		//----------------------------------------
		// optimization
		//----------------------------------------
		// update initial center position
		_levelCellPtr->updateCenterCoords();
		
		//----------------------------------------
		// rendering
		//----------------------------------------
		_simulateKey( Qt::Key_E );
		
		break;
	}
	case Qt::Key_A: {
		
		//----------------------------------------
		// initialization
		//----------------------------------------
		_levelType = LEVEL_CELLCOMPONENT;
		_gv->isPolygonComplexFlag() = false;
		_gv->isCellPolygonFlag() = true;
		_gv->isCellFlag() = true;
		
		_simulateKey( Qt::Key_E );
		
		//----------------------------------------
		// optimization
		//----------------------------------------
		_threadCellComponentForce();
		//_simulateKey( Qt::Key_S );
		
		//----------------------------------------
		// rendering
		//----------------------------------------
		_simulateKey( Qt::Key_E );
		
		break;
	}
	case Qt::Key_S: {
		
		//----------------------------------------
		// initialization
		//----------------------------------------
		_gv->isCellPolygonFlag() = false;
		_gv->isCellFlag() = false;
		_gv->isCellPolygonComplexFlag() = true;
		_gv->isSimplifiedFlag() = false;
		
		//----------------------------------------
		// optimization
		//----------------------------------------
		// initialization and build the boundary
		_levelCellPtr->createPolygonComplex();
		_levelCellPtr->prepare();
		
		_threadOctilinearBoundary();
		_levelCellPtr->updatePolygonComplex();
		
		_gv->isBoundaryFlag() = true;
		_gv->isPolygonComplexFlag() = false;
		
		//----------------------------------------
		// rendering
		//----------------------------------------
		_simulateKey( Qt::Key_E );
		
		break;
	}
	case Qt::Key_Z: {
		
		//----------------------------------------
		// initialization
		//----------------------------------------
		_levelType = LEVEL_DETAIL;
		_gv->isCellPolygonComplexFlag() = false;
		_gv->isPathwayPolygonFlag() = true;
		//_gv->isMCLPolygonFlag() = false;
		_gv->isSubPathwayFlag() = true;
		
		//----------------------------------------
		// optimization
		//----------------------------------------
		_levelCellPtr->updatePathwayCoords();
		_threadPathwayForce();
		//_simulateKey( Qt::Key_X );
		
		//----------------------------------------
		// rendering
		//----------------------------------------
		_simulateKey( Qt::Key_E );
		
		break;
	}
	case Qt::Key_X: {
		
		//----------------------------------------
		// initialization
		//----------------------------------------
		_octilinearBoundaryVecPtr = &_levelDetailPtr->octilinearBoundaryVec();
		// initialization and build the boundary
		selectLevelDetailBuildBoundary();
		
		//----------------------------------------
		// optimization
		//----------------------------------------
		_threadOctilinearBoundary();
		updateLevelDetailPolygonComplex();
		
		//----------------------------------------
		// rendering
		//----------------------------------------
		_simulateKey( Qt::Key_E );
		
		break;
	}
	case Qt::Key_R: {
		
		_gv->isBoundaryFlag() = false;
		_gv->isRoadFlag() = true;
		_gv->isPathwayPolygonFlag() = false;
		_gv->isLaneFlag() = true;
		_gv->isCellPolygonComplexFlag() = true;
		
		// steiner tree
		steinertree();
		_simulateKey( Qt::Key_E );
		break;
	}
	case Qt::Key_D: {
		_pathwayPtr->exportDot();
		_pathwayPtr->exportEdges();
		break;
	}
	case Qt::Key_3: {
		_gv->isPathwayPolygonContourFlag() = !_gv->isPathwayPolygonContourFlag();
		redrawAllScene();
		break;
	}
	case Qt::Key_4: {
		_gv->isSkeletonFlag() = !_gv->isSkeletonFlag();
		redrawAllScene();
		break;
	}
	case Qt::Key_5: {
		_gv->isCompositeFlag() = !_gv->isCompositeFlag();
		redrawAllScene();
		break;
	}
	case Qt::Key_6: {
		_gv->isPolygonFlag() = !_gv->isPolygonFlag();
		redrawAllScene();
		break;
	}
	case Qt::Key_7: {
		_gv->isPolygonComplexFlag() = !_gv->isPolygonComplexFlag();
		redrawAllScene();
		break;
	}
	case Qt::Key_8: {
		_gv->isBoundaryFlag() = !_gv->isBoundaryFlag();
		redrawAllScene();
		break;
	}
	case Qt::Key_9: {
		_gv->isCellPolygonFlag() = !_gv->isCellPolygonFlag();
		_gv->isCellFlag() = !_gv->isCellFlag();
		redrawAllScene();
		break;
	}
	case Qt::Key_0: {
		_gv->isSubPathwayFlag() = !_gv->isSubPathwayFlag();
		redrawAllScene();
		break;
	}
	case Qt::Key_L: {
		
		//cerr << "_levelBorderPtr->regionBase().polygonComplex() = "
		//     << _levelBorderPtr->regionBase().polygonComplex().size() << endl;
		// initialize cell
		_levelCellPtr->init( &_content_width, &_content_height,
//		                     &_gv->veCoverage(), &_gv->veRatio(),
		                     &_levelBorderPtr->regionBase().polygonComplex() );
		
		// initialize subgraph layout
		_pathwayPtr->initLayout( _levelBorderPtr->regionBase().polygonComplex() );
		
		break;
	}
	case Qt::Key_V: {
		
		// read and initialize the data
		_pathwayPtr->init( _gv->inputPath(), _gv->tmpPath(),
		                   _gv->fileFreq(), _gv->fileType(),
		                   _gv->cloneThreshold() );
		_gv->init();
		
		// canvasArea: content width and height
		// labelArea: total area of text labels
		double labelArea = 0.0;
		map< string, Subdomain * > &sub = _pathwayPtr->subsys();
		for( map< string, Subdomain * >::iterator it = sub.begin();
		     it != sub.end(); it++ ) {
			labelArea += it->second->idealArea;
			// cerr << "area = " << it->second->idealArea << endl;
		}

#ifdef DEBUG
		cerr << "canvas = " << width() * height() << " labelArea = " << labelArea << endl;
		cerr << "nV = " << _pathwayPtr->nVertices() << " nE = " << _pathwayPtr->nEdges()
			 << " veCoverage = " << _gv->veCoverage() << endl;
#endif // DEBUG
		
		// default canvas size
		double ratio = ( double ) width() / ( double ) height();
		double x = sqrt( pow( labelArea, 1.75 ) / ( double ) _pathwayPtr->nVertices() / ratio );
		//double x = sqrt( labelArea * _gv->veCoverage() / ( double ) _pathwayPtr->nVertices() / ratio );
		_content_width = ratio * x;
		_content_height = x;

#ifdef DEBUG
		cerr << "veCoverage = " << _gv->veCoverage()
			 << " _content_width = " << _content_width << " _content_height = " << _content_height << endl;
#endif // DEBUG
		
		// initialize canvas
		_gv->setSceneRect( -( _content_width + LEFTRIGHT_MARGIN ) / 2.0, -( _content_height + TOPBOTTOM_MARGIN ) / 2.0,
		                   _content_width + LEFTRIGHT_MARGIN, _content_height + TOPBOTTOM_MARGIN );
		// initialize border
		_levelBorderPtr->init( &_content_width, &_content_height,
		                       &_gv->veCoverage(), _pathwayPtr->skeletonG() );
		
		// initialize componentRegion
		_levelDetailPtr->init( &_content_width, &_content_height,
		                       &_gv->veCoverage() );

#ifdef DEBUG
		//cerr << "width x height = " << width() * height() << endl;
		//cerr << "label sum = " << sum << endl;
		//cerr << "ratio = " << width() * height() / sum << endl;
		cerr << "new_content_width = " << _content_width
			 << " new_content_height = " << _content_height << endl;
#endif // DEBUG
		
		// ui
		_gv->isCompositeFlag() = true;
		
		_simulateKey( Qt::Key_E );
		redrawAllScene();
		break;
	}
	case Qt::Key_F: {
		
		Base::Timer< chrono::milliseconds > timer( "ms" );
		timer.begin();
		_simulateKey( Qt::Key_V );
		_simulateKey( Qt::Key_1 );
		_simulateKey( Qt::Key_2 );
		_simulateKey( Qt::Key_Q );
		_simulateKey( Qt::Key_W );
		_simulateKey( Qt::Key_A );
		_simulateKey( Qt::Key_S );
		_simulateKey( Qt::Key_Z );
		_simulateKey( Qt::Key_X );
		_simulateKey( Qt::Key_R );
		
		timer.end();
		timer.elapsed();
		break;
	}
	case Qt::Key_C: {
		spaceCoverage();
		break;
	}
	case Qt::Key_E: {
		redrawAllScene();
		// _gv->exportPNG( -width()/2.0, -height()/2.0, width(), height() );
		_gv->exportPNG( -( _content_width + LEFTRIGHT_MARGIN ) / 2.0, -( _content_height + TOPBOTTOM_MARGIN ) / 2.0,
		                _content_width + LEFTRIGHT_MARGIN, _content_height + TOPBOTTOM_MARGIN );
		//_gv->exportSVG( -( _content_width + LEFTRIGHT_MARGIN ) / 2.0, -( _content_height + TOPBOTTOM_MARGIN ) / 2.0,
		//                _content_width + LEFTRIGHT_MARGIN, _content_height + TOPBOTTOM_MARGIN );
		break;
	}
	case Qt::Key_Minus: {
		_gv->isSimplifiedFlag() = false;
		_gv->isSkeletonFlag() = false;
		_gv->isCompositeFlag() = false;
		_gv->isPolygonFlag() = false;
		_gv->isPolygonComplexFlag() = false;
		_gv->isBoundaryFlag() = false;
		_gv->isCellFlag() = false;
		_gv->isCellPolygonFlag() = false;
		_gv->isCellPolygonComplexFlag() = false;
		//_gv->isMCLPolygonFlag() = false;
		_gv->isRoadFlag() = true;
		_gv->isLaneFlag() = true;
		_gv->isSubPathwayFlag() = true;
		_gv->isPathwayPolygonFlag() = true;
		cerr << "_gv->isSimplifiedFlag() = " << _gv->isSimplifiedFlag() << endl;
		cerr << "_gv->isSkeletonFlag() = " << _gv->isSkeletonFlag() << endl;
		cerr << "_gv->isCompositeFlag() = " << _gv->isCompositeFlag() << endl;
		cerr << "_gv->isPolygonFlag() = " << _gv->isPolygonFlag() << endl;
		cerr << "_gv->isPolygonComplexFlag() = " << _gv->isPolygonComplexFlag() << endl;
		cerr << "_gv->isBoundaryFlag() = " << _gv->isBoundaryFlag() << endl;
		cerr << "_gv->isCellFlag() = " << _gv->isCellFlag() << endl;
		cerr << "_gv->isCellPolygonFlag() = " << _gv->isCellPolygonFlag() << endl;
		cerr << "_gv->isCellPolygonComplexFlag() = " << _gv->isCellPolygonComplexFlag() << endl;
		//cerr << "_gv->isMCLPolygonFlag() = " << _gv->isMCLPolygonFlag() << endl;
		cerr << "_gv->isRoadFlag() = " << _gv->isRoadFlag() << endl;
		cerr << "_gv->isLaneFlag() = " << _gv->isLaneFlag() << endl;
		cerr << "_gv->isSubPathwayFlag() = " << _gv->isSubPathwayFlag() << endl;
		cerr << "_gv->isPathwayPolygonFlag() = " << _gv->isPathwayPolygonFlag() << endl;
		redrawAllScene();
	}
		break;
	case Qt::Key_Plus: {
		_gv->isSimplifiedFlag() = false;
		_gv->isSkeletonFlag() = false;
		_gv->isCompositeFlag() = false;
		_gv->isPolygonFlag() = false;
		_gv->isPolygonComplexFlag() = false;
		_gv->isBoundaryFlag() = true;
		_gv->isCellFlag() = false;
		_gv->isCellPolygonFlag() = false;
		_gv->isCellPolygonComplexFlag() = false;
		//_gv->isMCLPolygonFlag() = false;
		_gv->isRoadFlag() = true;
		_gv->isLaneFlag() = true;
		_gv->isSubPathwayFlag() = true;
		_gv->isPathwayPolygonFlag() = true;
		cerr << "_gv->isSimplifiedFlag() = " << _gv->isSimplifiedFlag() << endl;
		cerr << "_gv->isSkeletonFlag() = " << _gv->isSkeletonFlag() << endl;
		cerr << "_gv->isCompositeFlag() = " << _gv->isCompositeFlag() << endl;
		cerr << "_gv->isPolygonFlag() = " << _gv->isPolygonFlag() << endl;
		cerr << "_gv->isPolygonComplexFlag() = " << _gv->isPolygonComplexFlag() << endl;
		cerr << "_gv->isBoundaryFlag() = " << _gv->isBoundaryFlag() << endl;
		cerr << "_gv->isCellFlag() = " << _gv->isCellFlag() << endl;
		cerr << "_gv->isCellPolygonFlag() = " << _gv->isCellPolygonFlag() << endl;
		cerr << "_gv->isCellPolygonComplexFlag() = " << _gv->isCellPolygonComplexFlag() << endl;
		//cerr << "_gv->isMCLPolygonFlag() = " << _gv->isMCLPolygonFlag() << endl;
		cerr << "_gv->isRoadFlag() = " << _gv->isRoadFlag() << endl;
		cerr << "_gv->isLaneFlag() = " << _gv->isLaneFlag() << endl;
		cerr << "_gv->isSubPathwayFlag() = " << _gv->isSubPathwayFlag() << endl;
		cerr << "_gv->isPathwayPolygonFlag() = " << _gv->isPathwayPolygonFlag() << endl;
		redrawAllScene();
	}
		break;
	case Qt::Key_Escape:
		close();
		break;
	default:
		QWidget::keyPressEvent( event );
	}
}
