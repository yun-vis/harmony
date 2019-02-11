#include "ui/Window.h"

//----------------------------------------------------------
// Window
//----------------------------------------------------------
Window::Window( QWidget *parent )
    : QMainWindow( parent )
{
    _gv = new GraphicsView( this );
    setCentralWidget( _gv );

    setGeometry( 0, 0, _gv->width(), _gv->height() );

    createActions();
    createMenus();

    //_smoothPtr = NULL;
    //_octilinearPtr = NULL;

    _timerPtr = new vector< QBasicTimer	* >;
    //_timerPtr->resize( 1 );
    //(*_timerPtr)[0] = new QBasicTimer();

#ifdef RECORD_VIDEO
    _timerVideoStart();
#endif // RECORD_VIDEO
}

Window::~Window()
{
}

Window::Window( const Window & obj )
{
    _timerPtr = obj._timerPtr;
    _gv = obj._gv;
    _boundaryPtr = obj._boundaryPtr;
    _simplifiedBoundaryPtr = obj._simplifiedBoundaryPtr;

    // cells of subgraphs
    _cellPtr = obj._cellPtr;
    _roadPtr = obj._roadPtr;
    _lanePtr = obj._lanePtr;

    _smoothPtr = obj._smoothPtr;
    _octilinearPtr = obj._octilinearPtr;

    // display
    _content_width = obj._content_width;
    _content_height = obj._content_height;
    _timerType = obj._timerType;
}

void Window::_init( void )
{
    _content_width = width() - LEFTRIGHT_MARGIN;
    _content_height = height() - TOPBOTTOM_MARGIN;

    // initialization of boundary 
    _boundaryPtr = new Boundary;
    _simplifiedBoundaryPtr = new Boundary;
    _smoothPtr = new Smooth;
    _octilinearPtr = new Octilinear;

    // initialization of region data
    _cellPtr = new Cell;
    _roadPtr = new Road;
    _lanePtr = new vector< Road >;

    _cellPtr->setPathwayData( _pathway );
    _roadPtr->setPathwayData( _pathway );

    _gv->setPathwayData( _pathway );
    _gv->setRegionData( _boundaryPtr, _simplifiedBoundaryPtr,
                        _cellPtr, _roadPtr, _lanePtr );
}

void Window::createActions( void )
{
    // simplification
    selCloneGraphAct = new QAction( tr("C&loneGraph"), this );
    connect( selCloneGraphAct, SIGNAL( triggered() ), this, SLOT( selectCloneGraph() ) );
    selMinDistanceAct = new QAction( tr("M&inDistance"), this );
    connect( selMinDistanceAct, SIGNAL( triggered() ), this, SLOT( selectMinDistance() ) );
    selMovebackSmoothAct = new QAction( tr("M&ovebackSmooth"), this );
    connect( selMovebackSmoothAct, SIGNAL( triggered() ), this, SLOT( selectMovebackSmooth() ) );
    selMovebackOctilinearAct = new QAction( tr("M&ovebackOctilinear"), this );
    connect( selMovebackOctilinearAct, SIGNAL( triggered() ), this, SLOT( selectMovebackOctilinear() ) );

    // optimization
    // least square
    selSmoothLSAct = new QAction( tr("S&mooth Original (LS)"), this );
    connect( selSmoothLSAct, SIGNAL( triggered() ), this, SLOT( selectSmoothLS() ) );
    selOctilinearLSAct = new QAction( tr("O&ctilinear Original (LS)"), this );
    connect( selOctilinearLSAct, SIGNAL( triggered() ), this, SLOT( selectOctilinearLS() ) );

    // conjugate gradient
    selSmoothSmallCGAct = new QAction( tr("S&mooth Small (CG)"), this );
    connect( selSmoothSmallCGAct, SIGNAL( triggered() ), this, SLOT( selectSmoothSmallCG() ) );
    selSmoothCGAct = new QAction( tr("S&mooth Original (CG)"), this );
    connect( selSmoothCGAct, SIGNAL( triggered() ), this, SLOT( selectSmoothCG() ) );
    selOctilinearSmallCGAct = new QAction( tr("O&ctilinear Small (CG)"), this );
    connect( selOctilinearSmallCGAct, SIGNAL( triggered() ), this, SLOT( selectOctilinearSmallCG() ) );
    selOctilinearCGAct = new QAction( tr("O&ctilinear Original (CG)"), this );
    connect( selOctilinearCGAct, SIGNAL( triggered() ), this, SLOT( selectOctilinearCG() ) );
}

void Window::createMenus( void )
{
    // load
    // loadMenu = menuBar()->addMenu( tr("&Load") );
    // loadMenu->addAction( selDataAct );

    // simplification
    simMenu = menuBar()->addMenu( tr("&Simplification") );
    simMenu->addAction( selCloneGraphAct );
    simMenu->addAction( selMinDistanceAct );
    simMenu->addAction( selMovebackSmoothAct );
    simMenu->addAction( selMovebackOctilinearAct );

    // optimization
    optMenu = menuBar()->addMenu( tr("&Optimization") );
    optMenu->addAction( selSmoothLSAct );
    optMenu->addAction( selOctilinearLSAct );
    optMenu->addAction( selSmoothSmallCGAct );
    optMenu->addAction( selSmoothCGAct );
    optMenu->addAction( selOctilinearSmallCGAct );
    optMenu->addAction( selOctilinearCGAct );
}

void Window::simulateKey( Qt::Key key )
{
    QKeyEvent eventP( QEvent::KeyPress, key, Qt::NoModifier );
    QApplication::sendEvent( this, &eventP );
    QKeyEvent eventR( QEvent::KeyRelease, key, Qt::NoModifier );
    QApplication::sendEvent( this, &eventR );
}

void Window::postLoad( void )
{
    _boundaryPtr->adjustsize( _content_width/2, _content_height/2 );
    _boundaryPtr->clearConflicts();

    redrawAllScene();
}

void Window::redrawAllScene( void )
{
    _gv->initSceneItems();
    _gv->update();
    _gv->scene()->update();
    update();
    repaint();

    QCoreApplication::processEvents();
}

