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

#ifndef NETWORKEDITOR_H
#define NETWORKEDITOR_H

#include <QGraphicsView>

class Network;
class GraphicsNode;
class Node;

enum NetworkEditorState {
    NE_EditState,
    NE_QueryState,
    NE_NodeAddState,
    NE_EdgeAddState,
    NE_EdgeAddState2
};

class NetworkEditor : public QGraphicsView {
    Q_OBJECT

public:
    NetworkEditor(QWidget *parent = 0);
    bool modified();
    Node* selectedNode();
    GraphicsNode *selectedGNode();
    Network* getNetwork();
    GraphicsNode *getByName(QString name);
    QList<GraphicsNode*> nodes() const;

    QString fileName() const;
    void setFileName(QString name);

protected:
    void mouseReleaseEvent(QMouseEvent *event);

signals:
    void message(QString);
    void nodeUpdate();
    void evidenceChanged();

public slots:
    void addNodeStart();
    void addEdgeStart();
    void cancelAdd();

    void addNode(QString name, qreal x=0.0, qreal y=0.0);
    void setNodeVals(QString name, QStringList vals);
    void setNodeTable(QString name, QList<double>);
    void addEdge(QString src, QString dest);
    bool addEdge(GraphicsNode *src, GraphicsNode *dst);

    void updateNode(bool modified=true);
    void networkModified(bool modified);

    void setNodeX(QString name, qreal x);
    void setNodeY(QString name, qreal y);

    void queryMode(bool t);

private:

    NetworkEditorState state;
    Network *network;
    bool isModified;

    GraphicsNode* edgeFirstNode;

    QList<GraphicsNode*> nodeList;

    QString fName;
};

#endif // NETWORKEDITOR_H
