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

#include "optimization/Force.h"
#include "optimization/Smooth.h"
#include "optimization/Octilinear.h"
#include "ui/GraphicsView.h"
#include "voronoi/Voronoi.h"

#define REMOVEBACKNUM   (15)

class Window : public QMainWindow
{
    Q_OBJECT
private:

    // rendering
    GraphicsView    *_gv;

    // data structure
    Boundary        *_boundary;
    Boundary        *_simplifiedBoundary;

    // optimization
    Force           *_forcePtr;
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
    void selectBuildBoundary( void );

public:
    explicit Window( QWidget *parent = 0 );
    ~Window();

    void init( Boundary * __metro, Boundary * __simmetro, Force * __force,
               Smooth * __smoothPtr, Octilinear * __octilinearPtr );

protected:

    void keyPressEvent( QKeyEvent *event );
};


#endif // WINDOW_H