void Window::selectForce( void )
{
    simulateKey( Qt::Key_F );
    redrawAllScene();
}

void Window::selectCloneGraph( void )
{
    _simplifiedBoundaryPtr->cloneLayout( *_boundaryPtr );
    _simplifiedBoundaryPtr->clearConflicts();

    redrawAllScene();
}

void Window::selectMinDistance( void )
{
    _simplifiedBoundaryPtr->simplifyLayout();
    _gv->isSimplifiedFlag() = true;
    redrawAllScene();
}

void Window::selectMovebackSmooth( void )
{
    bool isFinished = true;
    isFinished = _simplifiedBoundaryPtr->movebackNodes( *_boundaryPtr, TYPE_SMOOTH );
    // if( isFinished == true ) cerr << "All stations are moved back!!" << endl;
    redrawAllScene();
}

void Window::selectMovebackOctilinear( void )
{
    bool isFinished = true;
    isFinished = _simplifiedBoundaryPtr->movebackNodes( *_boundaryPtr, TYPE_OCTILINEAR );
    // if( isFinished == true ) cerr << "All stations are moved back!!" << endl;
    redrawAllScene();
}

void Window::selectSmoothSmallCG( void )
{
    // run coarse smooth optimization
    clock_t start_time = clock();
    double err = 0.0;
    unsigned int nLabels = _simplifiedBoundaryPtr->nLabels();
    _smoothPtr->prepare( _simplifiedBoundaryPtr, _content_width/2, _content_height/2 );
    err = _smoothPtr->ConjugateGradient( 3 * _simplifiedBoundaryPtr->nVertices() );
    _smoothPtr->retrieve();

    cerr << "simNStation = " << _simplifiedBoundaryPtr->nVertices() << endl;
    cerr << "nStation = " << _boundaryPtr->nVertices() << endl;
    // ofs << "    Coarse CG: " << clock() - start_time << " err = " << err << " iter = " << 2 * _simplifiedBoundaryPtr->nStations() << endl;

    // run smooth optimization
    while( true ) {

        clock_t start = clock();
        int iter = 0;

        // check if all nodes are moved back
#ifdef  DEBUG
        cerr << " num_vertices( _simplifiedBoundaryPtr->boundary() ) = " << num_vertices( _simplifiedBoundaryPtr->boundary() ) << endl
             << " num_vertices( _boundaryPtr->boundary() ) = " << num_vertices( _boundaryPtr->boundary() ) << endl
             << " nLabels = " << nLabels << endl;
#endif  // DEBUG
        if( num_vertices( _simplifiedBoundaryPtr->boundary() ) == ( num_vertices( _boundaryPtr->boundary() ) + nLabels ) ) {
            break;
        }
        else {
            for( int i = 0; i < REMOVEBACKNUM; i++ ){
                selectMovebackSmooth();
            }
        }

        _smoothPtr->prepare( _simplifiedBoundaryPtr, _content_width/2, _content_height/2 );
        if( num_vertices( _simplifiedBoundaryPtr->boundary() ) == ( num_vertices( _boundaryPtr->boundary() ) + nLabels ) ) {
            iter = MAX2( 2 * ( _boundaryPtr->nVertices() + nLabels - _simplifiedBoundaryPtr->nVertices() ), 30 );
        }
        else{
            iter = MAX2( 2 * ( _boundaryPtr->nVertices() + nLabels - _simplifiedBoundaryPtr->nVertices() ), 30 );
        }
        err = _smoothPtr->ConjugateGradient( iter );
        _smoothPtr->retrieve();

        cerr << "    time each loop = " << clock() - start << " err = " << err << " iter = " << iter << endl;
        // ofs << "    time each loop = " << clock() - start << " err = " << err << " iter = " << iter << endl;
    }
    //_simplifiedBoundaryPtr->adjustsize( width(), height() );
    _smoothPtr->clear();
    _boundaryPtr->cloneSmooth( *_simplifiedBoundaryPtr );

    // cerr << "Total Time CG = " << clock() - start_time << endl;
    // ofs << "Total Time CG = " << clock() - start_time << endl;

    redrawAllScene();
}

void Window::selectSmooth( OPTTYPE opttype )
{
    // run smooth optimization
    _smoothPtr->prepare( _boundaryPtr, _content_width/2, _content_height/2 );
    switch( opttype ){
        case LEAST_SQUARE:
        {
            int iter = _boundaryPtr->nVertices();
            _smoothPtr->LeastSquare( iter );
        }
            break;
        case CONJUGATE_GRADIENT:
        {
            int iter = _boundaryPtr->nVertices();
            _smoothPtr->ConjugateGradient( iter );
        }
            break;
        default:
            break;
    }
    _smoothPtr->retrieve();
    _smoothPtr->clear();

    redrawAllScene();
}

void Window::selectSmoothLS( void )
{
    selectSmooth( LEAST_SQUARE );
    redrawAllScene();
}

void Window::selectSmoothCG( void )
{
    selectSmooth( CONJUGATE_GRADIENT );
    redrawAllScene();
}

void Window::selectOctilinearSmallCG( void )
{
    // run coarse octilinear optimization
    double err = 0.0;
    unsigned int nLabels = _simplifiedBoundaryPtr->nLabels();

    _octilinearPtr->prepare( _simplifiedBoundaryPtr, _content_width/2, _content_height/2 );
    err = _octilinearPtr->ConjugateGradient( 5 * _simplifiedBoundaryPtr->nVertices() );
    _octilinearPtr->retrieve();
    //ofs << "    Coarse CG: " << clock() - start_time << " err = " << err << " iter = " << 5 * _simplifiedBoundaryPtr->nVertices() << endl;

    // run octilinear optimization
    while( true ) {

        clock_t start = clock();
        int iter = 0;

        // check if all nodes are moved back
        if( num_vertices( _simplifiedBoundaryPtr->boundary() ) == ( num_vertices( _boundaryPtr->boundary() ) + nLabels ) ) {
            break;
        }
        else {
            for( int i = 0; i < REMOVEBACKNUM; i++ ){
                selectMovebackOctilinear();
            }
        }

        _octilinearPtr->prepare( _simplifiedBoundaryPtr, _content_width/2, _content_height/2 );
        if( num_vertices( _simplifiedBoundaryPtr->boundary() ) == ( num_vertices( _boundaryPtr->boundary() ) + nLabels ) ) {
            iter = MAX2( 2 * ( _boundaryPtr->nVertices() + nLabels - _simplifiedBoundaryPtr->nVertices() ), 30 );
        }
        else{
            iter = MAX2( 2 * ( _boundaryPtr->nVertices() + nLabels - _simplifiedBoundaryPtr->nVertices() ), 30 );
        }
        err = _octilinearPtr->ConjugateGradient( iter );
        _octilinearPtr->retrieve();

        // ofs << "    time each loop = " << clock() - start << " err = " << err << " iter = " << iter << endl;
    }

    _octilinearPtr->clear();
    _boundaryPtr->cloneOctilinear( *_simplifiedBoundaryPtr );

    redrawAllScene();
}

