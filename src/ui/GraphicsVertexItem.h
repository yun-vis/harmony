#ifndef GraphicsVertexItem_H
#define GraphicsVertexItem_H

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
#include <cassert>

using namespace std;

#ifndef Q_MOC_RUN

#include "core/Coord2.h"
#include "core/Common.h"
#include "ui/GraphicsBase.h"

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
class GraphicsVertexItem : public QGraphicsRectItem, public GraphicsBase {
private:
	
	VERTEXTYPE _vtype;
	QRectF _fineRect;

protected:


public:
	explicit GraphicsVertexItem( QGraphicsItem *parent = Q_NULLPTR );
	
	explicit GraphicsVertexItem( const QRectF &rect, QGraphicsItem *parent = Q_NULLPTR );
	
	explicit GraphicsVertexItem( qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent = Q_NULLPTR );
	
	~GraphicsVertexItem();
	
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
	
	VERTEXTYPE &vtype( void ) { return _vtype; }
	
	const VERTEXTYPE &vtype( void ) const { return _vtype; }


//------------------------------------------------------------------------------
//      Specific methods
//------------------------------------------------------------------------------
	void init( void );
	
};

#endif // GraphicsVertexItem_H
