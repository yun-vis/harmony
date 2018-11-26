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
    _forceTargetFlag = false;

    Polygon2 contour;
    contour.elements().push_back( Coord2( - 0.5*_content_width, - 0.5*_content_height ) );
    contour.elements().push_back( Coord2( + 0.5*_content_width, - 0.5*_content_height ) );
    contour.elements().push_back( Coord2( + 0.5*_content_width, + 0.5*_content_height ) );
    contour.elements().push_back( Coord2( - 0.5*_content_width, + 0.5*_content_height ) );

    _boundary->forceBoundary().init( &_boundary->composite(), contour );

#ifdef SKIP
    void *ptr = &_boundary->boundary();
            ForceGraph *fgPtr = (ForceGraph*) (ptr);
            _forceBoundary.init( fgPtr, contour );
            printGraph( *fgPtr );
#endif // SKIP

    _gv->isPolygonFlag() = true;
}

void Window::_timerBoundaryStop( void )
{
    simulateKey( Qt::Key_E );
    _boundary->createPolygonComplex();
    simulateKey( Qt::Key_B );
    _boundary->writePolygonComplex();

    _gv->isPolygonFlag() = false;
    _gv->isPolygonComplexFlag() = true;
    _gv->isBoundaryFlag() = true;
    _gv->isCompositeFlag() = true;

    redrawAllScene();
}

void Window::timerBoundary( void )
{
    double err = 0.0;

    switch ( _boundary->forceBoundary().mode() ) {
        case TYPE_FORCE:
        {
            _boundary->forceBoundary().force();
            err = _boundary->forceBoundary().gap();
            cerr << "err = " << err << endl;
            if ( err < _boundary->forceBoundary().finalEpsilon() ) {
                _timer[0]->stop();
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
            cerr << "err = " << err << endl;
            if ( err < _boundary->forceBoundary().finalEpsilon() ) {
                _timer[0]->stop();
                _timerBoundaryStop();
                cerr << "[Centroidal] Finished Execution Time = " << checkOutETime() << endl;
                cerr << "[Centroidal] Finished CPU Time = " << checkOutCPUTime() << endl;
            }
        }
        case TYPE_HYBRID:
        {
            _boundary->forceBoundary().force();
            _boundary->forceBoundary().centroidGeometry();
            err = _boundary->forceBoundary().gap();
            cerr << "err = " << err << endl;
            if ( err < _boundary->forceBoundary().finalEpsilon() ) {
                _timer[0]->stop();
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
    _forceTargetFlag = true;
}

void Window::_timerPathwayCellStop( void )
{
    redrawAllScene();
}

void Window::timerPathwayCell( void )
{
    double err = 0.0;

    for( unsigned int i = 0; i < _cell.forceCellVec().size(); i++ ){

        // cerr << "i = " << i << " mode = " << _cell.forceCellVec()[i].mode() << endl;
        switch ( _cell.forceCellVec()[i].mode() ) {
            case TYPE_FORCE:
            {
                _cell.forceCellVec()[i].force();
                err = _cell.forceCellVec()[i].gap();
                cerr << "err = " << err << endl;
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
                err = _cell.forceCellVec()[i].gap();
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
                _cell.forceCellVec()[i].centroidGeometry();
                err = _cell.forceCellVec()[i].gap();
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

void Window::timerEvent( QTimerEvent *event )
{
    // cerr << "timer event..." << endl;
    Q_UNUSED( event );

    if( _forceTargetFlag ){
        timerPathwayCell();
    }
    else{
        timerBoundary();
    }

    redrawAllScene();
}


void Window::keyPressEvent( QKeyEvent *event )
{
    switch( event->key() ) {

        case Qt::Key_1:
        {
            _timerBoundaryStart();
            checkInETime();
            cerr << "*********** Starting Execution Time = " << checkOutETime() << endl;
            checkInCPUTime();
            cerr << "*********** Starting CPU Time = " << checkOutCPUTime() << endl;
            _timer[0]->start( 30, this );
            break;
        }
        case Qt::Key_2:
        {
            cerr << "stop timer event" << endl;
            _timer[0]->stop();
            _timerBoundaryStop();
            break;
        }
        case Qt::Key_3:
        {
            _timerPathwayCellStart();
            checkInETime();
            cerr << "*********** Starting Execution Time = " << checkOutETime() << endl;
            checkInCPUTime();
            cerr << "*********** Starting CPU Time = " << checkOutCPUTime() << endl;

            _timer.resize( _cell.forceCellVec().size() );
            for( unsigned int i = 0; i < _cell.forceCellVec().size(); i++ ) {
                _timer[i] = new QBasicTimer();
                _timer[i]->start( 30, this );
            }
            break;
        }
        case Qt::Key_4:
        {
            cerr << "stop timer event" << endl;
            for( unsigned int i = 0; i < _cell.forceCellVec().size(); i++ ) {
                _timer[i]->stop();
            }
            _timerPathwayCellStop();
            _gv->isPathwayFlag() = !_gv->isPathwayFlag();
            redrawAllScene();
            break;
        }
/*
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
*/
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
            redrawAllScene();
            break;
        }
        case Qt::Key_L:
        {
            // load setting
            _pathway->init( "../xml/A/", "../xml/tmp/",
                            "../xml/frequency/metabolite_frequency.txt", "../xml/type/typelist.txt" );
            _pathway->generate();
            _pathway->initLayout( _boundary->polygonComplex() );

            // set widget and graphicsview
            //_pathway->normalization();
            //_gv->isPathwayFlag() = true;

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
            _cell.updatePathwayCoords();
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
        case Qt::Key_S:
        {
            selectCloneGraph();
            selectSmoothCG();
            //selectSmoothSmallCboundary();
            break;
        }
        case Qt::Key_O:
        {
            selectCloneGraph();
            selectOctilinearCG();
            _boundary->updatePolygonComplex();
            //selectOctilinearSmallCboundary();
            break;
        }
        case Qt::Key_B:
        {
            selectBuildBoundary();
            break;
        }
        //case Qt::Key_P:
        case Qt::Key_F:
        case Qt::Key_U:
        {
            break;
        }
        case Qt::Key_V:
        {
            _boundary->buildSkeleton();
            _boundary->decomposeSkeleton();
            _boundary->normalizeSkeleton( _content_width, _content_height );

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
        case Qt::Key_Q:
        case Qt::Key_Escape:
            close();
            break;
        default:
            QWidget::keyPressEvent( event );
    }
}
