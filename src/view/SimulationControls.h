/* The Butterfly Effect
 * This file copyright (C) 2011,2012 Klaas van Gend
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA.
 */

#ifndef SIMULATIONCONTROLS_H
#define SIMULATIONCONTROLS_H

#include <QLabel>
#include <QState>
#include <QMenu>
#include <QIcon>
#include <QStateMachine>
#include <QAction>
#include "tbe_global.h"
#include "ViewWorld.h"

namespace Ui {
class SimulationControls;
}

// forward declarations:
class MainWindow;

/// This direct QState derivative was created to have meaningful
/// debug messages when SimulationControls switches state
class SimState : public QState
{
    Q_OBJECT
public:
    explicit SimState(QState* parent = 0, const QString& aStateName = "")
        : QState(parent), isActive(false), theName(aStateName) {}

    virtual ~SimState()
    {}

    bool isActive;

protected:
    virtual void onEntry ( QEvent * event );
    virtual void onExit  ( QEvent * event );

private:
    QString theName;
};



/** This state machine controls how the simulation runs
  * inputs are the QActions that it creates in the menu bar and slots on its interface
  * outputs are the signals that it sends when states change
  */
class SimulationControls : public QLabel
{
    Q_OBJECT


public:
    explicit SimulationControls(QWidget *parent = 0);

    virtual ~SimulationControls();

    void setup(QMenu* aMenuPtr);

    void parentResize(const QSize& aSize);

    void hookSignalsUp(ViewWorld* aViewWorld, ResizingGraphicsView *aRGVPtr);

signals:
    /// emitted to go to really fast forward
    void go_quadspeed();

    void internalCrossPresent(void);
    void internalCrossGone(void);

    /// this signal is emitted when our slot onFailed is called
    void internalFailed(void);

    void reset(void);

public slots:
    /// call this slot if the simulation has failed, the internal
    /// state machine will go to the Failed state.
    void onFailed(void)
    { emit internalFailed(); }

    void onPause(void)
    { emit thePauseAction->trigger(); }

    void onReset(void)
    { emit theResetAction->trigger(); }

    /// Resizinggraphicsview connects CrossRegisterSingleton to this slot
    /// and it will be called whenever the user has a cross on one of his
    /// decorators.
    void slotNumberOfCrossesChanged(int aNewNumber);

private:
    QStateMachine theSimStateMachine;

    QAction* theForwardAction;
    QAction* thePlayAction;
    QAction* thePauseAction;
    QAction* theResetAction;
    QAction* the4FAction;   // hack for really fast forward

    QIcon theForwardIcon;
    QIcon thePauseIcon;
    QIcon thePlayIcon;
    QIcon theResetIcon;

    QPixmap the4FStatusPixmap;
    QPixmap theForwardStatusPixmap;
    QPixmap theFailedStatusPixmap;
    QPixmap thePausedStatusPixmap;
    QPixmap theProblemStatusPixmap;
    QPixmap theRunningStatusPixmap;
    QPixmap theStoppedStatusPixmap;

    SimState* the4FState;       // really fast forward
    SimState* theFailedState;   // happens upon death
    SimState* theForwardState;  // fast forward
    SimState* thePausedState;   // paused (but sim is active)
    SimState* theProblemState;  // at least one cross, user can change things
    SimState* theRunningState;  // sim is active and running
    SimState* theStoppedState;  // sim is inactive, user can change things

    Ui::SimulationControls *ui;
};

#endif // SIMULATIONCONTROLS_H
