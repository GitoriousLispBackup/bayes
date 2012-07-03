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

#ifndef NODEDOCK_H
#define NODEDOCK_H

#include <QDockWidget>
#include <QStackedWidget>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTableView>

class GraphicsNode;

class NodeDock : public QDockWidget {
    Q_OBJECT

public:
    NodeDock(QWidget *parent = 0);

signals:
    void nodeUpdate();

public slots:
    void toggleNode(GraphicsNode* n);

private slots:
    void nameChanged();

    void addValue();
    void delValue();

    void delEdge();

private:
    QStackedWidget *stack;
    QFormLayout *layout;

    GraphicsNode* editNode;

    QLineEdit *nameEdit;
    QPushButton *nodeDelBtn;

    QTableView *valueTable;
    QPushButton *valAddBtn;
    QPushButton *valDelBtn;

    QTableView *edgeTable;
    QPushButton *edgeDelBtn;

    QTableView *probabilityTable;

    static const int spacing;
};

#endif // NODEDOCK_H
