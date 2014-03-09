/* The Butterfly Effect
 * This file copyright (C) 2010,2014  Klaas van Gend
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

#ifndef PIVOTPOINT_H
#define PIVOTPOINT_H

#include "AbstractJoint.h"
#include "PivotPointPtr.h"

/// This class implements rotational joints - between objects or between one
/// object and "the world". 
/// Rotational joints can have an engine (implementing torque and/or 
/// rotational speed).
///
/// Example usage:
///    <object type="PivotPoint" X="3.2" Y="2.0">
///       <property key="object">Lever</property>
///    </object>
///
///    <object type="PivotPoint" X="3.2" Y="1.8">
///        <property key="object">SteamWheel</property>
///        <property key="Speed">1.0</property>
///        <property key="Torque">50.0</property>
///    </object>
///
///	   <object type="PivotPoint" X="3.95" Y="0.95">
///       <property key="object1">Bar</property>
///       <property key="object2">Handle</property>
///       <property key="collide">false</property>
///    </object>
/// See also http://sourceforge.net/apps/trac/tbe/wiki/PivotPoint
class PivotPoint : public AbstractJoint
{
public:
	/// empty constructor
	PivotPoint(void);

	/** constructor to add pivot for an object to world
	  * @param aAbstractObject
	  * @param aRelativePosition the (relative!) position of this pivot point
	  *                          relative to the center of the AbstractObject
	  */
	PivotPoint(AbstractObjectPtr aAbstractObject, const Vector& aRelativePosition);

	virtual ~PivotPoint();

    /// Overridden from AbstractObject/AbstractJoint: never create ViewObject.
    virtual ViewObject* createViewObject(float)
    { return NULL; }

	/// overridden from AbstractObject
	/// (this class does not have a body, only a joint)
	virtual void createPhysicsObject(void);

	/// overridden from AbstractObject
	/// returns the Name of the object.
	virtual const QString getName ( ) const
	{	return QObject::tr("PivotPoint");	}

	/// overridden from AbstractObject
	/// returns the Tooltip of the object.
	virtual const QString getToolTip ( ) const
	{	return QObject::tr("Objects rotate around this point"); }

	/// overridden from AbstractObject
	/// returns whether the object can be resized by the user
	virtual SizeDirections isResizable ( ) const
	{	return NORESIZING;	}

	/// overridden from AbstractObject
	/// returns true if the object can be rotated by the user
	virtual bool isRotatable ( ) const
	{	return false;	}

	/// overridden from AbstractObject
	/// parses all properties that PivotPoint understands
	virtual void  parseProperties(void);

	/// implemented from AbstractJoint
	virtual void updateOrigCenter(void);

protected:

private:
	void initPivotAttributes ( );

	AbstractObjectPtr theFirstPtr;
	AbstractObjectPtr theSecondPtr;

	/** use property 'collide' to set this.
	  * true means that objects can collide - useful for true hinges
	  */
	bool areObjectsColliding;

	Vector thePosRelativeToFirst;
};


#endif // PIVOTPOINT_H