void Window::selectOctilinear( OPTTYPE opttype )
{
    // run octilinear optimization
    _octilinearPtr->prepare( _boundaryPtr, _content_width/2, _content_height/2 );

    switch( opttype ) {
        case LEAST_SQUARE:
        {
            int iter = _boundaryPtr->nVertices();
            _octilinearPtr->LeastSquare( iter );
        }
            break;
        case CONJUGATE_GRADIENT:
        {
            int iter = _boundaryPtr->nVertices();
            _octilinearPtr->ConjugateGradient( iter );
        }
            break;
        default:
            break;
    }
    _octilinearPtr->retrieve();
    _octilinearPtr->clear();

    redrawAllScene();
}

void Window::selectOctilinearLS( void )
{
    selectOctilinear( LEAST_SQUARE );
    redrawAllScene();
}

void Window::selectOctilinearCG( void )
{
    selectOctilinear( CONJUGATE_GRADIENT );
    redrawAllScene();
}

void Window::selectBuildBoundary( void )
{
    _boundaryPtr->buildBoundaryGraph();
    _gv->isSimplifiedFlag() = false;
    redrawAllScene();
}

#ifdef RECORD_VIDEO
void Window::_timerVideoStart( void )
{
    _timerVideo.start( 1000, this );
    _timerVideoID = _timerVideo.timerId();
    cerr << "_timerVideo.timerId = " << _timerVideo.timerId() << endl;
}

void Window::_timerVideoStop( void )
{
    _timerVideo.stop();
}

void Window::timerVideo( void )
{
    simulateKey( Qt::Key_E );
}
#endif // RECORD_VIDEO

void Window::_timerBoundaryStart( void )
{
    _timerType = TIMER_BOUNDARY;

    Polygon2 contour;
    contour.elements().push_back( Coord2( - 0.5*_content_width, - 0.5*_content_height ) );
    contour.elements().push_back( Coord2( + 0.5*_content_width, - 0.5*_content_height ) );
    contour.elements().push_back( Coord2( + 0.5*_content_width, + 0.5*_content_height ) );
    contour.elements().push_back( Coord2( - 0.5*_content_width, + 0.5*_content_height ) );

    _boundaryPtr->forceBoundary().init( &_boundaryPtr->composite(), contour, "../configs/boundary.conf" );
#ifdef SKIP
    void *ptr = &_boundaryPtr->boundary();
            ForceGraph *fgPtr = (ForceGraph*) (ptr);
            _forceBoundary.init( fgPtr, contour );
            printGraph( *fgPtr );
#endif // SKIP

    _gv->isPolygonFlag() = true;

    _timerPtr->resize(1);
    (*_timerPtr)[0] = new QBasicTimer();
    (*_timerPtr)[0]->start( 30, this );
    cerr << "Boundary:_timerStart: " << " (*_timerPtr)[i].id = " << (*_timerPtr)[0]->timerId() << endl;
}

void Window::_timerStop( void )
{
    for( unsigned int i = 0; i < _timerPtr->size(); i++ ) {
        cerr << "_timerStop: " << " (*_timerPtr)[i]->timerId() = " << (*_timerPtr)[i]->timerId() << endl;
        (*_timerPtr)[i]->stop();
        // delete (*_timerPtr)[i];
    }
    _timerPtr->clear();
}

void Window::_timerBoundaryStop( void )
{
    _timerStop();

    _boundaryPtr->createPolygonComplex();
    simulateKey( Qt::Key_B );
    _boundaryPtr->writePolygonComplex();

    _gv->isPolygonFlag() = true;
    _gv->isPolygonComplexFlag() = false;
    _gv->isBoundaryFlag() = false;
    _gv->isCompositeFlag() = true;
}

void Window::timerBoundary( void )
{
    double err = 0.0;

    switch ( _boundaryPtr->forceBoundary().mode() ) {

        case TYPE_FORCE:
        case TYPE_BARNES_HUT:
        {
            _boundaryPtr->forceBoundary().force();
            err = _boundaryPtr->forceBoundary().verletIntegreation();
            cerr << "err (force) = " << err << endl;
            if ( err < _boundaryPtr->forceBoundary().finalEpsilon() ) {
                _timerBoundaryStop();
                cerr << "[Force-Directed] Finished Execution Time = " << checkOutETime() << endl;
                cerr << "[Force-Directed] Finished CPU Time = " << checkOutCPUTime() << endl;
                //cerr << "FINAL widget_count" << widget_count << endl;
            }
            break;
        }
        case TYPE_CENTROID:
        {
            _boundaryPtr->forceBoundary().centroidGeometry();
            err = _boundaryPtr->forceBoundary().gap();
            cerr << "err (centroid) = " << err << endl;
            if ( err < _boundaryPtr->forceBoundary().finalEpsilon() ) {
                _timerBoundaryStop();
                cerr << "[Centroidal] Finished Execution Time = " << checkOutETime() << endl;
                cerr << "[Centroidal] Finished CPU Time = " << checkOutCPUTime() << endl;
            }
            break;
        }
        case TYPE_HYBRID:
        {
            _boundaryPtr->forceBoundary().force();
            _boundaryPtr->forceBoundary().centroidGeometry();
            err = _boundaryPtr->forceBoundary().verletIntegreation();
            cerr << "err (hybrid) = " << err << endl;
            if ( err < _boundaryPtr->forceBoundary().finalEpsilon() ) {
                _timerBoundaryStop();
                cerr << "[Hybrid] Finished Execution Time = " << checkOutETime() << endl;
                cerr << "[Hybrid] Finished CPU Time = " << checkOutCPUTime() << endl;
            }
            break;
        }
        default:
            break;
    }
}

