/* The Butterfly Effect
 * This file copyright (C) 2011 Klaas van Gend
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

#include "UndoSingleton.h"
#include "RotateUndoCommand.h"
#include "ResizeUndoCommand.h"
#include "MoveUndoCommand.h"
#include "InsertUndoCommand.h"
#include "DeleteUndoCommand.h"
#include "ChoosePhoneUndoCommand.h"

static UndoSingleton* theUndoSingletonPtr = NULL;
static AbstractUndoCommand* theCurrentlyActiveUndoCommand = NULL;


UndoSingleton::UndoSingleton(void)
{
	// nothing to do
}


void UndoSingleton::clear()
{
	me()->theUndoStack.clear();
}

QAction* UndoSingleton::createRedoAction (QObject* parent,const QString& prefix)
{
	return me()->theUndoStack.createRedoAction(parent, prefix);
}


QAction* UndoSingleton::createUndoAction (QObject* parent,const QString& prefix)
{
	return me()->theUndoStack.createUndoAction(parent, prefix);
}


AbstractUndoCommand*
UndoSingleton::createUndoCommand(ViewObject* anObject,
								 ActionIcon::ActionType anUndoType)
{
	qDebug() << Q_FUNC_INFO << "action type: " << anUndoType;

	AbstractUndoCommand* myNewCommand = NULL;
	// return immediately for UndoActions that do not need further
	// user interactions (i.e. those derived from Dummy).
	switch(anUndoType)
	{
	case ActionIcon::ACTION_INSERT:
		return new InsertUndoCommand(anObject);
		break;
	case ActionIcon::ACTION_MOVE:
		myNewCommand = new MoveUndoCommand(anObject);
		break;
	case ActionIcon::ACTION_ROTATE:
		myNewCommand =  new RotateUndoCommand(anObject);
		break;
	case ActionIcon::ACTION_RESIZE:
		myNewCommand =  new ResizeUndoCommand(anObject);
		break;
	case ActionIcon::ACTION_SETPHONE:
		return new ChoosePhoneUndoCommand(anObject);
		break;
	case ActionIcon::ACTION_DELETE:
		return new DeleteUndoCommand(anObject);
		break;
	case ActionIcon::ACTION_EDITSPECIAL:
		// TODO/FIXME
		break;
	}
	if (theCurrentlyActiveUndoCommand!=NULL)
		delete theCurrentlyActiveUndoCommand;
	theCurrentlyActiveUndoCommand = myNewCommand;
	return myNewCommand;
}


UndoSingleton* UndoSingleton::me(void)
{
	if (theUndoSingletonPtr==NULL)
		theUndoSingletonPtr = new UndoSingleton();
	return theUndoSingletonPtr;
}


void UndoSingleton::notifyGone(AbstractUndoCommand* anAUCPtr)
{
	if (anAUCPtr == theCurrentlyActiveUndoCommand)
		theCurrentlyActiveUndoCommand = NULL;
}


void UndoSingleton::push(AbstractUndoCommand* anAUCPtr)
{
	qDebug() << "pushed " << anAUCPtr->text();
	me()->theUndoStack.push(anAUCPtr);
	notifyGone(anAUCPtr);
}


void UndoSingleton::setClean()
{
    DEBUG3ENTRY;
    me()->theUndoStack.setClean();
}
