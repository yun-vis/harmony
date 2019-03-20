#ifndef GraphicsPolygonItem_H
#define GraphicsPolygonItem_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <map>
#include <list>
#include <vector>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <cstdlib>

using namespace std;

//#ifndef Q_MOC_RUN
#include "base/Common.h"
//#endif // Q_MOC_RUN

#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsItem>
#include <QtGui/QPainter>
#include <QtCore/QString>
#include <QtWidgets/QGraphicsSceneMouseEvent>


//------------------------------------------------------------------------------
//	Macro definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Class definition
//------------------------------------------------------------------------------
class GraphicsPolygonItem : public QGraphicsPolygonItem
{
private:

    unsigned int    _id;
    bool            _textOn;

protected:


public:
    explicit GraphicsPolygonItem( QGraphicsItem *parent = Q_NULLPTR );
    explicit GraphicsPolygonItem( QPolygonF &polygon, QGraphicsItem *parent = Q_NULLPTR );
    ~GraphicsPolygonItem();

    int type( void ) const Q_DECL_OVERRIDE;

    // source from the qt library
    //QPainterPath path() const;
    //void setPath(const QPainterPath &path);

    //QPainterPath shape() const Q_DECL_OVERRIDE;
    //bool contains(const QPointF &point) const Q_DECL_OVERRIDE;

//------------------------------------------------------------------------------
//      Reimplementation
//------------------------------------------------------------------------------
    QRectF boundingRect() const Q_DECL_OVERRIDE;

    void paint( QPainter *painter, const QStyleOptionGraphicsItem *option,
                QWidget *widget = Q_NULLPTR ) Q_DECL_OVERRIDE;


//------------------------------------------------------------------------------
//      Reference to members
//------------------------------------------------------------------------------

    unsigned int &	        id( void ) 	        { return _id; }
    const unsigned int &	id( void ) const	{ return _id; }

    bool &	                textOn( void ) 	    { return _textOn; }
    const bool &	        textOn( void ) const{ return _textOn; }

//------------------------------------------------------------------------------
//      Specific methods
//------------------------------------------------------------------------------


private:

};

#endif // GraphicsPolygonItem_H
