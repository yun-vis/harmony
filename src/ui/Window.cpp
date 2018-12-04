#include "ui/Window.h"

//ofstream    ofs( "../batch.txt" );

//----------------------------------------------------------
// Window
//----------------------------------------------------------
Window::Window( QWidget *parent )
    : QMainWindow( parent )
{
    _gv = new GraphicsView( this );
    _gv->setGeometry( 0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT );
    setCentralWidget( _gv );

    createActions();
    createMenus();

    //_smoothPtr = NULL;
    //_octilinearPtr = NULL;

    _timer.resize( 1 );
    _timer[0] = new QBasicTimer();
}

Window::~Window()
{
}

void Window::_init( Boundary * __boundary, Boundary * __simBoundary )
{
    _content_width = DEFAULT_WIDTH - LEFTRIGHT_MARGIN;
    _content_height = DEFAULT_HEIGHT - TOPBOTTOM_MARGIN;

    _boundary = __boundary;
    _simplifiedBoundary = __simBoundary;

    _cell.setPathwayData( _pathway );

    //_smoothPtr = __smoothPtr;
    //_octilinearPtr = __octilinearPtr;

    //if( _smoothPtr == NULL ) assert( _smoothPtr );
    //if( _octilinearPtr == NULL ) assert( _octilinearPtr );

    _gv->setPathwayData( _pathway );
    _gv->init( _boundary, _simplifiedBoundary, &_cell );
}

