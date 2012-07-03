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

#ifndef NETWORKEDITORTABS_H
#define NETWORKEDITORTABS_H

#include <QTabWidget>

class GraphicsNode;

class NetworkEditor;

class NetworkEditorTabs : public QTabWidget {
    Q_OBJECT

public:
    NetworkEditorTabs(QWidget *parent = 0);
    NetworkEditor* currentNetwork();

    GraphicsNode* selectedNode();

signals:
    void message(QString);
    void networkChange();
    void evidenceChanged();

public slots:
    void setTabTitle();
    void createTab();
    void messageReceived(QString);
    void nodeChanged();
    void networkChanged();
    void evidenceChange();
    void enableBar(bool);

private slots:
    void nodeUpdate();

private:

};

#endif // NETWORKEDITORTABS_H
