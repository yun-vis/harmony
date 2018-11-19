#ifndef WINDOW_H
#define WINDOW_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <map>
#include <list>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <cstdlib>

using namespace std;

#include <QtWidgets/QOpenGLWidget>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtGui/QMouseEvent>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QApplication>
#include <QtCore/QTimer>

#include "optimization/Force.h"
#include "optimization/Smooth.h"
#include "optimization/Octilinear.h"
#include "ui/GraphicsView.h"
#include "voronoi/Voronoi.h"
#include "base/Timer.h"

#define REMOVEBACKNUM   (15)

class Window : public QMainWindow, public PathwayData
{
    Q_OBJECT
private:

    QBasicTimer	    *_timer;

    // rendering
    GraphicsView    *_gv;

    // data structure
    Boundary        *_boundary;
    Boundary        *_simplifiedBoundary;

    // optimization
    Force           *_forceBoundaryPtr;
    vector< Force > *_forceCellVecPtr;
    vector< ForceGraph * >      _forceGraph;
    Smooth          *_smoothPtr;
    Octilinear      *_octilinearPtr;

    // display
    int         _content_width;
    int         _content_height;

    // menu
    // load
    QMenu *loadMenu;
    QAction *selDataAct;

    // simplification
    QMenu *simMenu;
    QAction *selCloneGraphAct;
    QAction *selMinDistanceAct;
    QAction *selMovebackSmoothAct;
    QAction *selMovebackOctilinearAct;

    // optimization
    QMenu *optMenu;
    QAction *selSmoothLSAct;
    QAction *selOctilinearLSAct;
    QAction *selSmoothSmallCGAct;
    QAction *selSmoothCGAct;
    QAction *selOctilinearSmallCGAct;
    QAction *selOctilinearCGAct;

    void createActions( void );
    void createMenus( void );

    void postLoad( void );
    void simulateKey( Qt::Key key );
    void redrawAllScene( void );

public slots:

    // load
    void selectData( void );

    // simplification
    void selectCloneGraph( void );
    void selectMinDistance( void );
    void selectMovebackSmooth( void );
    void selectMovebackOctilinear( void );

    // force
    void selectForce( void );

    // optimization
    void selectSmoothLS( void );
    void selectOctilinearLS( void );
    void selectSmoothSmallCG( void );
    void selectSmoothCG( void );
    void selectOctilinearSmallCG( void );
    void selectOctilinearCG( void );
    void selectSmooth( OPTTYPE opttype = CONJUGATE_GRADIENT );
    void selectOctilinear( OPTTYPE opttype = CONJUGATE_GRADIENT );

    // voronoi
    void selectVoronoi( void );
    void selectUpdateVoronoi( void );
    void selectBuildBoundary( void );

public:
    explicit Window( QWidget *parent = 0 );
    ~Window();

    void init( Boundary * __boundary, Boundary * __simBoundary,
               Force * __forceBoundary, vector< Force > * __forceCellVec,
               Smooth * __smoothPtr, Octilinear * __octilinearPtr );

protected:

    void keyPressEvent( QKeyEvent *event );
    void timerEvent( QTimerEvent *event );
};


#endif // WINDOW_H
