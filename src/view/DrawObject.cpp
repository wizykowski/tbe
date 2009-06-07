/* The Butterfly Effect 
 * This file copyright (C) 2009  Klaas van Gend
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation
 * applicable version is GPL version 2 only.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "DrawObject.h"
#include "BaseObject.h"

#include <QGraphicsScene>
#include <QPainter>
#include <QStyleOption>
#include <QGraphicsSceneMouseEvent>

// Constructors/Destructors
//  

DrawObject::DrawObject (BaseObject* aBaseObjectPtr)
	: theBaseObjectPtr(aBaseObjectPtr)
{
	theBodyID = aBaseObjectPtr->getTheBodyID();
	initAttributes();

	// the objects sizes usually are less than a meter
	// however, that does not sit well with QPainter, which is still a 
	// bitmap-oriented class - we're discussing images of less than one-by-one pixel.
	// that's what we need scaling for.
	//
	// theScale is set to 100.0 - that implies centimeters. 
    scale(1.0/theScale, 1.0/theScale);
    
    // in radians!
    theOldAngle=0;//aBaseObjectPtr->getTheCenter().angle;
    
    // make sure that this item is selectable & draggable
    // (if the object allows it - of course)
    if (aBaseObjectPtr->isMovable())
    	setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
    
//    setCacheMode(QGraphicsItem::ItemCoordinateCache, QSize(128,128));
    setToolTip(theBaseObjectPtr->getToolTip());
}

DrawObject::~DrawObject ( ) { }

//  
// Methods
//  


// Accessor methods
//  


// Other methods
//  

void DrawObject::advance(int)
{
	applyPosition();
}

void DrawObject::applyPosition(void)
{
    const dReal *pos1 = dGeomGetPosition (theBaseObjectPtr->getTheGeomID());
    const dReal *ang  = dGeomGetRotation (theBaseObjectPtr->getTheGeomID());

    // TODO: this can be done a lot nicer with QTranform
    qreal myAngle = atan2(ang[1], ang[0]);
    
    // Qt has Y positive downwards, whereas all of the model has Y upwards.
    // that's what the minus is for :-)
    setPos(pos1[0], -pos1[1]);
    rotate((myAngle-theOldAngle)*180/3.14);
    theOldAngle=myAngle;
}

QRectF DrawObject::boundingRect() const
{
	qreal myWidth = theBaseObjectPtr->getTheWidth()*theScale;
	qreal myHeight= theBaseObjectPtr->getTheHeight()*theScale;
	qreal adjust = 0.03;
	
    return QRectF(-myWidth/2-adjust, -myHeight/2-adjust, myWidth+2*adjust, myHeight+2*adjust);
}

void DrawObject::initAttributes ( ) 
{
	
}

void DrawObject::mouseMoveEvent ( QGraphicsSceneMouseEvent * event )
{
	DEBUG5("DrawObject::sceneEven(%d)\n", event->type());

	dReal orgposx = dGeomGetPosition(theBaseObjectPtr->getTheGeomID())[0];
	dReal orgposy = dGeomGetPosition(theBaseObjectPtr->getTheGeomID())[1];
	
	QPointF myPos=event->scenePos ();
	
	if ( (myPos.x()-theBaseObjectPtr->getTheWidth()/2.0) >= 0.0 
			&& (myPos.y()+theBaseObjectPtr->getTheHeight()/2.0) <= 0.0)
	{
		dGeomSetPosition(theBaseObjectPtr->getTheGeomID(), myPos.x(), -myPos.y(), 0.0);
		applyPosition();
		
		// if the new position collides with another, reset the position to the original one
		if (!scene()->collidingItems(this).isEmpty())
		{
			dGeomSetPosition(theBaseObjectPtr->getTheGeomID(), orgposx, orgposy, 0.0);
			applyPosition();
		}
	}
}


void DrawObject::paint(QPainter* myPainter, const QStyleOptionGraphicsItem *, QWidget *)
{
	qreal myWidth = theBaseObjectPtr->getTheWidth()*theScale;
	qreal myHeight= theBaseObjectPtr->getTheHeight()*theScale;

	DEBUG5("DrawObject::paint for %p: @(%f,%f)\n", this, myWidth, myHeight);
	
	QColor color(qrand() % 256, qrand() % 256, qrand() % 256);
    // Body
    myPainter->drawRect(-myWidth/2, -myHeight/2, myWidth, myHeight);

    myPainter->setBrush(color);

    myPainter->drawEllipse(-myWidth/2, -myHeight/2, myWidth, myHeight);

    applyPosition();
}
