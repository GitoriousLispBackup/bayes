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

#include "graphicsnode.h"

#include "node.h"
#include "graphicsedge.h"

#include <QPen>
#include <QBrush>
#include <QPainter>
#include <QFont>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsSceneMouseEvent>

/*
 * Drawing constants
 */
const qreal GraphicsNode::qModeValWidth = 80.0;
const qreal GraphicsNode::qModeValHeight = 18.0;
const qreal GraphicsNode::qModeOtherWidth = 170.0;
const qreal GraphicsNode::qModeOtherPercentWidth = 70.0;
const qreal GraphicsNode::qModeBorderWidth = 1.0;
const QColor GraphicsNode::qModeBorderColor = QColor(0, 0, 0);
const QColor GraphicsNode::qModeBackgroundColor = QColor(180, 180, 180);
const QColor GraphicsNode::qModeTextColor = QColor(0, 0, 0);
const QColor GraphicsNode::qModeRectColor0 = QColor(200, 0, 0);
const QColor GraphicsNode::qModeRectColor1 = QColor(255, 217, 0);
const QColor GraphicsNode::qModeRectColor2 = QColor(0, 200, 0);
const qreal GraphicsNode::qModeCornerR = 5.0;
const qreal GraphicsNode::padding = 10.0;
const qreal GraphicsNode::vPadding = 2.0;
const qreal GraphicsNode::radius = 30.0;
const qreal GraphicsNode::borderWidth = 1.0;
const qreal GraphicsNode::selectedBorderWidth = 2.0;
const QColor GraphicsNode::borderColor = QColor(0, 0, 0);
const QColor GraphicsNode::selectedBorderColor = QColor(0, 0, 200);
const QColor GraphicsNode::backgroundColor = QColor(220, 220, 220);
const QColor  GraphicsNode::textColor = QColor(200, 100, 100);

/*
 * Creates new graphical node
 */
GraphicsNode::GraphicsNode(QString name) {

    // Set-up graphics & internals
    setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);
    setFlag(ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache);
    queryMode = false;

    // Set-up node
    node = new Node();
    node->setName(name);
}

/*
 * Gets node instance associated with graphical node
 */
Node* GraphicsNode::getNode() {
    return node;
}

/*
 * Adds edge to list of edges conencted with this node
 */
void GraphicsNode::addEdge(GraphicsEdge* e) {
    edgeList << e;
    if ( e->destination() == this ) {
        node->addParent(e->source()->getNode());
    }
}

/*
 * Deletes edge from list of edges connected with this node
 */
void GraphicsNode::removeEdge(GraphicsEdge *e) {
    if ( e->source() == this ) {
        node->removeParent(e->destination()->getNode());
    }

    edgeList.removeOne(e);
}

/*
 * Returns list of nodes
 */
QList<GraphicsEdge*> GraphicsNode::edges() const {
    return edgeList;
}

/*
 * Set node X position
 */
void GraphicsNode::setX(double x) {
    setPos(x, pos().y());
}

/*
 * Set node Y position
 */
void GraphicsNode::setY(double y) {
    setPos(pos().x(), y);
}

/*
 * Returns list of parent nodes
 */
QList<GraphicsNode*> GraphicsNode::parentNodes() {
    QList<GraphicsNode*> l = QList<GraphicsNode*>();

    foreach ( GraphicsEdge *e, edgeList ) {
        if ( e->destination() == this ) {
            l << e->source();
        }
    }

    return l;
}

/*
 * Checks if edge between two nodes exists (in any direction)
 */
bool GraphicsNode::hasEdgeWith(GraphicsNode *n) {
    foreach (GraphicsEdge *e, edgeList) {
        if ( e->source() == n || e->destination() == n ) {
            return true;
        }
    }

    return false;
}

/*
 * Sets query mode
 */
void GraphicsNode::setQueryMode(bool t) {
    queryMode = t;
    update();
}

/*
 * Gets item bounds
 */
QRectF GraphicsNode::boundingRect() const {
    // In query mode we've got rectangle
    if ( queryMode ) {
        // Calculate total width & height
        int n = node->valueList().count() + 1; // Number of rows (+ title)
        qreal height = qModeValHeight*n + 2*padding;
        qreal width = qModeValWidth + qModeOtherWidth + 2*padding;

        // Return rectangle of that dimensions with center in (0, 0)
        return QRectF(-width/2, -height/2, width, height);

    // Otherwise we've got rectangle around circle
    } else {
        int r = radius + padding;
        return QRectF(-r, -r, 2*r, 2*r);
    }
}

/*
 * Gets radius
 */
qreal GraphicsNode::getRadius() {
    return radius + padding;
}

/*
 * Gets item shape
 */
QPainterPath GraphicsNode::shape() const {
    QPainterPath path;

    // If query mode we've got (again, same) rectangle
    if ( queryMode ) {
        // Reuse
        path.addRect(boundingRect());

    // Otherwise we've got circle
    } else {
        path.addEllipse(boundingRect());
    }

    return path;
}

