#ifndef GraphicsBallItem_H
#define GraphicsBallItem_H

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

#ifndef Q_MOC_RUN
#include "base/Coord2.h"
#include "base/Common.h"
#endif // Q_MOC_RUN

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
class GraphicsBallItem : public  QGraphicsRectItem
{
private:

    unsigned int _id;
    QString _name;
    bool    _textOn;

    int     _font_size;
    QFont   _font;
    QPen    _textpen;
    double  _radius;

protected:


public:
    explicit GraphicsBallItem( QGraphicsItem *parent = Q_NULLPTR );
    explicit GraphicsBallItem( const QRectF &rect, QGraphicsItem *parent = Q_NULLPTR );
    explicit GraphicsBallItem( qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent = Q_NULLPTR );
    ~GraphicsBallItem();

    // source from the qt library
    //QRectF rect() const;
    //void setRect(const QRectF &rect);
    //inline void setRect(qreal x, qreal y, qreal w, qreal h);

    //QPainterPath shape() const Q_DECL_OVERRIDE;
    //bool contains(const QPointF &point) const Q_DECL_OVERRIDE;

    //bool isObscuredBy(const QGraphicsItem *item) const Q_DECL_OVERRIDE;
    //QPainterPath opaqueArea() const Q_DECL_OVERRIDE;

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

    int &	                fontSize( void ) 	    { return _font_size; }
    const int &	            fontSize( void ) const	{ return _font_size; }

    bool &	                textOn( void ) 	    { return _textOn; }
    const bool &	        textOn( void ) const{ return _textOn; }

//------------------------------------------------------------------------------
//      Specific methods
//------------------------------------------------------------------------------
    void    init      ( void );

private:

};

#endif // GraphicsBallItem_H
