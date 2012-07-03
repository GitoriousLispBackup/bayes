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

#include "networkeditor.h"

#include <QMouseEvent>

#include "network.h"
#include "graphicsnode.h"
#include "graphicsedge.h"
#include "node.h"

/*
 * Creates new (graphical) node editor
 */
NetworkEditor::NetworkEditor(QWidget *parent) : QGraphicsView(parent) {

    // Set-up graphic display
    QGraphicsScene *scene = new QGraphicsScene(this);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    setScene(scene);
    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    setMinimumSize(400, 400);

    // Internals
    state = NE_EditState;
    fName = QString("");

    // Set-up current network
    network = new Network(this);
    isModified = false;
}

/*
 * Checks if edited network is modified
 */
bool NetworkEditor::modified() {
    return isModified;
}

/*
 * Sets file name of network
 */
void NetworkEditor::setFileName(QString name) {
    fName = name;
}

/*
 * Gets file name of network
 */
QString NetworkEditor::fileName() const {
    return fName;
}

/*
 * Outside command to add node where clicked
 */
void NetworkEditor::addNodeStart() {
    state = NE_NodeAddState;
    setCursor(Qt::CrossCursor);
    emit message(tr("Click anywhere on the screen to add node or " \
                    "press ESC to cancel!"));
}

/*
 * Outside command to add edge
 */
void NetworkEditor::addEdgeStart() {
    state = NE_EdgeAddState;
    setCursor(Qt::CrossCursor);
    emit message(tr("Click on source node or press ESC to cancel"));
}

/*
 * Outside command to stop adding
 */
void NetworkEditor::cancelAdd() {
    if ( state == NE_NodeAddState ) {
        emit message(tr("Node adding canceled."));
        setCursor(Qt::ArrowCursor);
        state = NE_EditState;

    } else if (state == NE_EdgeAddState || state == NE_EdgeAddState2 ) {
        setCursor(Qt::ArrowCursor);
        emit message(tr("Edge adding canceled."));
        state = NE_EditState;
    }
}

/*
 * Adds new named node to editor
 */
void NetworkEditor::addNode(QString name, qreal x, qreal y) {
    GraphicsNode *node = new GraphicsNode(name);
    node->setX(x);
    node->setY(y);
    scene()->addItem(node);
    node->setSelected(true);
    nodeList << node;
    connect(node, SIGNAL(evidenceChanged()), this, SIGNAL(evidenceChanged()));
}

/*
 * Sets tables for named  named node
 */
void NetworkEditor::setNodeTable(QString name, QList<double> tbl) {
    GraphicsNode *n = getByName(name);

    if ( n != NULL ) {
        n->getNode()->setTable(tbl);
    }
}

/*
 * Set list of values for named node
 */
void NetworkEditor::setNodeVals(QString name, QStringList vals) {
    GraphicsNode *n = getByName(name);

    if ( n != NULL ) {
        n->getNode()->setValueList(vals);
    }
}

/*
 * Set node X position by name
 */
void NetworkEditor::setNodeX(QString name, qreal x) {
    GraphicsNode *n = getByName(name);

    if ( n != NULL ) {
        n->setX(x);
    }
}

/*
 * Set node Y postiion bt name
 */
void NetworkEditor::setNodeY(QString name, qreal y) {
    GraphicsNode *n = getByName(name);

    if ( n != NULL ) {
        n->setY(y);
    }
}

/*
 * Called when node is changed
 */
void NetworkEditor::updateNode(bool modified) {
    GraphicsNode *n = selectedGNode();

    if ( n != NULL ) {
        n->update();

        if ( modified ) {
            isModified = true;
        }
    }

    emit nodeUpdate();
}

/*
 * Called to change modification state of network
 */
void NetworkEditor::networkModified(bool modified) {
    isModified = modified;
}

/*
 * Called when mode changed (edit / query)
 */
void NetworkEditor::queryMode(bool t) {
    if (t) {
        state = NE_QueryState;
    } else {
        state = NE_EditState;
    }

    foreach (GraphicsNode *n, nodeList) {
        n->setQueryMode(t);
    }

    update();
}

/*
 * Gets selected graphics node
 */
GraphicsNode* NetworkEditor::selectedGNode() {
    if ( state != NE_QueryState )  {
        QList<QGraphicsItem*> s = scene()->selectedItems();

        if ( s.length() > 0 ) {
            return (GraphicsNode*)s.first();
        }
    }

    return NULL;
}

/*
 * Gets selected node
 */
Node* NetworkEditor::selectedNode() {
    GraphicsNode* n = selectedGNode();

    if ( n != NULL ) {
        return n->getNode();
    }

    return NULL;
}

/*
 * Gets network this is editor for
 */
Network* NetworkEditor::getNetwork() {
    return network;
}

/*
 * Adds edge between two nodes by name
   (this method should be used carefully, only on valid networks with no
    duplicate node names - ie. ones freshly loaded from file)
 */
void NetworkEditor::addEdge(QString src, QString dest) {
    GraphicsNode *srcNode = getByName(src);
    GraphicsNode *destNode = getByName(dest);

    if ( srcNode!=NULL && destNode!=NULL ) {
        addEdge(srcNode, destNode);
        destNode->setSelected(true);
    }
}

/*
 * Adds edge between two graphical nodes
 */
bool NetworkEditor::addEdge(GraphicsNode *src, GraphicsNode *dst) {
    // Cannot have edge to self
    if ( src == dst ) {
        return false;
    }

    // Check if edge between nodes already exists
    if ( src->hasEdgeWith(dst) ) {
        return false;
    }

    // Create edge
    GraphicsEdge *edge = new GraphicsEdge(src, dst);

    // Add edge to nodes
    src->addEdge(edge);
    dst->addEdge(edge);

    // Add them to the scene
    scene()->addItem(edge);

    return true;
}

/*
 * Fetches graphics node by name
 */
GraphicsNode* NetworkEditor::getByName(QString name) {
    foreach(GraphicsNode *n, nodeList) {

        if ( n->getNode()->name() == name ) {
            return n;
        }
    }

    return NULL;
}

/*
 * Returns list of nodes
 */
QList<GraphicsNode*> NetworkEditor::nodes() const {
    return nodeList;
}

/*
 * Network editor is clicked
 */
void NetworkEditor::mouseReleaseEvent(QMouseEvent *event) {

    // Position of click
    QPointF p = mapToScene(event->pos());

    switch ( state ) {
        case NE_EditState:
            break;

        case NE_QueryState:
            break;

        // Adding new node
        case NE_NodeAddState:
            setCursor(Qt::ArrowCursor);
            addNode(tr("Untitled node"), p.x(), p.y());
            state = NE_EditState;
            emit message(tr("Added new node!"));
            break;

        // Adding edge started (nor source nor destination selected yet)
        case NE_EdgeAddState:
            state = NE_EdgeAddState2;
            edgeFirstNode = (GraphicsNode*)scene()->itemAt(p);
            emit message(tr("Now select destination node or "\
                            "press ESC to cancel"));
            break;

        // Adding edge - first one is already selected (taking second one)
        case NE_EdgeAddState2:
            state = NE_EditState;
            GraphicsNode *edgeSecondNode = (GraphicsNode*)scene()->itemAt(p);

            if ( edgeSecondNode != NULL ) {
                if ( addEdge(edgeFirstNode, edgeSecondNode) ) {
                    emit message(tr("Edge added."));
                } else {
                    emit message(tr("Illegal edge!"));
                }
                setCursor(Qt::ArrowCursor);
            }
            break;
    }

    QGraphicsView::mouseReleaseEvent(event);

    updateNode();
}