void Window::_timerPathwayCellStart( void )
{
    _timerType = TIMER_CELL;
    _timerPtr->resize( _cellPtr->forceCellVec().size() );

    cerr << "_cellPtr->forceCellVec().size() = " << _cellPtr->forceCellVec().size() << endl;
    for( unsigned int i = 0; i < _cellPtr->forceCellVec().size(); i++ ) {
        (*_timerPtr)[i] = new QBasicTimer();
        (*_timerPtr)[i]->start( 30, this );
        cerr << "Cell:_timerStart: " << " (*_timerPtr)[i].id = " << (*_timerPtr)[i]->timerId() << endl;
    }

    _gv->isCellPolygonFlag() = true;
    _gv->isCellFlag() = true;
}

void Window::_timerPathwayCellStop( void )
{
    bool isActive = false;
    for( unsigned int i = 0; i < _timerPtr->size(); i++ ){
        isActive = isActive || (*_timerPtr)[i]->isActive();
    }
    if( !isActive ) {

        _timerStop();
        _gv->isCellPolygonFlag() = false;
        _gv->isCellFlag() = false;
        _gv->isCellPolygonComplexFlag() = true;

        simulateKey( Qt::Key_P );
    }
}

void Window::timerPathwayCell( void )
{
    double err = 0.0;

    assert( _timerPtr->size() == _cellPtr->forceCellVec().size() );

    for( unsigned int i = 0; i < _cellPtr->forceCellVec().size(); i++ ){

        // cerr << "i = " << i << " active = " << (*_timerPtr)[i]->isActive() << endl;
        if( (*_timerPtr)[i]->isActive() == true ){
            switch ( _cellPtr->forceCellVec()[i].mode() ) {
                case TYPE_FORCE:
                case TYPE_BARNES_HUT:
                {
                    _cellPtr->forceCellVec()[i].force();
                    _cellPtr->additionalForces();
                    err = _cellPtr->forceCellVec()[i].verletIntegreation();
                    cerr << "err (force) = " << err << endl;
                    if ( err < _cellPtr->forceCellVec()[i].finalEpsilon() ) {
                        (*_timerPtr)[i]->stop();
                        _timerPathwayCellStop();
                        cerr << "[Force-Directed] Finished Execution Time [" << i << "] = " << checkOutETime() << endl;
                        cerr << "[Force-Directed] Finished CPU Time [" << i << "] = " << checkOutCPUTime() << endl;
                        //cerr << "FINAL widget_count" << widget_count << endl;
                    }
                    break;
                }
                case TYPE_CENTROID:
                {
                    _cellPtr->forceCellVec()[i].centroidGeometry();
                    _cellPtr->additionalForces();
                    err = _cellPtr->forceCellVec()[i].gap();
                    cerr << "err (centroid) = " << err << endl;
                    if ( err < _cellPtr->forceCellVec()[i].finalEpsilon() ) {
                        (*_timerPtr)[i]->stop();
                        _timerPathwayCellStop();
                        cerr << "[Centroidal] Finished Execution Time [" << i << "] = " << checkOutETime() << endl;
                        cerr << "[Centroidal] Finished CPU Time [" << i << "] = " << checkOutCPUTime() << endl;
                    }
                    break;
                }
                case TYPE_HYBRID:
                {
                    _cellPtr->forceCellVec()[i].force();
                    _cellPtr->additionalForces();
                    _cellPtr->forceCellVec()[i].centroidGeometry();
                    err = _cellPtr->forceCellVec()[i].verletIntegreation();
                    cerr << "err (hybrid) = " << err << endl;
                    if ( err < _cellPtr->forceCellVec()[i].finalEpsilon() ) {
                        (*_timerPtr)[i]->stop();
                        _timerPathwayCellStop();
                        cerr << "[Hybrid] Finished Execution Time [" << i << "] = " << checkOutETime() << endl;
                        cerr << "[Hybrid] Finished CPU Time [" << i << "] = " << checkOutCPUTime() << endl;
                    }
                    break;
                }
                default:
                    break;
            }
        }

    }
}

void Window::_timerMCLStart( void )
{
    _timerType = TIMER_BONE;

    vector< multimap< int, CellComponent > > & cellComponentVec = _cellPtr->cellComponentVec();

    for( unsigned int i = 0; i < cellComponentVec.size(); i++ ){

        multimap< int, CellComponent > &cellComponentMap = cellComponentVec[ i ];
        multimap< int, CellComponent >::iterator itC;

        for( itC = cellComponentMap.begin(); itC != cellComponentMap.end(); itC++ ){

            CellComponent &cell = itC->second;
            if( cell.nMCL > 1 ) {

                QBasicTimer *t = new QBasicTimer();
                _timerPtr->push_back( t );
                t->start( 30, this );
                cerr << "MCL:_timerStart: " << " (*_timerPtr)[i].id = " << t->timerId() << endl;
            }
        }
    }
}

void Window::_timerMCLStop( void )
{
    bool isActive = false;
    _timerType = TIMER_IDLE;

    for( unsigned int i = 0; i < _timerPtr->size(); i++ ){
        isActive = isActive || (*_timerPtr)[i]->isActive();
        cerr << "timerId = " << (*_timerPtr)[i]->timerId() << " act = " << (*_timerPtr)[i]->isActive() << endl;
    }

    if( !isActive ) {

        _timerStop();
        _gv->isSubPathwayFlag() = true;
        //_gv->isMCLFlag() = true;
    }
}