void Window::createActions( void )
{
    // load
    selDataAct = new QAction( tr("D&ata"), this );
    connect( selDataAct, SIGNAL( triggered() ), this, SLOT( selectData() ) );

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
    loadMenu = menuBar()->addMenu( tr("&Load") );
    loadMenu->addAction( selDataAct );

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

void Window::selectData( void )
{
    // load file
    string datafilename = "../data/vienna-ubahn.txt";
    //string datafilename = "../data/synthetic.txt";
    _boundary->load( datafilename.c_str() );

    postLoad();
}

void Window::postLoad( void )
{
    _boundary->adjustsize( _content_width/2, _content_height/2 );
    _boundary->clearConflicts();

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
    _simplifiedBoundary->cloneLayout( *_boundary );
    _simplifiedBoundary->clearConflicts();

    redrawAllScene();
}

void Window::selectMinDistance( void )
{
    _simplifiedBoundary->simplifyLayout();
    _gv->isSimplifiedFlag() = true;
    redrawAllScene();
}

void Window::selectMovebackSmooth( void )
{
    bool isFinished = true;
    isFinished = _simplifiedBoundary->movebackNodes( *_boundary, TYPE_SMOOTH );
    // if( isFinished == true ) cerr << "All stations are moved back!!" << endl;
    redrawAllScene();
}

void Window::selectMovebackOctilinear( void )
{
    bool isFinished = true;
    isFinished = _simplifiedBoundary->movebackNodes( *_boundary, TYPE_OCTILINEAR );
    // if( isFinished == true ) cerr << "All stations are moved back!!" << endl;
    redrawAllScene();
}

void Window::selectSmoothSmallCG( void )
{
    // run coarse smooth optimization
    clock_t start_time = clock();
    double err = 0.0;
    unsigned int nLabels = _simplifiedBoundary->nLabels();
    _smooth.prepare( _simplifiedBoundary, _content_width/2, _content_height/2 );
    err = _smooth.ConjugateGradient( 3 * _simplifiedBoundary->nVertices() );
    _smooth.retrieve();

    cerr << "simNStation = " << _simplifiedBoundary->nVertices() << endl;
    cerr << "nStation = " << _boundary->nVertices() << endl;
    // ofs << "    Coarse CG: " << clock() - start_time << " err = " << err << " iter = " << 2 * _simplifiedBoundary->nStations() << endl;

    // run smooth optimization
    while( true ) {

        clock_t start = clock();
        int iter = 0;

        // check if all nodes are moved back
#ifdef  DEBUG
        cerr << " num_vertices( _simplifiedBoundary->boundary() ) = " << num_vertices( _simplifiedBoundary->boundary() ) << endl
             << " num_vertices( _boundary->boundary() ) = " << num_vertices( _boundary->boundary() ) << endl
             << " nLabels = " << nLabels << endl;
#endif  // DEBUG
        if( num_vertices( _simplifiedBoundary->boundary() ) == ( num_vertices( _boundary->boundary() ) + nLabels ) ) {
            break;
        }
        else {
            for( int i = 0; i < REMOVEBACKNUM; i++ ){
                selectMovebackSmooth();
            }
        }

        _smooth.prepare( _simplifiedBoundary, _content_width/2, _content_height/2 );
        if( num_vertices( _simplifiedBoundary->boundary() ) == ( num_vertices( _boundary->boundary() ) + nLabels ) ) {
            iter = MAX2( 2 * ( _boundary->nVertices() + nLabels - _simplifiedBoundary->nVertices() ), 30 );
        }
        else{
            iter = MAX2( 2 * ( _boundary->nVertices() + nLabels - _simplifiedBoundary->nVertices() ), 30 );
        }
        err = _smooth.ConjugateGradient( iter );
        _smooth.retrieve();

        cerr << "    time each loop = " << clock() - start << " err = " << err << " iter = " << iter << endl;
        // ofs << "    time each loop = " << clock() - start << " err = " << err << " iter = " << iter << endl;
    }
    //_simplifiedBoundary->adjustsize( width(), height() );
    _smooth.clear();
    _boundary->cloneSmooth( *_simplifiedBoundary );

    // cerr << "Total Time CG = " << clock() - start_time << endl;
    // ofs << "Total Time CG = " << clock() - start_time << endl;

    redrawAllScene();
}

void Window::selectSmooth( OPTTYPE opttype )
{
    // run smooth optimization
    _smooth.prepare( _boundary, _content_width/2, _content_height/2 );
    switch( opttype ){
        case LEAST_SQUARE:
        {
            int iter = _boundary->nVertices();
            _smooth.LeastSquare( iter );
        }
            break;
        case CONJUGATE_GRADIENT:
        {
            int iter = _boundary->nVertices();
            _smooth.ConjugateGradient( iter );
        }
            break;
        default:
            break;
    }
    _smooth.retrieve();
    _smooth.clear();

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
    unsigned int nLabels = _simplifiedBoundary->nLabels();

    _octilinear.prepare( _simplifiedBoundary, _content_width/2, _content_height/2 );
    err = _octilinear.ConjugateGradient( 5 * _simplifiedBoundary->nVertices() );
    _octilinear.retrieve();
    //ofs << "    Coarse CG: " << clock() - start_time << " err = " << err << " iter = " << 5 * _simplifiedBoundary->nVertices() << endl;

    // run octilinear optimization
    while( true ) {

        clock_t start = clock();
        int iter = 0;

        // check if all nodes are moved back
        if( num_vertices( _simplifiedBoundary->boundary() ) == ( num_vertices( _boundary->boundary() ) + nLabels ) ) {
            break;
        }
        else {
            for( int i = 0; i < REMOVEBACKNUM; i++ ){
                selectMovebackOctilinear();
            }
        }

        _octilinear.prepare( _simplifiedBoundary, _content_width/2, _content_height/2 );
        if( num_vertices( _simplifiedBoundary->boundary() ) == ( num_vertices( _boundary->boundary() ) + nLabels ) ) {
            iter = MAX2( 2 * ( _boundary->nVertices() + nLabels - _simplifiedBoundary->nVertices() ), 30 );
        }
        else{
            iter = MAX2( 2 * ( _boundary->nVertices() + nLabels - _simplifiedBoundary->nVertices() ), 30 );
        }
        err = _octilinear.ConjugateGradient( iter );
        _octilinear.retrieve();

        // ofs << "    time each loop = " << clock() - start << " err = " << err << " iter = " << iter << endl;
    }

    _octilinear.clear();
    _boundary->cloneOctilinear( *_simplifiedBoundary );

    redrawAllScene();
}

void Window::selectOctilinear( OPTTYPE opttype )
{
    // run octilinear optimization
    _octilinear.prepare( _boundary, _content_width/2, _content_height/2 );

    switch( opttype ) {
        case LEAST_SQUARE:
        {
            int iter = _boundary->nVertices();
            _octilinear.LeastSquare( iter );
        }
            break;
        case CONJUGATE_GRADIENT:
        {
            int iter = _boundary->nVertices();
            _octilinear.ConjugateGradient( iter );
        }
            break;
        default:
            break;
    }
    _octilinear.retrieve();
    _octilinear.clear();

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
    _boundary->buildBoundaryGraph();
    _gv->isSimplifiedFlag() = false;
    redrawAllScene();
}

void Window::_timerBoundaryStart( void )
{
    _timerType = TIMER_BOUNDARY;

    Polygon2 contour;
    contour.elements().push_back( Coord2( - 0.5*_content_width, - 0.5*_content_height ) );
    contour.elements().push_back( Coord2( + 0.5*_content_width, - 0.5*_content_height ) );
    contour.elements().push_back( Coord2( + 0.5*_content_width, + 0.5*_content_height ) );
    contour.elements().push_back( Coord2( - 0.5*_content_width, + 0.5*_content_height ) );

    _boundary->forceBoundary().init( &_boundary->composite(), contour, "../configs/boundary.conf" );
#ifdef SKIP
    void *ptr = &_boundary->boundary();
            ForceGraph *fgPtr = (ForceGraph*) (ptr);
            _forceBoundary.init( fgPtr, contour );
            printGraph( *fgPtr );
#endif // SKIP

    _gv->isPolygonFlag() = true;

    _timer.resize(1);
    _timer[0]->start( 30, this );
}

void Window::_timerStop( void )
{
    for( unsigned int i = 0; i < _timer.size(); i++ ) {
        _timer[i]->stop();
        delete _timer[i];
    }
    _timer.clear();
}

void Window::_timerBoundaryStop( void )
{
    _timerStop();

    simulateKey( Qt::Key_E );
    _boundary->createPolygonComplex();
    simulateKey( Qt::Key_B );
    _boundary->writePolygonComplex();

    _gv->isPolygonFlag() = false;
    _gv->isPolygonComplexFlag() = true;
    _gv->isBoundaryFlag() = true;
    _gv->isCompositeFlag() = true;
}

void Window::timerBoundary( void )
{
    double err = 0.0;

    switch ( _boundary->forceBoundary().mode() ) {

        case TYPE_FORCE:
        {
            _boundary->forceBoundary().force();
            err = _boundary->forceBoundary().verletIntegreation();
            cerr << "err (force) = " << err << endl;
            if ( err < _boundary->forceBoundary().finalEpsilon() ) {
                _timerBoundaryStop();
                cerr << "[Force-Directed] Finished Execution Time = " << checkOutETime() << endl;
                cerr << "[Force-Directed] Finished CPU Time = " << checkOutCPUTime() << endl;
                //cerr << "FINAL widget_count" << widget_count << endl;
            }
            break;
        }
        case TYPE_CENTROID:
        {
            _boundary->forceBoundary().centroidGeometry();
            err = _boundary->forceBoundary().gap();
            cerr << "err (centroid) = " << err << endl;
            if ( err < _boundary->forceBoundary().finalEpsilon() ) {
                _timerBoundaryStop();
                cerr << "[Centroidal] Finished Execution Time = " << checkOutETime() << endl;
                cerr << "[Centroidal] Finished CPU Time = " << checkOutCPUTime() << endl;
            }
        }
        case TYPE_HYBRID:
        {
            _boundary->forceBoundary().force();
            _boundary->forceBoundary().centroidGeometry();
            err = _boundary->forceBoundary().verletIntegreation();
            cerr << "err (hybrid) = " << err << endl;
            if ( err < _boundary->forceBoundary().finalEpsilon() ) {
                _timerBoundaryStop();
                cerr << "[Hybrid] Finished Execution Time = " << checkOutETime() << endl;
                cerr << "[Hybrid] Finished CPU Time = " << checkOutCPUTime() << endl;
            }
        }
        default:
            break;
    }
}

void Window::_timerPathwayCellStart( void )
{
    _timerType = TIMER_PATHWAY_CELL;

   _timer.resize( _cell.forceCellVec().size() );
    for( unsigned int i = 0; i < _cell.forceCellVec().size(); i++ ) {
        _timer[i] = new QBasicTimer();
        _timer[i]->start( 30, this );
    }
}

void Window::_timerPathwayCellStop( void )
{
    bool isActive = false;
    for( unsigned int i = 0; i < _timer.size(); i++ ){
        isActive = isActive || _timer[i]->isActive();
    }
    if( !isActive ) {
        _timerStop();
        simulateKey( Qt::Key_P );
    }
}

void Window::timerPathwayCell( void )
{
    double err = 0.0;

    assert( _timer.size() == _cell.forceCellVec().size() );

    for( unsigned int i = 0; i < _cell.forceCellVec().size(); i++ ){

        // cerr << "i = " << i << " active = " << _timer[i]->isActive() << endl;
        if( _timer[i]->isActive() == true ){
            switch ( _cell.forceCellVec()[i].mode() ) {
                case TYPE_FORCE:
                {
                    _cell.forceCellVec()[i].force();
                    _cell.additionalForces();
                    err = _cell.forceCellVec()[i].verletIntegreation();
                    cerr << "err (force) = " << err << endl;
                    if ( err < _cell.forceCellVec()[i].finalEpsilon() ) {
                        _timer[i]->stop();
                        _timerPathwayCellStop();
                        cerr << "[Force-Directed] Finished Execution Time [" << i << "] = " << checkOutETime() << endl;
                        cerr << "[Force-Directed] Finished CPU Time [" << i << "] = " << checkOutCPUTime() << endl;
                        //cerr << "FINAL widget_count" << widget_count << endl;
                    }
                    break;
                }
                case TYPE_CENTROID:
                {
                    _cell.forceCellVec()[i].centroidGeometry();
                    _cell.additionalForces();
                    err = _cell.forceCellVec()[i].gap();
                    cerr << "err (centroid) = " << err << endl;
                    if ( err < _cell.forceCellVec()[i].finalEpsilon() ) {
                        _timer[i]->stop();
                        _timerPathwayCellStop();
                        cerr << "[Centroidal] Finished Execution Time [" << i << "] = " << checkOutETime() << endl;
                        cerr << "[Centroidal] Finished CPU Time [" << i << "] = " << checkOutCPUTime() << endl;
                    }
                }
                case TYPE_HYBRID:
                {
                    _cell.forceCellVec()[i].force();
                    _cell.additionalForces();
                    _cell.forceCellVec()[i].centroidGeometry();
                    err = _cell.forceCellVec()[i].verletIntegreation();
                    cerr << "err (hybrid) = " << err << endl;
                    if ( err < _cell.forceCellVec()[i].finalEpsilon() ) {
                        _timer[i]->stop();
                        _timerPathwayCellStop();
                        cerr << "[Hybrid] Finished Execution Time [" << i << "] = " << checkOutETime() << endl;
                        cerr << "[Hybrid] Finished CPU Time [" << i << "] = " << checkOutCPUTime() << endl;
                    }
                }
                default:
                    break;
            }
        }

    }
}

void Window::_timerPathwayStart( void )
{
    _timerType = TIMER_PATHWAY;
    unsigned int nComponent = _cell.nComponent();

    _timer.resize( nComponent );
    for( unsigned int i = 0; i < nComponent; i++ ){
        _timer[i] = new QBasicTimer();
        _timer[i]->start( 30, this );
    }
}

void Window::_timerPathwayStop( void )
{
    bool isActive = false;

    for( unsigned int i = 0; i < _timer.size(); i++ ){
        isActive = isActive || _timer[i]->isActive();
    }

    if( !isActive ) {
        _timerStop();
    }
}

void Window::timerPathway( void )
{
    double err = 0.0;
    vector< multimap< int, CellComponent > > &cellComponentVec = _cell.cellComponentVec();

    assert( _timer.size() == _cell.nComponent() );

    unsigned int idC = 0;
    for( unsigned int i = 0; i < cellComponentVec.size(); i++ ){

        multimap< int, CellComponent > &cellComponentMap = cellComponentVec[i];
        multimap< int, CellComponent >::iterator itC = cellComponentMap.begin();

        for( ; itC != cellComponentMap.end(); itC++ ){

            itC->second.detail.force();
            err = itC->second.detail.verletIntegreation();
            cerr << idC << ": err (pathway force) = " << err << endl;
            if ( err < itC->second.detail.finalEpsilon() ) {
                _timer[idC]->stop();
                _timerPathwayStop();
                cerr << "[Force-Directed] Finished Execution Time [" << idC << "] = " << checkOutETime() << endl;
                cerr << "[Force-Directed] Finished CPU Time [" << idC << "] = " << checkOutCPUTime() << endl;
            }
            idC++;
        }
    }
}

void Window::timerEvent( QTimerEvent *event )
{
    // cerr << "timer event..." << endl;
    Q_UNUSED( event );

    if( _timerType == TIMER_BOUNDARY ){
        timerBoundary();
    }
    else if( _timerType == TIMER_PATHWAY_CELL ){
        timerPathwayCell();
    }
    else if( _timerType == TIMER_PATHWAY ){
        cerr << "timerType = TIMER_PATHWAY" << endl;
        timerPathway();
    }
    else{
        cerr << "Something is wrong here... at" << __LINE__ << " in " << __FILE__ << endl;
    }

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
            assert( _timer.size() == 0 );
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
            simulateKey( Qt::Key_P );
            _timerPathwayCellStop();
            assert( _timer.size() == 0 );
            simulateKey( Qt::Key_9 );
            redrawAllScene();
            break;
        }
        case Qt::Key_A:
        {
            checkInETime();
            cerr << "*********** Starting Execution Time = " << checkOutETime() << endl;
            checkInCPUTime();
            cerr << "*********** Starting CPU Time = " << checkOutCPUTime() << endl;
            _timerPathwayStart();
            break;
        }
        case Qt::Key_S:
        {
            cerr << "stop timer event" << endl;
            _timerStop();
            _timerPathwayStop();
            redrawAllScene();
            assert( _timer.size() == 0 );
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
        case Qt::Key_L:
        {
            // load setting
            _pathway->initLayout( _boundary->polygonComplex() );

            // initialize cell
            _cell.clear();
            _cell.init( &_boundary->polygonComplex() );

            simulateKey( Qt::Key_5 );
            simulateKey( Qt::Key_9 );
            _gv->isCellFlag() = true;
            redrawAllScene();
            break;
        }
        case Qt::Key_P:
        {
            _cell.createPolygonComplex();
            _cell.updatePathwayCoords();
            _gv->isCellPolygonComplexFlag() = true;
            _gv->isPathwayFlag() = true;
            redrawAllScene();
            break;
        }
        case Qt::Key_I:
        {
            selectData();
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
            _boundary->readPolygonComplex();
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
            _boundary->updatePolygonComplex();
            //selectOctilinearSmallCboundary();
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
            //                "../xml/frequency/metabolite_frequency.txt", "../xml/type/typelist.txt" );
            _pathway->init( "../xml/A/", "../xml/tmp/",
                            "../xml/frequency/metabolite_frequency.txt", "../xml/type/typelist.txt" );
            _pathway->generate();

            _boundary->init( _pathway->skeletonG() );
            //_boundary->buildSkeleton();
            _boundary->normalizeSkeleton( _content_width, _content_height );
            _boundary->decomposeSkeleton();
            _boundary->normalizeComposite( _content_width, _content_height );

            _gv->isCompositeFlag() = true;

            //selectVoronoi();
            redrawAllScene();
            break;
        }
        case Qt::Key_E:
        {
            _gv->exportPNG( -width()/2.0, -height()/2.0, width(), height() );
            break;
        }
        case Qt::Key_Escape:
            close();
            break;
        default:
            QWidget::keyPressEvent( event );
    }
}
