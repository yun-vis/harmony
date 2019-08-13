#ifndef WINDOW_H
#define WINDOW_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <map>
#include <list>
#include <algorithm>
#include <cmath>
#include <cstdlib>

using namespace std;

#ifndef Q_MOC_RUN
#include "ui/GraphicsView.h"
#include "base/TimeComplexity.h"
#include "base/ThreadOctilinearBoundary.h"
#include "base/ThreadLevelHigh.h"
#include "base/ThreadLevelCenter.h"
#include "base/ThreadLevelMiddle.h"
#include "base/ThreadLevelDetailed.h"
#include "base/RegionData.h"
#include "../lib/CTPL/ctpl.h"
#endif // Q_MOC_RUN

#include <QtWidgets/QOpenGLWidget>      // qt should be included after boost to avoid conflict
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtGui/QMouseEvent>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QApplication>
#include <QtCore/QTimer>
#include <QtCore/QThread>


#define REMOVEBACKNUM   (15)
//#define RECORD_VIDEO

class Window : public QMainWindow, public PathwayData, public RegionData, public Package
{
    Q_OBJECT
private:

#ifdef RECORD_VIDEO
    unsigned int             _timerVideoID;
    QBasicTimer              _timerVideo;
#endif // RECORD_VIDEO

    // rendering
    GraphicsView    *_gv;
    LEVELTYPE       _levelType;

    // display
    double             _content_width;
    double             _content_height;
    Polygon2           _contour;

    //*******************************************
    // menu
    //*******************************************
    // load
    QMenu *loadMenu;
    QAction *selDataAct;

    // optimization
    QMenu *optMenu;

    QAction *selOctilinearLSAct;
    QAction *selOctilinearCGAct;

    void postLoad( void );

    void simulateKey( Qt::Key key );

    void _init( void );

#ifdef RECORD_VIDEO
    void _timerVideoStart( void );
    void _timerVideoStop( void );
#endif //RECORD_VIDEO

    // thread
    void threadBoundaryForce( void );
    void threadCellForce( void );
    // void threadBoneForce( void );
    void threadCenterForce( void );
    void threadPathwayForce( void );
    void threadOctilinearBoundary( void );

    void steinertree( void );

    // octilinearity
    void processOctilinearBoundary( void );
    void spaceCoverage( void );

public Q_SLOTS:

    // optimization
    //void selectSmooth( void );
    //void selectOctilinear( void );

    // level high
    void selectLevelHighBuildBoundary( void );
    void buildLevelHighBoundaryGraph( void );
    void updateLevelHighPolygonComplex( void );

    // level middle
    void selectLevelMiddleBuildBoundary( void );
    void buildLevelMiddleBoundaryGraph( void );
    void updateLevelMiddlePolygonComplex( void );

    // level low
    // level detail
    void selectLevelDetailBuildBoundary( void );
    void buildLevelDetailBoundaryGraph( void );
    void updateLevelDetailPolygonComplex( void );

    // timer
#ifdef RECORD_VIDEO
    void timerVideo( void );
#endif //RECORD_VIDEO

    // display
    void redrawAllScene( void );

public:
    explicit Window( QWidget *parent = 0 );
    explicit Window( const Window & obj );
    ~Window();

    void init( void ){
        _init();
    }

protected:

    void keyPressEvent( QKeyEvent *event );
    void timerEvent( QTimerEvent *event );
};


#endif // WINDOW_H