void Window::timerMCL( void )
{
    double err = 0.0;
    //assert( _timerPtr->size() == _cellPtr->forceCellVec().size() );

    vector< multimap< int, CellComponent > > & cellComponentVec = _cellPtr->cellComponentVec();

    unsigned int idT = 0;
    for( unsigned int i = 0; i < cellComponentVec.size(); i++ ){

        multimap< int, CellComponent > &cellComponentMap = cellComponentVec[ i ];
        multimap< int, CellComponent >::iterator itC;

        for( itC = cellComponentMap.begin(); itC != cellComponentMap.end(); itC++ ){

            CellComponent &cell = itC->second;

            if( cell.nMCL > 1 ){

                if( (*_timerPtr)[idT]->isActive() == true ) {

                    switch ( itC->second.mcl.mode() ) {
                        case TYPE_FORCE:
                        case TYPE_BARNES_HUT:
                        {
                            itC->second.mcl.force();
                            err = itC->second.mcl.verletIntegreation();
                            cerr << idT << ": err (mcl force) = " << err << endl;
                            if (err < itC->second.mcl.finalEpsilon()) {
                                (*_timerPtr)[idT]->stop();
                                //_timerMCLStop();
                                cerr << "[Force-Directed] Finished Execution Time [" << idT << "] = " << checkOutETime() << endl;
                                cerr << "[Force-Directed] Finished CPU Time [" << idT << "] = " << checkOutCPUTime() << endl;
                            }
                            break;
                        }
                        case TYPE_CENTROID:
                        {
                            itC->second.mcl.centroidGeometry();
                            err = itC->second.mcl.gap();
                            cerr << idT << ": err (mcl force) = " << err << endl;
                            if (err < itC->second.mcl.finalEpsilon()) {
                                (*_timerPtr)[idT]->stop();
                                //_timerMCLStop();
                                cerr << "[Force-Directed] Finished Execution Time [" << idT << "] = " << checkOutETime() << endl;
                                cerr << "[Force-Directed] Finished CPU Time [" << idT << "] = " << checkOutCPUTime() << endl;
                            }
                            break;
                        }
                        case TYPE_HYBRID:
                        {
                            itC->second.mcl.force();
                            itC->second.mcl.centroidGeometry();
                            err = itC->second.mcl.verletIntegreation();
                            cerr << idT << ": err (mcl force) = " << err << endl;
                            if (err < itC->second.mcl.finalEpsilon()) {
                                (*_timerPtr)[idT]->stop();
                                //_timerMCLStop();
                                cerr << "[Force-Directed] Finished Execution Time [" << idT << "] = " << checkOutETime() << endl;
                                cerr << "[Force-Directed] Finished CPU Time [" << idT << "] = " << checkOutCPUTime() << endl;
                            }
                            break;
                        }
                        default:
                            break;
                    }
                }

                idT++;
            }
        }
    }
}

void Window::_timerPathwayStart( void )
{
    _timerType = TIMER_PATHWAY;
    unsigned int nComponent = _cellPtr->nComponent();

    _timerPtr->resize( nComponent );
    for( unsigned int i = 0; i < nComponent; i++ ){
        (*_timerPtr)[i] = new QBasicTimer();
        (*_timerPtr)[i]->start( 30, this );
        cerr << "Pathway:_timerStart: " << " (*_timerPtr)[i].id = " << (*_timerPtr)[i]->timerId() << endl;
    }
}

void Window::_timerPathwayStop( void )
{
    bool isActive = false;

    for( unsigned int i = 0; i < _timerPtr->size(); i++ ){
        isActive = isActive || (*_timerPtr)[i]->isActive();
        cerr << "timerId = " << (*_timerPtr)[i]->timerId() << " act = " << (*_timerPtr)[i]->isActive() << endl;
    }

    if( !isActive ) {

        _timerStop();
        _roadPtr->initRoad( _cellPtr->cellComponentVec() );
        _roadPtr->buildRoad();
        // cerr << "road built..." << endl;
        _gv->isRoadFlag() = true;

        cerr << "**************" << endl;
        cerr << "Steriner tree" << endl;

        for( unsigned int i = 0; i < _roadPtr->highwayMat().size(); i++ ){
            for( unsigned int j = 0; j < _roadPtr->highwayMat()[i].size(); j++ ){

                map< MetaboliteGraph::vertex_descriptor,
                MetaboliteGraph::vertex_descriptor > &common = _roadPtr->highwayMat()[i][j].common;

                map< MetaboliteGraph::vertex_descriptor,
                MetaboliteGraph::vertex_descriptor >::iterator it;
#ifdef DEBUG
                for( it = common.begin(); it != common.end(); it++ ){
                    cerr << _pathway->subG()[i][ it->first ].id << endl;
                }
                cerr << endl;
#endif // DEBUG
            }
        }

        _lanePtr->clear();
        _lanePtr->resize( _pathway->nSubsys() );
        for( unsigned int i = 0; i < _lanePtr->size(); i++ ){
            vector < Highway > &highwayVec = _roadPtr->highwayMat()[i];
            (*_lanePtr)[i].setPathwayData( _pathway );
            (*_lanePtr)[i].initLane( i, _cellPtr->cellComponentVec()[i], &highwayVec );
            (*_lanePtr)[i].steinerTree();
        }
        _gv->isLaneFlag() = true;
    }
}