/*
 * Paints (draws) node
 */
void GraphicsNode::paint(QPainter *painter,
                        const QStyleOptionGraphicsItem *option,
                        QWidget *widget) {

    // If in query mode - use helper method
    if ( queryMode ) {
        return paintQueryMode(painter, option, widget);
    }

    // Node border
    if ( isSelected() ) {
        painter->setPen(QPen(selectedBorderColor, selectedBorderWidth));
    } else {
        painter->setPen(QPen(borderColor, borderWidth));
    }

    // Node backgorund color
    painter->setBrush(QBrush(backgroundColor));

    // Draw circle from bounding box
    painter->drawEllipse(boundingRect());

    // Set pen for text
    painter->setPen(textColor);

    // Draw text in centre (herozintal and vertical)
    painter->drawText(boundingRect(),
                      Qt::AlignCenter|Qt::AlignHCenter|Qt::TextWordWrap,
                      node->name());

}
void GraphicsNode::paintQueryMode(QPainter *painter,
                                 const QStyleOptionGraphicsItem *option,
                                 QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // Rectangle
    QRectF rect = boundingRect();
    qreal width = rect.width();
    qreal height = rect.height();

    // Draw borders
    painter->setPen(QPen(qModeBorderColor, qModeBorderWidth));
    painter->setBrush(QBrush(qModeBackgroundColor));
    painter->drawRoundedRect(rect, qModeCornerR, qModeCornerR);

    // Get and set fonts
    QFont nonBoldFont = painter->font();
    QFont boldFont = painter->font();
    boldFont.setBold(true);

    // Draw title - node name
    QPen textPen = QPen(QColor(qModeTextColor));
    painter->setPen(textPen);
    painter->setFont(boldFont);
    painter->drawText(QRectF(padding-width/2, padding-height/2,
                             qModeValWidth+qModeOtherWidth+padding,
                             qModeValHeight), Qt::AlignHCenter|Qt::AlignVCenter,
                      node->name());

    // Draw values
    int n = node->valueList().length();
    int e = node->getEvidence();
    for (int i=0; i<n; ++i) {
        QString valName = node->valueList().at(i);
        double p = node->getQueryP(i);

        // Draw text
        painter->setPen(textPen);
        painter->setBrush(QColor(qModeBackgroundColor));
        painter->setFont((e==i) ? boldFont : nonBoldFont);
        painter->drawText(QRectF(padding-width/2,
                                 padding-height/2 + (i+1)*qModeValHeight,
                                 qModeValWidth, qModeValHeight),
                          Qt::AlignLeft|Qt::AlignVCenter, valName);

        // Create color base on probability
        QColor rectColor;
        if ( p < 0.2 ) {
            rectColor = qModeRectColor0;
        } else if (p >= 0.75 ) {
            rectColor = qModeRectColor2;
        } else {
            rectColor = qModeRectColor1;
        }

        // Draw percent
        painter->setFont(nonBoldFont);
        painter->setPen(QPen(rectColor));
        painter->drawText(QRectF(padding-width/2 + qModeValWidth + padding,
                                 padding-height/2 + (i+1)*qModeValHeight,
                                 qModeOtherPercentWidth, qModeValHeight),
                          Qt::AlignRight|Qt::AlignVCenter,
                          QString("%1%").arg(p * 100.0, 2, 'f', 2));

        // Draw other rectangle
        QRectF pRect = QRectF(padding-width/2 + qModeValWidth
                              + qModeOtherPercentWidth + 2*padding,
                              padding-height/2 + (i+1)*qModeValHeight +vPadding,
                              qModeOtherWidth-qModeOtherPercentWidth-2*padding,
                              qModeValHeight - 2*vPadding);
        painter->drawRect(pRect);

        // Draw inner rectangle
        if ( p > 0.0 ) {
            painter->setBrush(QBrush(rectColor));
            pRect.setWidth(pRect.width() * p);
            painter->drawRect(pRect);
        }
    }
}

/*
 * Called when node is clicked
 */
void GraphicsNode::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {

    if ( queryMode ) {
        QPointF p = event->pos() + QPoint(0, boundingRect().height()/2);

        int i = (p.y() - padding) / qModeValHeight - 1;

        int evidence = node->getEvidence();

        if ( i >= 0 ) {
            if ( evidence == i ) {
                evidence = -1;

            } else {
                evidence = i;
            }

            node->setEvidence(evidence);

            emit evidenceChanged();
        }
    }

    update();
    QGraphicsItem::mouseReleaseEvent(event);
}

/*
 * Called when graphics node has changed (moved, selected, ...)
 */
QVariant GraphicsNode::itemChange(GraphicsItemChange change,
                                  const QVariant &value) {
    switch (change) {
        case ItemPositionHasChanged:
            foreach (GraphicsEdge *edge, edgeList) {
                edge->adjust();
            }
            node->setMeta("x", pos().x());
            node->setMeta("y", pos().y());
        break;

        default:
            break;
    };

    return QGraphicsItem::itemChange(change, value);
}
