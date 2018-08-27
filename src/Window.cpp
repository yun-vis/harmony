#include "Window.h"

ofstream    ofs( "../batch.txt" );

//----------------------------------------------------------
// Window
//----------------------------------------------------------
Window::Window( QOpenGLWidget *parent )
    : QMainWindow( parent )
{
    _gv = new GraphicsView( this );
    _gv->setGeometry( 0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT );
    setCentralWidget( _gv );

    createActions();
    createMenus();
}

Window::~Window()
{
}

void Window::init( Metro * __metro, Metro * __simmetro,
                   Smooth * __smooth, Octilinear * __octilinear )
{
    _metro = __metro;
    _simmetro = __simmetro;
    _smooth = __smooth;
    _octilinear = __octilinear;

    _gv->init( _metro, _simmetro );

    _content_width = DEFAULT_WIDTH - LEFTRIGHT_MARGIN;
    _content_height = DEFAULT_HEIGHT - TOPBOTTOM_MARGIN;
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

void Window::selectData( void )
{
    // load file
    //string datafilename = "../data/vienna-ubahn.txt";
    string datafilename = "../data/synthetic.txt";
    _metro->load( datafilename.c_str() );

    postLoad();
}

void Window::postLoad( void )
{
    _metro->adjustsize( _content_width/2, _content_height/2 );
    _metro->clearConflicts();

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

void Window::selectCloneGraph( void )
{
    _simmetro->cloneLayout( *_metro );
    _simmetro->clearConflicts();

    redrawAllScene();
}

void Window::selectMinDistance( void )
{
    _simmetro->simplifyLayout();
    redrawAllScene();
}

void Window::selectMovebackSmooth( void )
{
    bool isFinished = true;
    isFinished = _simmetro->movebackSmooth( *_metro );
    // if( isFinished == true ) cerr << "All stations are moved back!!" << endl;
    redrawAllScene();
}

void Window::selectMovebackOctilinear( void )
{
    bool isFinished = true;
    isFinished = _simmetro->movebackOctilinear( *_metro );
    // if( isFinished == true ) cerr << "All stations are moved back!!" << endl;
    redrawAllScene();
}

void Window::selectSmoothSmallCG( void )
{    
    // run coarse smooth optimization
    clock_t start_time = clock();
    double err = 0.0;
    unsigned int nLabels = _simmetro->nLabels();
    _smooth->prepare( _simmetro, _content_width, _content_height );
    err = _smooth->ConjugateGradient( 3 * _simmetro->nStations() );
    _smooth->retrieve();

    cerr << "simNStation = " << _simmetro->nStations() << endl;
    cerr << "nStation = " << _metro->nStations() << endl;
    // ofs << "    Coarse CG: " << clock() - start_time << " err = " << err << " iter = " << 2 * _simmetro->nStations() << endl;

    // run smooth optimization
    while( true ) {

        clock_t start = clock();
        int iter = 0;

        // check if all nodes are moved back
#ifdef  DEBUG
        cerr << " num_vertices( _simmetro->g() ) = " << num_vertices( _simmetro->g() ) << endl
             << " num_vertices( _metro->g() ) = " << num_vertices( _metro->g() ) << endl 
             << " nLabels = " << nLabels << endl;
#endif  // DEBUG
        if( num_vertices( _simmetro->g() ) == ( num_vertices( _metro->g() ) + nLabels ) ) {
            break;
        }
        else {
            for( int i = 0; i < REMOVEBACKNUM; i++ ){
                selectMovebackSmooth();
            }
        }

        _smooth->prepare( _simmetro, _content_width/2, _content_height/2 );
        if( num_vertices( _simmetro->g() ) == ( num_vertices( _metro->g() ) + nLabels ) ) {
            iter = MAX2( 2 * ( _metro->nStations() + nLabels - _simmetro->nStations() ), 30 );
        }
        else{
            iter = MAX2( 2 * ( _metro->nStations() + nLabels - _simmetro->nStations() ), 30 );
        }
        err = _smooth->ConjugateGradient( iter );
        _smooth->retrieve();

        cerr << "    time each loop = " << clock() - start << " err = " << err << " iter = " << iter << endl;
        // ofs << "    time each loop = " << clock() - start << " err = " << err << " iter = " << iter << endl;
    }
    //_simmetro->adjustsize( width(), height() );
    _smooth->clear();
    _metro->cloneSmooth( *_simmetro );

    // cerr << "Total Time CG = " << clock() - start_time << endl;
    // ofs << "Total Time CG = " << clock() - start_time << endl;

    redrawAllScene();
}

void Window::selectSmooth( OPTTYPE opttype )
{
    // run smooth optimization
    int iter = 0;
    _smooth->prepare( _metro, _content_width/2, _content_height/2 );
    switch( opttype ){
        case LEAST_SQUARE:
            iter = 2 * _metro->nStations();
            //iter = 20;
            _smooth->LeastSquare( iter );
            break;
        case CONJUGATE_GRADIENT:
            iter = 2 * _metro->nStations();
            _smooth->ConjugateGradient( iter );
            break;
        default:
            break;
    }
    _smooth->retrieve();
    _smooth->clear();

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
    unsigned int nLabels = _simmetro->nLabels();

    _octilinear->prepare( _simmetro, _content_width/2, _content_height/2 );
    err = _octilinear->ConjugateGradient( 5 * _simmetro->nStations() );
    _octilinear->retrieve();
    //ofs << "    Coarse CG: " << clock() - start_time << " err = " << err << " iter = " << 5 * _simmetro->nStations() << endl;

    // run octilinear optimization
    while( true ) {

        clock_t start = clock();
        int iter = 0;

        // check if all nodes are moved back
        if( num_vertices( _simmetro->g() ) == ( num_vertices( _metro->g() ) + nLabels ) ) {
            break;
        }
        else {
            for( int i = 0; i < REMOVEBACKNUM; i++ ){
                selectMovebackOctilinear();
            }
        }

        _octilinear->prepare( _simmetro, _content_width/2, _content_height/2 );
        if( num_vertices( _simmetro->g() ) == ( num_vertices( _metro->g() ) + nLabels ) ) {
            iter = MAX2( 2 * ( _metro->nStations() + nLabels - _simmetro->nStations() ), 30 );
        }
        else{
            iter = MAX2( 2 * ( _metro->nStations() + nLabels - _simmetro->nStations() ), 30 );
        }
        err = _octilinear->ConjugateGradient( iter );
        _octilinear->retrieve();

        // ofs << "    time each loop = " << clock() - start << " err = " << err << " iter = " << iter << endl;
    }

    _octilinear->clear();
    _metro->cloneOctilinear( *_simmetro );

    redrawAllScene();
}

void Window::selectOctilinear( OPTTYPE opttype )
{
    // run octilinear optimization
    double iter = 0;
    _octilinear->prepare( _metro, _content_width/2, _content_height/2 );
    switch( opttype ) {
        case LEAST_SQUARE:
            iter = 2 * _metro->nStations();
            _octilinear->LeastSquare( iter );
            break;
        case CONJUGATE_GRADIENT:
            iter = 2 * _metro->nStations();
            _octilinear->ConjugateGradient( iter );
            break;
        default:
            break;
    }
    _octilinear->retrieve();
    _octilinear->clear();

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

void Window::keyPressEvent( QKeyEvent *e )
{
    switch( e->key() ) {

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
        case Qt::Key_S:
        {
            selectCloneGraph();
            selectSmoothSmallCG();
            break;
        }
        case Qt::Key_O:
        {
            selectCloneGraph();
            selectOctilinearSmallCG();
            break;
        }
        case Qt::Key_Q:
        case Qt::Key_Escape:
            close();
            break;
        default:
            QWidget::keyPressEvent( e );
    }

    redrawAllScene();
}