bool Window::_callTimerPathway( unsigned int id, unsigned int i, unsigned int j )
{
    bool isStopped = false;
    double err = 0.0;
    vector< multimap< int, CellComponent > > &cellComponentVec = _cellPtr->cellComponentVec();

    multimap< int, CellComponent >::iterator itC;
    multimap< int, CellComponent > &cellComponentMap = cellComponentVec[i];
    itC = cellComponentMap.begin();
    advance( itC, j );

    // cerr << "mode = " << itC->second.detail.mode() << endl;
    switch ( itC->second.detail.mode() ) {

        case TYPE_FORCE:
        case TYPE_BARNES_HUT:
        {
            itC->second.detail.force();
            err = itC->second.detail.verletIntegreation();
            cerr << id << ": err (pathway force) = " << err << endl;
            if (err < itC->second.detail.finalEpsilon()) {
                cerr << "stoping... " << (*_timerPtr)[id]->timerId() << endl;
                (*_timerPtr)[id]->stop();
                //_timerPathwayStop();
                cerr << "[Force-Directed] Finished Execution Time [" << id << "] = " << checkOutETime() << endl;
                cerr << "[Force-Directed] Finished CPU Time [" << id << "] = " << checkOutCPUTime() << endl;
            }
            break;
        }
        case TYPE_CENTROID:
        {
            itC->second.detail.centroidGeometry();
            err = itC->second.detail.gap();
            cerr << id << ": err (pathway force) = " << err << endl;
            if (err < itC->second.detail.finalEpsilon()) {
                cerr << "stoping... " << (*_timerPtr)[id]->timerId() << endl;
                (*_timerPtr)[id]->stop();
                //_timerPathwayStop();
                cerr << "[Force-Directed] Finished Execution Time [" << id << "] = " << checkOutETime() << endl;
                cerr << "[Force-Directed] Finished CPU Time [" << id << "] = " << checkOutCPUTime() << endl;
            }
            break;
        }
        case TYPE_HYBRID:
        {
            itC->second.detail.force();
            itC->second.detail.centroidGeometry();
            err = itC->second.detail.verletIntegreation();
            cerr << id << ": err (pathway force) = " << err << endl;
            if (err < itC->second.detail.finalEpsilon()) {
                cerr << "stoping... " << (*_timerPtr)[id]->timerId() << endl;
                (*_timerPtr)[id]->stop();
                //_timerPathwayStop();
                cerr << "[Force-Directed] Finished Execution Time [" << id << "] = " << checkOutETime() << endl;
                cerr << "[Force-Directed] Finished CPU Time [" << id << "] = " << checkOutCPUTime() << endl;
            }
            break;
        }
        default:
            break;
    }

    return isStopped;
}

void Window::timerPathway( void )
{
    vector< multimap< int, CellComponent > > &cellComponentVec = _cellPtr->cellComponentVec();

    if( _timerPtr->size() != _cellPtr->nComponent() ){

        cerr << "timerSize = " << _timerPtr->size() << " nCompo = " << _cellPtr->nComponent() << endl;
        return;
        // assert( _timerPtr->size() == _cellPtr->nComponent() );
    }

    vector< vector< unsigned int > > idMat;
    idMat.resize( cellComponentVec.size() );
    unsigned int idD = 0;
    for( unsigned int i = 0; i < idMat.size(); i++ ){

        multimap< int, CellComponent > &cellComponentMap = cellComponentVec[i];
        idMat[i].resize( cellComponentMap.size() );
        for( unsigned int j = 0; j < idMat[i].size(); j++ ){

            idMat[i][j] = idD;
            idD++;
        }
    }
    // cerr << "idD = " << idD << endl;

    // vector< std::thread * > threads;
    vector< std::thread * > threads;
    for( unsigned int i = 0; i < cellComponentVec.size(); i++ ){

        const multimap< int, CellComponent > &cellComponentMap = cellComponentVec[i];
        // multimap< int, CellComponent >::iterator itC = cellComponentMap.begin();

        for( unsigned int j = 0; j < cellComponentMap.size(); j++ ){

            //_callTimerPathway( idMat[i][j], i, j );
            // cerr << "id = " << idMat[i][j] << endl;
/*
            // QBasicTimer	*timer = (*_timerPtr)[idMat[i][j]];
            if( (*_timerPtr)[idMat[i][j]]->isActive() ){

                cerr << endl << endl << "enable a thread... i = " << i << " j = " << j << endl;
                Controller * thPtr = new Controller;
                threads.push_back( thPtr );

                QString test;
                Q_EMIT thPtr->operate( test );
            }
*/

//            QBasicTimer	*timer = (*_timerPtr)[idMat[i][j]];
            if( (*_timerPtr)[idMat[i][j]]->isActive() ){
                std::thread * th = new std::thread( &Window::_callTimerPathway, this, idMat[i][j], i, j );
                threads.push_back( th );
            }

        }
    }

    // check if all threads are finished
    for( unsigned int i = 0; i < threads.size(); i++ ) {
        threads[ i ]->join();
        delete threads[ i ];
    }

}

void Window::listenProcessDetailedPathway( void )
{
    bool allFinished = true;

    // check if all threads are finished
    for( unsigned int i = 0; i < controllers.size(); i++ ) {
        allFinished = allFinished && controllers[ i ]->wt().isFinished();
        // cerr << "is controllers[" << i << "] finished ? "<< controllers[ i ]->wt().isFinished();
    }

    if( allFinished == true ){
        simulateKey( Qt::Key_X );
    }
}

void Window::processDetailedPathway( void )
{
    vector< multimap< int, CellComponent > > &cellComponentVec = _cellPtr->cellComponentVec();

    vector< vector< unsigned int > > idMat;
    idMat.resize( cellComponentVec.size() );
    unsigned int idD = 0;
    for( unsigned int i = 0; i < idMat.size(); i++ ){

        multimap< int, CellComponent > &cellComponentMap = cellComponentVec[i];
        idMat[i].resize( cellComponentMap.size() );
        for( unsigned int j = 0; j < idMat[i].size(); j++ ){

            idMat[i][j] = idD;
            idD++;
        }
    }
    // cerr << "idD = " << idD << endl;
    cerr << "From main thread: " << QThread::currentThreadId() << endl;
    for( unsigned int i = 0; i < cellComponentVec.size(); i++ ){

        const multimap< int, CellComponent > &cellComponentMap = cellComponentVec[i];
        // multimap< int, CellComponent >::iterator itC = cellComponentMap.begin();

        for( unsigned int j = 0; j < cellComponentMap.size(); j++ ){

            cerr << endl << endl << "enable a thread... i = " << i << " j = " << j << endl;
            Controller * conPtr = new Controller;
            conPtr->setPathwayData( _pathway );
            conPtr->setRegionData( _boundaryPtr, _simplifiedBoundaryPtr,
                                   _cellPtr, _roadPtr, _lanePtr );
            conPtr->init( i, j );
            controllers.push_back( conPtr );

            connect( conPtr, &Controller::update, this, &Window::redrawAllScene );
            connect( &conPtr->wt(), &QThread::finished, this, &Window::listenProcessDetailedPathway );

            QString test;
            Q_EMIT conPtr->operate( test );
        }
    }
}

