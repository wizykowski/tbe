/* The Butterfly Effect
 * This file copyright (C) 2009,2011,2012,2013,2014,2015 Klaas van Gend
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

#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QtCore>
#include <QtGui>
#include <QListWidgetItem>
#include <QFileDialog>

#include "AbstractObject.h"
#include "ChooseLevel.h"
#include "EditLevelProperties.h"
#include "GoalEditor.h"
#include "Hint.h"
#include "ImageCache.h"
#include "InsertUndoCommand.h"
#include "Level.h"
#include "ListViewItemTooltip.h"
#include "ObjectFactory.h"
#include "Popup.h"
#include "RegressionTest.h"
#include "SaveLevelInfo.h"
#include "ToolboxListWidgetItem.h"
#include "UndoSingleton.h"
#include "World.h"


MainWindow::MainWindow(bool isMaximized, QWidget *parent)
	: QMainWindow(parent),
	  theRegressionTest(nullptr),
	  ui(new Ui::MainWindow),
	  theLevelPtr(nullptr),
	  theWorldPtr(nullptr)
{
    ui->setupUi(this);

    setupView();
    if (isMaximized)
        showMaximized();
    if (theIsLevelEditor)
        on_action_Switch_to_Level_Editor_triggered();
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type())
    {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}


void MainWindow::loadLevel(const QString& aFileName)
{
	DEBUG1ENTRY;
	if (theLevelPtr != nullptr)
		purgeLevel();

	// create level and display in main window
	theLevelPtr = new Level();
	QString myErrorMessage = theLevelPtr->load(aFileName,
								   ui->graphicsView->getGameResourcesDialogPtr());
	if (!myErrorMessage.isEmpty())
	{
		QChar myFirst = myErrorMessage[0];
		myErrorMessage = myErrorMessage.mid(1);
		if (myFirst == 'E')
		{
			Popup::Critical(tr("ERROR during reading file '%1': '%2'\n")
							.arg(aFileName).arg(myErrorMessage), this );
			exit(1);
		}
		if (myFirst == 'W')
		{
			Popup::Warning(tr("Non-fatal problem reading file '%1': '%2'.\n"
							  "This may affect playability, though!")
							.arg(aFileName).arg(myErrorMessage), this );
		}
	}

    ui->listWidget->clear();
    theLevelPtr->getTheWorldPtr()->createScene(ui->graphicsView);
    for (auto i : theLevelPtr->theToolboxList)
        new ToolboxListWidgetItem(ui->graphicsView, i, ui->listWidget);
}


void MainWindow::loadLevelDelayed()
{
	DEBUG1ENTRY;
	loadLevel(theStartFileName);
}


void MainWindow::on_action_About_triggered()
{
    //: translators: <b> and <br> are statements for bold and newline, respectively
    Popup::Info(tr("<b>The Butterfly Effect</b><br><br>"
                   "An open source game that uses realistic physics"
                   " simulations to combine lots of simple mechanical elements"
                   " to achieve a simple goal in the most complex way possible.<br><br>"
                   "(C) 2009,2010,2011,2012,2013,2015 Peter van Ginneken and Klaas van Gend<br>"
                   "Licensed under GPL version 2 - <i>only</i>.<br><br>"
				   "See <a href=\"http://%1/\">http://%1/</a> for more info on this project.")
                .arg(QCoreApplication::instance()->organizationDomain()), this);
}


void MainWindow::on_action_Bug_Reports_triggered()
{
    //: translators: <b> and <br> are statements for bold and newline, respectively
    Popup::Info(tr("<b>The Butterfly Effect - Bug Reports</b><br><br>"
                   "Of course, this game is not bug free yet.<br>"
                   "If you come across anything that you think should not "
                   "happen, please let us know. Go to our ticket website:"
                   "<br><a href=\""
                   "https://github.com/kaa-ching/tbe/issues\">"
                   "https://github.com/kaa-ching/tbe/issues</a><br>"
                   "Please tell us at least the <i>name</i> of the level, what you "
                   "expected to happen and what did happen. If you want to "
                   "learn how we fix your issue, please provide a valid "
                   "e-mail address."
                   ""), this);
}


void MainWindow::on_action_Keyboard_Shortcuts_triggered()
{
    //: translators: <b> and <br> are statements for bold and newline, respectively, please make sure to
    //: please make sure to match the statements in this dialog with your shortcuts
    Popup::Info(tr("<b>The Butterfly Effect - Keyboard shortcuts</b><br><br>"
                   "The following keys can be used to accelerate actions you'd have to do with the mouse otherwise"
                   "<table cellpadding=\"4\">"
                   "<tr><th align=\"left\">Key</th><th align=\"left\">Function </th></tr> "
                   "<tr><td>Space</td><td>start / stop simulation </td></tr> "
                   "<tr><td>f</td><td>(during simulation) fast forward / slow down </td></tr> "
                   "<tr><td>shift+f</td><td>(during simulation) really fast forward </td></tr> "
                   "<tr><td>r</td><td>reset the simulation </td></tr> "
                   "<tr><td>Ctrl-Z</td><td> undo last action </td></tr> "
                   "<tr><td>Ctrl-Y</td><td> redo last action </td></tr> "
                   "</table>"), this);
}


void MainWindow::on_action_Libraries_triggered()
{
	//: translators: <b> and <br> are statements for bold and newline, respectively
	Popup::Info(tr("<b>The Butterfly Effect - Libraries</b><br><br>"
				"The Butterfly Effect is a proud user of the Box2D "
				"Physics Library. Please refer to <a href=\"http://www.box2d.org/\">http://www.box2d.org/</a>."
				"<br>"
				"The Butterfly Effect uses the Qt GUI toolkit. "
				"Please refer to <a href=\"http://qt-project.org/\">http://qt-project.org/</a>."
				""), this);
}


void MainWindow::on_action_New_triggered()
{
	if (nullptr!=theLevelPtr)
	{
		// there's a level already here???
		// TODO: are there any unsaved changes (we currently cannot check for
		//       that - see issue #56 for that)
		if (!Popup::YesNoQuestion(tr("Do you really want to discard the "
									 "current level and start a new one?"),
								  this))
			return;
		purgeLevel();
	}

	theLevelPtr = new Level();
	theStartFileName.clear();
	ui->listWidget->clear();

	// pop-up the modal LevelProperties dialog
	on_levelPropertiesEditorAction_clicked();

	// repopulate the scene and toolbox
	theLevelPtr->getTheWorldPtr()->createScene(ui->graphicsView);
	for (auto i : theLevelPtr->theToolboxList)
		new ToolboxListWidgetItem(ui->graphicsView, i, ui->listWidget);
}


void MainWindow::on_action_New_Level_Ideas_triggered()
{
	//: translators: <b> and <br> are statements for bold and newline, respectively
	Popup::Info(tr("<b>The Butterfly Effect - Create New Levels</b><br><br>"
				   "We know you can design better levels than we do!<br>"
				   "Use the Level Creator to build your levels and please submit them to us.<br>"
				   "Even if your level is not finished yet, don't hesitate to share it with us! "
				   "Of course, define how you think it should work so others can join in."
				   "<br><br>Please file a ticket on github with your idea:<br>"
				   "<a href=\"https://github.com/kaa-ching/tbe/issues\">https://github.com/kaa-ching/tbe/issues</a><br>"
				   ""), this);
}


void MainWindow::on_action_Open_Level_triggered()
{
    ChooseLevel* myDialogPtr = new ChooseLevel(ui->graphicsView);
    connect(myDialogPtr, SIGNAL(loadLevel(QString)),
            this, SLOT(loadLevel(QString)));
}


void MainWindow::on_action_Open_File_triggered()
{
    QString myFileName = QFileDialog::getOpenFileName(this,
                                                      tr("Open level"), ".", tr("TBE levels (*.xml)"));
    if (myFileName.isEmpty())
        return;
    loadLevel(myFileName);
}


void MainWindow::on_action_Quit_triggered()
{
    if (Popup::YesNoQuestion(tr("really?"), this))
        QApplication::exit(0);
}


void MainWindow::on_action_Save_triggered()
{
    DEBUG1ENTRY;
	if (nullptr == theLevelPtr)
        return;

	if (theLevelPtr->getLevelFileName().isEmpty())
	{
		Popup::Warning(tr("Level has no name - could not be saved. Please use \"Save As...\""));
		return;
	}
    QFileInfo myFileInfo(theLevelPtr->getLevelFileName());
	if (false==theLevelPtr->save(myFileInfo.absoluteFilePath()))
        Popup::Warning(tr("File '%1' could not be saved.").arg(myFileInfo.absoluteFilePath()));
    else
		DEBUG2("File '%s' saved.",ASCII(myFileInfo.absoluteFilePath()));
}


void MainWindow::on_action_Save_As_triggered()
{
    DEBUG1ENTRY;
    Q_ASSERT(theLevelPtr);
	if (nullptr == theLevelPtr)
        return;

	SaveLevelInfo mySaveLevel(theLevelPtr, this);
    int myReturnCode = mySaveLevel.exec();
    if (myReturnCode == QDialog::Rejected)
        return;

    // for now we ignore the return code...
    if (mySaveLevel.commitToLevel()==false)
        Popup::Warning(tr("You did not fill in all fields - but level saved anyway\n"));

    QFileInfo myFileInfo(theLevelPtr->getLevelFileName());

	DEBUG5("File '%s' is readable: %d, writeable: %d",
           ASCII(myFileInfo.absoluteFilePath()),
           myFileInfo.isReadable(), myFileInfo.isWritable());

    on_action_Save_triggered();

}


void MainWindow::on_action_Skip_Level_triggered()
{
    if (Popup::YesNoQuestion(tr("Mark this level 'skipped' and continue with the next level?"), this))
        ui->graphicsView->slot_actionSkipLevel();
}


void MainWindow::on_action_Suggestions_triggered()
{
	Popup::Info(tr("<b>The Butterfly Effect - Suggestions</b><br><br>"
				   "If you have great ideas for new features in the game, "
				   "please go to our shiny forums at: <br><a href=\""
				   "http://the-butterfly-effect.org/\">"
				   "http://the-butterfly-effect.org/</a>"
				   "<br>to share your ideas with the world."
				""), this);
}


void MainWindow::on_action_Switch_to_Level_Editor_triggered()
{
    // add "New", "Save" and "Save as" items to File menu
    typedef QList<QAction*> ActionList;
    ActionList myList = ui->menuFile->actions();
    for (auto i : myList)
    {
        if (!i->isVisible())
        {
            i->setVisible(true);
            i->setEnabled(true);
        }
        if (i->data()=="Switch")
        {
            i->setVisible(false);
            i->setEnabled(false);
        }
    }

    // add new drop-down menu "Insert" (and put it before the "Controls" menu)
    QMenu* myInsertMenuPtr = new QMenu(tr("&Insert"), nullptr);
    ui->menuBar->insertMenu(ui->menuControls->menuAction(), myInsertMenuPtr);
    // add all objects into it
    ObjectFactory::ObjectFactoryList* myOFListPtr = ObjectFactory::getAllFactories();
    for (auto i : *myOFListPtr)
    {
        // TODO: remove the Link-derived ones from the list
        // TODO: add icons to the action
        InsertMenuQAction* myTempActionPtr = new InsertMenuQAction(i->getFactoryName(), nullptr);
        connect(myTempActionPtr, SIGNAL(triggeredName(QString)), this, SLOT(on_insert(QString)));
        myInsertMenuPtr->addAction(myTempActionPtr);
    }
    delete myOFListPtr;
    myOFListPtr = nullptr;

    // add new top menu "Editors"
    QMenu* myEditorsMenuPtr = new QMenu(tr("&Editors"), nullptr);
    ui->menuBar->insertMenu(ui->menuControls->menuAction(), myEditorsMenuPtr);
    // TODO: add some of the original dialogs to it
    QAction* myGoalActionPtr = new QAction(tr("&Goal Editor..."), nullptr);
    connect (myGoalActionPtr, SIGNAL(triggered()), this, SLOT(on_goalEditorAction_clicked()));
    myEditorsMenuPtr->addAction(myGoalActionPtr);
    QAction* myLevPropActionPtr = new QAction(tr("&Level Properties..."), nullptr);
    connect (myLevPropActionPtr, SIGNAL(triggered()), this, SLOT(on_levelPropertiesEditorAction_clicked()));
    myEditorsMenuPtr->addAction(myLevPropActionPtr);
    QAction* myEditObjectActionPtr = new QAction(tr("&Object Editor..."), nullptr);
    connect (myEditObjectActionPtr, SIGNAL(triggered()), this, SLOT(on_objectEditorAction_clicked()));
    myEditorsMenuPtr->addAction(myEditObjectActionPtr);
    // Enable level editor mode
    theIsLevelEditor = true;
    ui->action_Switch_to_Level_Editor->setEnabled(false);
    // TODO: it would be marvellous to have Cut/Copy/Paste in the Edit menu!
	ui->action_Open_File->setEnabled(true);
	ui->action_Open_File->setVisible(true);
}

void MainWindow::on_goalEditorAction_clicked()
{
    // the Goals dialog is modeless, i.e. it can stay floating around
    GoalEditor* myGoalEditorPtr = new GoalEditor(theLevelPtr->getTheWorldPtr(), this);
    myGoalEditorPtr->show();
}

void MainWindow::on_insert(const QString& anObjectName)
{
    DEBUG1ENTRY;
    InsertUndoCommand::createInsertUndoCommand(
                ObjectFactory::createObject(anObjectName, Position(1,1)));
}

void MainWindow::on_levelPropertiesEditorAction_clicked()
{
    // this dialog is modal, i.e. user has to click OK/Cancel
    EditLevelProperties* myEditorPtr = new EditLevelProperties(theLevelPtr, this);
    myEditorPtr->show();
    // I don't care about the return.
    // If it was OK, the dialog already 'fixed' everything.
    myEditorPtr->exec();
}


void MainWindow::on_objectEditorAction_clicked()
{
    emit dynamic_cast<ResizingGraphicsView*>(ui->graphicsView)->slot_showEditObjectDialog(nullptr);
}


void MainWindow::purgeLevel()
{
	DEBUG1ENTRY;
	UndoSingleton::clear();
	delete theLevelPtr;
	theLevelPtr=nullptr;
	ui->graphicsView->clearViewWorld();
}


void MainWindow::reloadLevel()
{
	DEBUG1ENTRY;
	if (theLevelPtr==nullptr)
		return;
	QString myLevelName = theLevelPtr->getLevelFileName();
	purgeLevel();
	loadLevel(myLevelName);
}

bool MainWindow::slot_insertHint(unsigned int aHintNumber) const
{
	DEBUG1ENTRY;
	Hint* myHintPtr = theLevelPtr->getHint(aHintNumber);
	if (myHintPtr==nullptr)
		return false;

	// If we get here, the hint exists.
	// Because we only care about regression right now, we don't need to worry
	// that the object has been inserted before, that the hint has been used
	// before, etc...

	// Find the right ToolboxGroup.
	// We need to work with the internal name - the other name might be translated...
	ToolboxGroup* myTBGPtr = nullptr;
	for(int i=0; i < ui->listWidget->count(); i++)
	{
		ToolboxListWidgetItem* myItemPtr = dynamic_cast<ToolboxListWidgetItem*>(ui->listWidget->item(i));
		ToolboxGroup* myGPtr = myItemPtr->getToolboxGroupPtr();
		if (myGPtr->theInternalName == myHintPtr->getHintInternalName())
		{
			myTBGPtr = myGPtr;
			break;
		}
	}
	if (myTBGPtr == nullptr)
		return false;

	// OK, we found the TBG to take the object from.
	// Create the InsertUndoCommand
	InsertUndoCommand::createInsertUndoCommand(myTBGPtr, myHintPtr);


	// successfully done
	return true;
}

void MainWindow::setupView()
{
    // setup UndoGroup's QActions and add them to Edit menu
    // note that this doesn't enable them yet, our ViewWorld should handle that...
    QAction* myUndoActionPtr = UndoSingleton::createUndoAction(this, tr("&Undo"));
    myUndoActionPtr->setShortcut(tr("Ctrl+Z"));
    ui->menuEdit->addAction(myUndoActionPtr);
    QAction* myRedoActionPtr = UndoSingleton::createRedoAction(this, tr("&Redo"));
    QList<QKeySequence> redoShortcuts;
    redoShortcuts << tr("Ctrl+Y") << tr("Shift+Ctrl+Z");
    myRedoActionPtr->setShortcuts(redoShortcuts);
    ui->menuEdit->addAction(myRedoActionPtr);

	// set up the actions for the two buttons below the toolbox
	QIcon myEjectIcon  = ImageCache::getQIcon("ActionMenuEject", QSize(16,16));
	ui->toolButton_chooseLevel->setIcon(myEjectIcon);
	connect(ui->toolButton_chooseLevel, SIGNAL(clicked(bool)),
			this, SLOT(on_action_Open_Level_triggered()));
	QIcon myInfoIcon  = ImageCache::getQIcon("IconInfo", QSize(16,16));
	ui->toolButton_infoLevel->setIcon(myInfoIcon);
	connect(ui->toolButton_infoLevel, SIGNAL(clicked(bool)),
			ui->graphicsView, SLOT(slot_showGameResourcesDialog()));

    ui->graphicsView->setup(this, ui->menuBar, ui->menuControls);

	if (theIsRunAsRegression)
    {
		Q_ASSERT(theRegressionTest==nullptr);
		theRegressionTest = new RegressionTest(this);
		theRegressionTest->startRegressionRun();
    }
    else
	{
		if (theStartFileName.isEmpty())
			theStartFileName = ChooseLevel::getNextLevelName();
		// if all levels have been played, this remains empty
		if (theStartFileName.isEmpty())
			QTimer::singleShot(200, this, SLOT(on_action_Open_Level_triggered()));
		else
			QTimer::singleShot(200, this, SLOT(loadLevelDelayed()));
	}
}

