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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>

class NetworkDock;
class NodeDock;
class NetworkEditorTabs;
class Engine;
class SettingsDialog;

#define MAIN_WINDOW_TITLE "Bayes GUI"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);

protected:
    void closeEvent(QCloseEvent *event);
    void keyReleaseEvent ( QKeyEvent * event );

signals:

public slots:
    void showMessage(QString message);
    void engineCmd(QString cmd, QStringList args);

private slots:
    void fileNew();
    void fileOpen();
    void fileSave();
    void fileSaveAs();
    void fileSettings();

    void helpAbout();

    void dockAddNode();
    void dockAddEdge();
    void dockStateChanged();

    void tabChanged(int index);
    void tabClose(int index);
    void networkChanged();

    void doQuery(bool createNet=false);

private:
    static MainWindow *instance;

    void createTabs();
    void createDocks();
    void createMenus();
    void createEngine();

    void saveSettings();
    void readSettings();

    void defineNetwork();

    NetworkEditorTabs *tabs();
    void createNewNetwork(QString fromFile = QString(""));

    NetworkDock *networkDock;
    NodeDock *nodeDock;

    Engine *engine;

    SettingsDialog *settingsDialog;

    // Status flags
    bool loadingFile;
    bool savingFile;
    bool query;
};

#endif // MAINWINDOW_H