void Window::stopProcessDetailedPathway( void )
{
    // quit all threads
    for( unsigned int i = 0; i < controllers.size(); i++ ) {
        controllers[ i ]->wt().quit();
        delete controllers[ i ];
    }
    controllers.clear();

    // compute steiner tree
    _roadPtr->initRoad( _cellPtr->cellComponentVec() );
    _roadPtr->buildRoad();
    // cerr << "road built..." << endl;
    _gv->isRoadFlag() = true;

    cerr << "/***************" << endl;
    cerr << "/* Steiner tree" << endl;
    cerr << "/***************" << endl;

    for( unsigned int i = 0; i < _roadPtr->highwayMat().size(); i++ ){
        for( unsigned int j = 0; j < _roadPtr->highwayMat()[i].size(); j++ ){

            map< MetaboliteGraph::vertex_descriptor,
                    MetaboliteGraph::vertex_descriptor > &common = _roadPtr->highwayMat()[i][j].common;

            map< MetaboliteGraph::vertex_descriptor,
                    MetaboliteGraph::vertex_descriptor >::iterator it;
#ifdef DEBUG
            for( it = common.begin(); it != common.end(); it++ ){
                    cerr << _pathway->subG()[i][ it->first ].id << endl;
                }
                cerr << endl;
#endif // DEBUG
        }
    }

    _lanePtr->clear();
    _lanePtr->resize( _pathway->nSubsys() );
    for( unsigned int i = 0; i < _lanePtr->size(); i++ ){
        vector < Highway > &highwayVec = _roadPtr->highwayMat()[i];
        (*_lanePtr)[i].setPathwayData( _pathway );
        (*_lanePtr)[i].initLane( i, _cellPtr->cellComponentVec()[i], &highwayVec );
        (*_lanePtr)[i].steinerTree();
    }
    _gv->isLaneFlag() = true;
}

void Window::timerEvent( QTimerEvent *event )
{
    // cerr << "timer event..." << endl;
    Q_UNUSED( event );

#ifdef RECORD_VIDEO
    if( event->timerId() == _timerVideoID ){
        // cerr << "event->timerId() = " << event->timerId() << endl;
        timerVideo();
    }
    else{
#endif // RECORD_VIDEO
        // cerr << "timerType = " << _timerType << " event->timerId() = " << event->timerId() << endl;

        if( _timerType == TIMER_BOUNDARY ){
            timerBoundary();
        }
        else if( _timerType == TIMER_CELL ){
            timerPathwayCell();
        }
        else if( _timerType == TIMER_BONE ){
            timerMCL();
        }
        else if( _timerType == TIMER_PATHWAY ){
            timerPathway();
        }
        else if( _timerType == TIMER_IDLE ) {
            ;
        }
        else {
            cerr << "Something is wrong here... at" << __LINE__ << " in " << __FILE__ << endl;
        }
#ifdef RECORD_VIDEO
    }
#endif // RECORD_VIDEO
    //cerr << "tid = " << event->timerId() << endl;
    //cerr << "timerType = " << _timerType << " tid = " << event->timerId() << endl;
    redrawAllScene();
}


