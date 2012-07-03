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

#include "graphicsedge.h"

#include "graphicsnode.h"

#include <QPainter>

// For PI
#define _USE_MATH_DEFINES
#include <math.h>

// Constants
const qreal GraphicsEdge::padding = 2.0;
const qreal GraphicsEdge::arrowSize = 10.0;
const qreal GraphicsEdge::lineWidth = 2.0;
const QColor GraphicsEdge::lineColor = QColor(0, 0, 0);

/*
 * Creates new graphical edge
 */
GraphicsEdge::GraphicsEdge(GraphicsNode *nsrc, GraphicsNode *ndst) {

    setAcceptedMouseButtons(0);
    setZValue(-1);

    src = nsrc;
    dest = ndst;

    adjust();
}

/*
 * Edge cleanup
 */
GraphicsEdge::~GraphicsEdge() {
    // Remove from both nodes
    src->removeEdge(this);
    dest->removeEdge(this);
}

/*
 * Gets reference to source graphics node
 */
GraphicsNode* GraphicsEdge::source() const {
    return src;
}

/*
 * Gets reference to destination graphics node
 */
GraphicsNode* GraphicsEdge::destination() const {
    return dest;
}

/*
 * Adjusts edge to the nodes it's pointing to
 */
void GraphicsEdge::adjust() {

    // We tell our intention to change object geometry
    prepareGeometryChange();

    // Line from source to destination
    QLineF line( mapFromItem(src, 0, 0), mapFromItem(dest, 0, 0));

    // Line has length
    if ( line.length() > 0.0 ) {
        // Calculate offset because from node center to edge
        QPointF offset( (line.dx()*src->getRadius())/line.length(),
                        (line.dy()*dest->getRadius())/line.length());

        // Apply offset to lines and calculate real points
        srcPoint = line.p1() + offset;
        destPoint = line.p2() - offset;

    // Items are on the same position
    } else {
        srcPoint = destPoint = line.p1();
    }

    // Update (redraw)
    update();
}

/*
 * Gets item bound
 */
QRectF GraphicsEdge::boundingRect() const {

    // Width & height
    qreal width = destPoint.x() - srcPoint.x();
    qreal height = destPoint.y() - srcPoint.y();

    // Offset
    qreal offset = padding + arrowSize + lineWidth;

    // Create rectangle from src point with width and height
    // "normalize" and "adjust" it
    return QRectF(srcPoint, QSizeF(width, height)).normalized()
            .adjusted(-offset, -offset, offset, offset);
}

/*
 * Draws item on scene
 */
void GraphicsEdge::paint(QPainter *painter,
                         const QStyleOptionGraphicsItem *option,
                         QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if ( srcPoint == destPoint ) {
        return;
    }

    // "Nice" line from source to destination
    QLineF niceLine(srcPoint, destPoint);
    painter->setPen(QPen(lineColor, lineWidth,
                         Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->drawLine(niceLine);

    // Draw the arrow
    // First, get angle of the line between source and destination
    double angle = acos(niceLine.dx() / niceLine.length());
    if (niceLine.dy() > 0.0 ) {
        angle = 2*M_PI - angle;
    }
    double deg60 = M_PI / 3;

    // We need two points (besides tip, which is dest) to define arrow
    // We draw them with 60 deegres  and size of arrow size
    QPointF a1 = destPoint + QPointF(sin(angle - deg60) * arrowSize,
                                     cos(angle - deg60) * arrowSize);
    QPointF a2 = destPoint + QPointF(sin(angle - M_PI + deg60) * arrowSize,
                                     cos(angle - M_PI + deg60) * arrowSize);

    // Fill inside of arrow
    painter->setBrush(QBrush(lineColor));

    // Draw polygon: tip (destPoint) -> a1 -> a2 (and finish -> destPoint)
    painter->drawPolygon(QPolygonF() << niceLine.p2() << a1 << a2);
}
