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

#ifndef NETWORKDOCK_H
#define NETWORKDOCK_H

#include <QDockWidget>
#include <QStackedWidget>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>

enum NetworkDockState {
    UndefinedState,
    EditState,
    QueryState
};

class Network;

class NetworkDock : public QDockWidget {
    Q_OBJECT

public:
    NetworkDock(QWidget *parent = 0);
    NetworkDockState state();

    QString algorithmName();
    bool algorithmHasParam();
    int algorithmParamVal();

signals:
    void networkChanged();
    void stateChanged();
    void newNode();
    void newEdge();

public slots:
    void toggleNetwork(Network* net);
    void setState(NetworkDockState newState);
    void addAlgorithm(QString name, bool hasParam);

private slots:
    void nameChanged();
    void newNodePushed();
    void newEdgePushed();
    void algorithmChanged(int i);
    void queryEditPushed();

private:
    NetworkDockState netState;

    QStackedWidget *stack;
    QFormLayout *layout;
    QLabel *nameLabel;
    QLineEdit *nameEdit;
    QComboBox *algorithmCombo;
    QLabel *algorithmParamLabel;
    QLineEdit *algorithmParam;
    QPushButton *editQueryBtn;
    QPushButton *newNodeBtn;
    QPushButton *newEdgeBtn;

    bool isQuery;

    Network *network;
};

#endif // NETWORKDOCK_H