void Window::keyPressEvent( QKeyEvent *event )
{
    switch( event->key() ) {

        case Qt::Key_1:
        {
            checkInETime();
            cerr << "*********** Starting Execution Time = " << checkOutETime() << endl;
            checkInCPUTime();
            cerr << "*********** Starting CPU Time = " << checkOutCPUTime() << endl;
            _timerBoundaryStart();
            break;
        }
        case Qt::Key_2:
        {
            cerr << "stop timer event" << endl;
            _timerStop();
            _timerBoundaryStop();
            redrawAllScene();
            assert( _timerPtr->size() == 0 );
            break;
        }
        case Qt::Key_Q:
        {
            checkInETime();
            cerr << "*********** Starting Execution Time = " << checkOutETime() << endl;
            checkInCPUTime();
            cerr << "*********** Starting CPU Time = " << checkOutCPUTime() << endl;
            _timerPathwayCellStart();
            break;
        }
        case Qt::Key_W:
        {
            cerr << "stop timer event" << endl;
            _timerStop();
            //simulateKey( Qt::Key_P );
            _timerPathwayCellStop();
            assert( _timerPtr->size() == 0 );
            redrawAllScene();
            break;
        }
        case Qt::Key_A:
        {
            checkInETime();
            cerr << "*********** Starting Execution Time = " << checkOutETime() << endl;
            checkInCPUTime();
            cerr << "*********** Starting CPU Time = " << checkOutCPUTime() << endl;
            _timerMCLStart();
            _gv->isCellPolygonFlag() = false;
            _gv->isMCLPolygonFlag() = true;
            break;
        }
        case Qt::Key_S:
        {
            cerr << "stop timer event" << endl;
            _timerStop();
            _timerMCLStop();
            _cellPtr->updatePathwayCoords();
            //_gv->isMCLPolygonFlag() = false;
            redrawAllScene();
            break;
        }
        case Qt::Key_Z:
        {
            checkInETime();
            cerr << "*********** Starting Execution Time = " << checkOutETime() << endl;
            checkInCPUTime();
            cerr << "*********** Starting CPU Time = " << checkOutCPUTime() << endl;
            //_timerPathwayStart();
            processDetailedPathway();
            _gv->isPathwayPolygonFlag() = true;
            _gv->isMCLPolygonFlag() = false;
            break;
        }
        case Qt::Key_X:
        {
            cerr << "stopProcessDetailedPathway..." << endl;
            stopProcessDetailedPathway();
            cerr << "[Force-Directed] Finished Execution Time = " << checkOutETime() << endl;
            cerr << "[Force-Directed] Finished CPU Time = " << checkOutCPUTime() << endl;
            redrawAllScene();
            break;
        }
        case Qt::Key_3:
        {
            _gv->isSimplifiedFlag() = !_gv->isSimplifiedFlag();
            redrawAllScene();
            break;
        }
        case Qt::Key_4:
        {
            _gv->isSkeletonFlag() = !_gv->isSkeletonFlag();
            redrawAllScene();
            break;
        }
        case Qt::Key_5:
        {
            _gv->isCompositeFlag() = !_gv->isCompositeFlag();
            redrawAllScene();
            break;
        }
        case Qt::Key_6:
        {
            _gv->isPolygonFlag() = !_gv->isPolygonFlag();
            redrawAllScene();
            break;
        }
        case Qt::Key_7:
        {
            _gv->isPolygonComplexFlag() = !_gv->isPolygonComplexFlag();
            redrawAllScene();
            break;
        }
        case Qt::Key_8:
        {
            _gv->isBoundaryFlag() = !_gv->isBoundaryFlag();
            redrawAllScene();
            break;
        }
        case Qt::Key_9:
        {
            _gv->isCellPolygonFlag() = !_gv->isCellPolygonFlag();
            _gv->isCellFlag() = !_gv->isCellFlag();
            redrawAllScene();
            break;
        }
        case Qt::Key_0:
        {
            _gv->isSubPathwayFlag() = !_gv->isSubPathwayFlag();
            redrawAllScene();
            break;
        }
        case Qt::Key_L:
        {
            // load setting
            _pathway->initLayout( _boundaryPtr->polygonComplex() );

            // initialize cell
            _cellPtr->clear();
            _cellPtr->init( &_boundaryPtr->polygonComplex() );

            _gv->isCellFlag() = true;
            _gv->isCompositeFlag() = false;
            _gv->isPolygonFlag() = false;
            _gv->isPolygonComplexFlag() = true;
            _gv->isBoundaryFlag() = false;
            redrawAllScene();
            break;
        }
        case Qt::Key_P:
        {
            _cellPtr->createPolygonComplex();
            _cellPtr->updateMCLCoords();
            //_cellPtr->updatePathwayCoords();

            // _gv->isCellPolygonComplexFlag() = true;
            // _gv->isSubPathwayFlag() = true;
            redrawAllScene();
            break;
        }
        case Qt::Key_C:
        {
            selectCloneGraph();
            break;
        }
        case Qt::Key_M:
        {
            selectMinDistance();
            break;
        }
        case Qt::Key_R:
        {
            _boundaryPtr->readPolygonComplex();
            _gv->isPolygonComplexFlag() = true;
            _gv->isBoundaryFlag() = true;
            redrawAllScene();
            break;
        }
/*
        case Qt::Key_S:
        {
            selectCloneGraph();
            selectSmoothCG();
            //selectSmoothSmallCboundary();
            break;
        }
*/
        case Qt::Key_O:
        {
            selectCloneGraph();
            selectOctilinearCG();
            _boundaryPtr->updatePolygonComplex();
            //selectOctilinearSmallCboundary();
            _gv->isPolygonFlag() = false;
            _gv->isCompositeFlag() = false;
            _gv->isPolygonComplexFlag() = true;
            _gv->isBoundaryFlag() = true;
            redrawAllScene();
            break;
        }
        case Qt::Key_B:
        {
            selectBuildBoundary();
            break;
        }
        case Qt::Key_V:
        {
            //_pathway->init( "../xml/tiny/", "../xml/tmp/",
            _pathway->init( "../xml/small/", "../xml/tmp/",
            //_pathway->init( "../xml/A/", "../xml/tmp/",
            //_pathway->init( "../xml/full/", "../xml/tmp/",
                            "../xml/frequency/metabolite_frequency.txt", "../xml/type/typelist.txt" );
            //_pathway->loadDot( "../dot/TradeLand.dot" );

            _pathway->generate();
            _pathway->exportEdges();

            _boundaryPtr->init( _pathway->skeletonG() );
            //_boundaryPtr->buildSkeleton();

            // update content width and height
#ifdef DEBUG
            cerr << "_content_width = " << _content_width
                 << " _content_height = " << _content_height << endl;
#endif // DEBUG
            double sum = 0.0;
            MetaboliteGraph             &g         = _pathway->g();
            BGL_FORALL_VERTICES( vd, g, MetaboliteGraph ) {
                QFontMetrics metrics( QFont( "Arial", _gv->fontSize(), QFont::Bold, false ) );
                double sx = metrics.width( QString::fromStdString( *g[vd].namePtr ) );
                double sy = metrics.height();
                sum += sx*sy;
            }

            double ratio = 12.0;
            double x = sqrt( sum * ratio / 12.0 );
            _content_width = 4 * x;
            _content_height = 3 * x;

#ifdef DEBUG
            cerr << "width x height = " << width() * height() << endl;
            cerr << "label sum = " << sum << endl;
            cerr << "ratio = " << width() * height() / sum << endl;
            cerr << "new_content_width = " << _content_width
                 << " new_content_height = " << _content_height << endl;
#endif // DEBUG
            _gv->setSceneRect( -( _content_width + LEFTRIGHT_MARGIN )/2.0, -( _content_height + TOPBOTTOM_MARGIN )/2.0,
                               _content_width + LEFTRIGHT_MARGIN, _content_height + TOPBOTTOM_MARGIN );

            _boundaryPtr->normalizeSkeleton( _content_width, _content_height );
            _boundaryPtr->decomposeSkeleton();
            _boundaryPtr->normalizeComposite( _content_width, _content_height );

            _gv->isCompositeFlag() = true;

            redrawAllScene();
            break;
        }
        case Qt::Key_E:
        {
            // _gv->exportPNG( -width()/2.0, -height()/2.0, width(), height() );
            _gv->exportPNG( -( _content_width + LEFTRIGHT_MARGIN )/2.0, -( _content_height + TOPBOTTOM_MARGIN )/2.0,
                               _content_width + LEFTRIGHT_MARGIN, _content_height + TOPBOTTOM_MARGIN );
            break;
        }
        case Qt::Key_Escape:
            close();
            break;
        default:
            QWidget::keyPressEvent( event );
    }
}
