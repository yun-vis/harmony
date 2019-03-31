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
//#include <thread>
#include <cstdlib>

using namespace std;

#ifndef Q_MOC_RUN
#include "ui/GraphicsView.h"
#include "base/TimeComplexity.h"
#include "base/Controller.h"
#include "base/ControllerBoundary.h"
#include "base/RegionData.h"
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

    // threads
    vector< Controller * > _controllers;
    vector< ControllerBoundary * > _bControllers;

    // display
    double             _content_width;
    double             _content_height;
    Polygon2           _contour;

    // menu
    // load
    QMenu *loadMenu;
    QAction *selDataAct;

    // simplification
    //QMenu *simMenu;
    //QAction *selMovebackSmoothAct;
    //QAction *selMovebackOctilinearAct;

    // optimization
    QMenu *optMenu;
    //QAction *selSmoothLSAct;
    //QAction *selSmoothCGAct;

    QAction *selOctilinearLSAct;
    QAction *selOctilinearCGAct;

    void postLoad( void );
    void simulateKey( Qt::Key key );

    void _init( void );

#ifdef RECORD_VIDEO
    void _timerVideoStart( void );
    void _timerVideoStop( void );
#endif //RECORD_VIDEO

    // thread controller
    // boundary
    void processBoundaryForce( void );
    void processBoundaryStress( void );
    void stopProcessBoundary( void );
    // cell
    void processCellForce( void );
    void processCellStress( void );
    void stopProcessCell( void );
    // bone
    void processBoneForce( void );
    void processBoneStress( void );
    void stopProcessBone( void );
    // pathway
    void processDetailedPathwayForce( void );
    void processDetailedPathwayStress( void );
    void stopProcessDetailedPathway( void );
    void steinertree( void );

    // octilinearity
    void processOctilinearBoundary( void );
    void spaceCoverage( void );

public Q_SLOTS:

    // optimization
    //void selectSmooth( void );
    void selectOctilinear( void );

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

    // thread controller
    // boundary
    void listenProcessBoundary( void );
    // cell
    void listenProcessCell( void );
    // bone
    void listenProcessBone( void );
    // pathway
    void listenProcessDetailedPathway( void );
    // octilinear
    void listenProcessOctilinearBoundary( void );
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
