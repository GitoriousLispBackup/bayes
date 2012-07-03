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

#ifndef GRAPHICEDGE_H
#define GRAPHICEDGE_H

#include <QGraphicsItem>

class GraphicsNode;

class GraphicsEdge : public QGraphicsItem {

public:
    GraphicsEdge(GraphicsNode *nsrc, GraphicsNode *ndst);
    ~GraphicsEdge();

    GraphicsNode* source() const;
    GraphicsNode* destination() const;
    void adjust();

protected:
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                     QWidget *widget);

private:
    GraphicsNode *src;
    GraphicsNode *dest;

    QPointF srcPoint;
    QPointF destPoint;

    static const qreal padding;
    static const qreal arrowSize;
    static const qreal lineWidth;
    static const QColor lineColor;
};

#endif // GRAPHICEDGE_H
