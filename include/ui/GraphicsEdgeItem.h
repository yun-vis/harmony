#ifndef GraphicsEdgeItem_H
#define GraphicsEdgeItem_H

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
class GraphicsEdgeItem : public  QGraphicsPathItem
{
private:

    unsigned int    _id;
    QString         _name;
    bool            _textOn;

protected:


public:
    explicit GraphicsEdgeItem( QGraphicsItem *parent = Q_NULLPTR );
    explicit GraphicsEdgeItem( const QPainterPath &path, QGraphicsItem *parent = Q_NULLPTR );
    ~GraphicsEdgeItem();

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

    QString &	            name( void )        { return _name; }
    const QString &	        name( void ) const	{ return _name; }

    bool &	                textOn( void ) 	    { return _textOn; }
    const bool &	        textOn( void ) const{ return _textOn; }

//------------------------------------------------------------------------------
//      Specific methods
//------------------------------------------------------------------------------


private:

};

#endif // GraphicsEdgeItem_H
