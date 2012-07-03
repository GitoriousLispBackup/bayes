/*
    Copyright (C) 2012 Ivan Radicek

    This file is part of Bayes.

    Bayes is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Bayes is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Bayes.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "mainwindow.h"

#include <QStatusBar>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QKeyEvent>
#include <QFileDialog>

#include "settings.h"
#include "networkdock.h"
#include "nodedock.h"
#include "networkeditortabs.h"
#include "networkeditor.h"
#include "engine.h"
#include "network.h"
#include "graphicsnode.h"
#include "node.h"
#include "settingsdialog.h"

// Reference to (single) instance of MainWindow
MainWindow* MainWindow::instance = 0;

/*
 * Sets everything on application main window:
 * - Main menu and submenus
 * - Docks
 * - Tabs
 * ...
 */
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    // Main window title
    setWindowTitle(tr(MAIN_WINDOW_TITLE));

    // Create and set components
    createTabs();
    createDocks();
    createMenus();
    createEngine();
    readSettings();

    // Settings dialog
    settingsDialog = new SettingsDialog(this);

    // Reset status flags
    loadingFile = savingFile = query = false;
}

/*
 * Creates central widget - tabs
 */
void MainWindow::createTabs() {
    // Create
    NetworkEditorTabs *tabs = new NetworkEditorTabs(this);
    connect(tabs, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
    connect(tabs, SIGNAL(message(QString)), this, SLOT(showMessage(QString)));
    connect(tabs, SIGNAL(networkChange()), this, SLOT(networkChanged()));
    connect(tabs, SIGNAL(evidenceChanged()), this, SLOT(doQuery()));
    connect(tabs, SIGNAL(tabCloseRequested(int)), this, SLOT(tabClose(int)));

    // Set as main (central) widget
    setCentralWidget(tabs);
}

/*
 * Creates docks
 */
void MainWindow::createDocks() {
    networkDock = new NetworkDock(this);
    addDockWidget(Qt::LeftDockWidgetArea, networkDock);
    connect(networkDock, SIGNAL(newNode()), this, SLOT(dockAddNode()));
    connect(networkDock, SIGNAL(newEdge()), this, SLOT(dockAddEdge()));
    connect(networkDock, SIGNAL(networkChanged()),
            tabs(), SLOT(networkChanged()));
    connect(networkDock, SIGNAL(stateChanged()),
            this, SLOT(dockStateChanged()));

    nodeDock = new NodeDock(this);
    connect(nodeDock, SIGNAL(nodeUpdate()), tabs(), SLOT(nodeChanged()));
    addDockWidget(Qt::LeftDockWidgetArea, nodeDock);
}

/*
 * Creates menus and submenus in menu bar
 */
void MainWindow::createMenus() {

    // File menu //////////////////////////////////////////////////////////////
    QMenu *file = menuBar()->addMenu(tr("&File"));

    // File -> New action (menu)
    QAction *fileNew = new QAction(tr("&New"), this);
    fileNew->setShortcuts(QKeySequence::New);
    connect(fileNew, SIGNAL(triggered()), this, SLOT(fileNew()));
    file->addAction(fileNew);

    // File -> Open
    QAction *fileOpen = new QAction(tr("&Open..."), this);
    fileOpen->setShortcuts(QKeySequence::Open);
    connect(fileOpen, SIGNAL(triggered()), this, SLOT(fileOpen()));
    file->addAction(fileOpen);

    // Separator
    file->addSeparator();

    // File -> Save
    QAction *fileSave = new QAction(tr("&Save"), this);
    fileSave->setShortcuts(QKeySequence::Save);
    connect(fileSave, SIGNAL(triggered()), this, SLOT(fileSave()));
    file->addAction(fileSave);

    // File -> Save As
    QAction *fileSaveAs = new QAction(tr("Save &As..."), this);
    fileSaveAs->setShortcuts(QKeySequence::SaveAs);
    connect(fileSaveAs, SIGNAL(triggered()), this, SLOT(fileSaveAs()));
    file->addAction(fileSaveAs);

    // Separator
    file->addSeparator();

    // File -> Settings
    QAction *fileSettings = new QAction(tr("Settings."), this);
    connect(fileSettings, SIGNAL(triggered()), this, SLOT(fileSettings()));
    file->addAction(fileSettings);

    // Separator
    file->addSeparator();

    // File -> Exit
    QAction *fileExit = new QAction(tr("E&xit"), this);
    fileExit->setShortcuts(QKeySequence::Quit);
    connect(fileExit, SIGNAL(triggered()), this, SLOT(close()));
    file->addAction(fileExit);

    // View menu ///////////////////////////////////////////////////////////////
    QMenu *view = menuBar()->addMenu(tr("&View"));

    // View items
    view->addAction(networkDock->toggleViewAction());
    view->addAction(nodeDock->toggleViewAction());

    // Help menu ///////////////////////////////////////////////////////////////
    QMenu *help = menuBar()->addMenu(tr("&Help"));

    // Help -> About
    QAction *helpAbout = new QAction(tr("About"), this);
    connect(helpAbout, SIGNAL(triggered()), this, SLOT(helpAbout()));
    help->addAction(helpAbout);
}

/*
 * Creates engine communication
 */
void MainWindow::createEngine() {
    engine = new Engine(this);
    connect(engine, SIGNAL(command(QString,QStringList)),
            this, SLOT(engineCmd(QString,QStringList)));
    engine->algorithms();
}

/*
 * Clean exit: networks & settings are saved
 */
void MainWindow::closeEvent(QCloseEvent *event) {
    // Quiot engine
    engine->exit();

    saveSettings();
    QMainWindow::closeEvent(event);
}

/*
 * Key is pressed
 */
void MainWindow::keyReleaseEvent ( QKeyEvent * event ) {
    NetworkEditor *e = tabs()->currentNetwork();
    if ( event->key() == Qt::Key_Escape && e!=NULL ) {
        e->cancelAdd();
        return;
    }

    QMainWindow::keyReleaseEvent(event);
}

/*
 * Captures some aspects of main application state and saves it in settings
 */
void MainWindow::saveSettings() {
    Settings::setMainWindowSize(size());
}

/*
 * Reads state of main window from settings and applies it
 */
void MainWindow::readSettings() {
    resize(Settings::mainWindowSize(size()));
}



/*******************************************************************************
 * Private Slots & helpers
 ******************************************************************************/

/*
 * Shows message in status bar
 */
void MainWindow::showMessage(QString message) {
    statusBar()->showMessage(message);
}

/*
 * Returns tabs widget
 */
NetworkEditorTabs* MainWindow::tabs() {
    return (NetworkEditorTabs*)centralWidget();
}

/*
 * Creates new tab and sets new network
 */
void MainWindow::createNewNetwork(QString fromFile) {

    // Create tab
    tabs()->createTab();

    // Load file
    if ( fromFile.length() > 0 ) {
        setEnabled(false);
        loadingFile = true;
        engine->loadFile(fromFile);
    }
}

/*
 * Creates new network
 */
void MainWindow::fileNew() {
    createNewNetwork();
}

void MainWindow::fileOpen() {
    QString directory = Settings::openPath();

    QString fileName =
            QFileDialog::getOpenFileName(this,
                                         tr("Open existing Bayes network"),
                                         directory,
                                         tr("Bayes network (*.net);;"\
                                            "All files (*.*)"));
    if ( fileName.isEmpty() ) {
        return;
    }

    Settings::setOpenPath(QDir(fileName).absolutePath());
    createNewNetwork(fileName);

    tabs()->currentNetwork()->setFileName(fileName);
}

void MainWindow::fileSave() {
    NetworkEditor *e = tabs()->currentNetwork();

    if ( e != NULL ) {
        if ( e->fileName().length() > 0 ) {
            setEnabled(false);
            savingFile = true;
            defineNetwork();
            engine->saveFile(e->fileName());
            Settings::setSavePath(QDir(e->fileName()).absolutePath());

        } else {
            fileSaveAs();
        }
    }
}

void MainWindow::fileSaveAs() {
    NetworkEditor *e = tabs()->currentNetwork();

    if ( e != NULL ) {
        QString directory;
        if ( e->fileName().isEmpty()) {
            directory = Settings::savePath();
        } else {
            directory = e->fileName();
        }

        QString fileName =
                QFileDialog::getSaveFileName(this,
                                             tr("Save Bayes network"),
                                             directory,
                                             tr("Bayes network (*.net);;"\
                                                "All files (*.*)"));

        if ( fileName.length() > 0 ) {
            e->setFileName(fileName);
            fileSave();
        }
    }
}

void MainWindow::helpAbout() {
    QString message = tr("Bayes Copyright (C) 2012  Ivan Radicek\n\n"\
			 "This program comes with ABSOLUTELY NO WARRANTY.\n" \
			 "This is free software, and you are welcome to redistribute it\n"
			 "under certain conditions; see COPYING file for details.");
    QMessageBox::about(this, tr("About Bayes-GUI"), message);
}

void MainWindow::fileSettings() {
    settingsDialog->show();
}

/*
 * Command from dock to add node
 */
void MainWindow::dockAddNode() {
    tabs()->currentNetwork()->addNodeStart();
}

/*
 * Command from dock to add edge
 */
void MainWindow::dockAddEdge() {
    tabs()->currentNetwork()->addEdgeStart();
}

/*
 * State of dock changed
 */
void MainWindow::dockStateChanged() {
    if ( networkDock->state() == EditState ) {
        query = false;
        tabs()->currentNetwork()->queryMode(false);
        tabs()->setEnabled(true);
        menuBar()->setEnabled(true);
        tabs()->enableBar(true);

    } else {
        menuBar()->setEnabled(false);
        tabs()->enableBar(false);
        tabs()->currentNetwork()->queryMode(true);
        query = true;
        doQuery(true);
    }
}

/*
 * Makes query to engine
 */
void MainWindow::doQuery(bool createNet) {

    query = true;
    setEnabled(false);
    tabs()->setEnabled(false);

    if ( createNet ) {
        defineNetwork();

        double smallValue = Settings::diffSmallValue();
        if ( smallValue > 0 ) {
            engine->setOption("diff-small-value", smallValue);
        }

        int checkPeriod = Settings::diffCheckPeriod();
        if ( checkPeriod > 0 ) {
            engine->setOption("diff-check-period", checkPeriod);
        }
    }

    QVariantList queryArgs;
    queryArgs << networkDock->algorithmName();
    if ( networkDock->algorithmHasParam() ) {
        queryArgs << networkDock->algorithmParamVal();
    }

    foreach (GraphicsNode *n, tabs()->currentNetwork()->nodes() ) {
        Node *node = n->getNode();

        int e = node->getEvidence();
        if ( e != -1 ) {
            QVariantList evidence;
            evidence << node->name() << node->valueList().at(e);
            queryArgs << QVariant(evidence);
        }
    }

    engine->query(queryArgs);
}

/*
 * Defines network to engine
 */
void MainWindow::defineNetwork() {
    QList<Node*> nodes;

    foreach(GraphicsNode *n, tabs()->currentNetwork()->nodes()) {
        nodes << n->getNode();
    }

    engine->loadNetwork(tabs()->currentNetwork()->getNetwork()->name(), nodes);
}

/*
 * Selected tab has changed
 */
void MainWindow::tabChanged(int index) {
    if ( index >= 0 ) {
        networkDock->toggleNetwork(tabs()->currentNetwork()->getNetwork());

    } else {
        networkDock->toggleNetwork(NULL);
    }
}

/*
 * Tab should be closed ("X" has been clicked)
 */
void MainWindow::tabClose(int index) {
    tabs()->removeTab(index);
}

/*
 * Selected network has changed
 */
void MainWindow::networkChanged() {
    nodeDock->toggleNode(tabs()->selectedNode());
}

/*
 * Command from engine received
 */
void MainWindow::engineCmd(QString cmd, QStringList args) {

    // Info message from engine - show status
    if ( cmd == "info" && args.length()==1 ) {
        showMessage(args.first());

    // Set loading file network name
    } else if ( cmd == "network-name" && args.length() == 1 ) {
        if ( loadingFile ) {
            tabs()->currentNetwork()->getNetwork()->setName(args.first());
        }

    // Set loading file new node name
    } else if ( cmd == "node-name" && args.length() == 1 ) {
        if ( loadingFile ) {
            tabs()->currentNetwork()->addNode(args.first());
        }

    // Set loading file node mta data
    } else if ( cmd == "node-meta" && args.length() == 3 ) {
        if ( args.at(1) == ":X" ) {
            tabs()->currentNetwork()->setNodeX(args.at(0),
                                               args.at(2).toDouble());

        } else if ( args.at(1) == ":Y" ) {
            tabs()->currentNetwork()->setNodeY(args.at(0),
                                                args.at(2).toDouble());
        }

    // Set loading file node values
    } else if ( cmd == "node-vals" && args.length() >= 3 ) {
        if ( loadingFile ) {
            QString name = args.first();
            args.pop_front();

            for (int i=1; i<args.length(); ++i) {
                tabs()->currentNetwork()->setNodeVals(name, args);
            }
        }

    } else if ( cmd == "node-table" && args.length() >= 3 ) {
        QList<double> tbl = QList<double>();

        for(int i=1; i<args.length(); ++i) {
            tbl << args.at(i).toDouble();
        }

        tabs()->currentNetwork()->setNodeTable(args.first(), tbl);

    // Adds new algorithm to list of algorithms
    } else if ( cmd == "add-algorithm" && args.length() == 2 ) {
        QString name = args.first();
        bool hasParam = args.at(1) != "NIL";
        networkDock->addAlgorithm(name, hasParam);

    // Set loading file node parent
    } else if ( cmd == "node-parent" && args.length() == 2 ) {
        tabs()->currentNetwork()->addEdge(args.at(1), args.at(0));

    // File loading is done
    } else if ( cmd == "load-file-done" ) {
        loadingFile = false;
        setEnabled(true);
        tabChanged(tabs()->currentIndex());

    // Query result
    } else if ( cmd == "setval" && args.length() == 3 ) {
        GraphicsNode *node = tabs()->currentNetwork()->getByName(args.at(0));

        if ( node != NULL ) {
            Node *n = node->getNode();

            n->setQueryP(args.at(1), args.at(2).toDouble());
            node->update();
        }

    // Query is done
    } else if ( cmd == "query-done" ) {
        query = false;
        setEnabled(true);
        tabs()->setEnabled(true);

    // Saving file is done
    } else if ( cmd == "file-save-done" ) {
        savingFile = false;
        setEnabled(true);

    // Error occured
    } else if ( cmd == "error" ) {
        QString err = args.join(" ");
        if ( loadingFile ) {
            QMessageBox::critical(this, tr("Error loading network"),
                                  tr("Loading error: ") + err);
            tabs()->removeTab(tabs()->currentIndex());
            loadingFile = false;

        } else if ( savingFile ) {
            QMessageBox::critical(this, tr("Error saving network"),
                                  tr("Saving error: ") + err);
            savingFile = false;

        } else if ( query ) {
            QMessageBox::critical(this, tr("Query error"),
                                  tr("Query error: ") + err);
            networkDock->setState(EditState);
            dockStateChanged();
        }

        setEnabled(true);

    // Unknown command - show error msg
    } else {
        if ( loadingFile ) {
            tabs()->removeTab(tabs()->currentIndex());
            loadingFile = false;

        } else if ( savingFile ) {
            savingFile = false;

        } else if ( query ) {
            networkDock->setState(EditState);
            dockStateChanged();
        }

        QString m = tr("Unknown command: ") + cmd +
                                    " (" + QString::number(args.length()) + ")";
        QMessageBox::critical(this, tr("Engine error"), m);

        setEnabled(true);
    }
}
