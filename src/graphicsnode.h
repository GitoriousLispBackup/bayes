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

#ifndef GRAPHICNODE_H
#define GRAPHICNODE_H

#include <QGraphicsObject>
#include <QObject>

class Node;
class GraphicsEdge;

class GraphicsNode :  public QGraphicsObject {
    Q_OBJECT

public:
    GraphicsNode(QString name = QString(""));

    Node *getNode();
    qreal getRadius();

    void addEdge(GraphicsEdge* e);
    void removeEdge(GraphicsEdge* e);

    bool hasEdgeWith(GraphicsNode *n);

    QList<GraphicsEdge*> edges() const;

    QList<GraphicsNode*> parentNodes();

    void setX(double x);
    void setY(double y);

    void setQueryMode(bool t);

    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

signals:
    void evidenceChanged();

protected:
    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                     QWidget *widget);
    void paintQueryMode(QPainter *painter,
                        const QStyleOptionGraphicsItem *option,
                        QWidget *widget);

    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

private:
    bool queryMode;

    Node *node;
    QList<GraphicsEdge*> edgeList;

    // Constants
    static const qreal qModeValWidth;
    static const qreal qModeValHeight;
    static const qreal qModeOtherWidth;
    static const qreal padding;
    static const qreal radius;
    static const qreal borderWidth;
    static const qreal selectedBorderWidth;
    static const QColor borderColor;
    static const QColor selectedBorderColor;
    static const QColor backgroundColor;
    static const QColor textColor;
    static const qreal qModeBorderWidth;
    static const QColor qModeBorderColor;
    static const QColor qModeBackgroundColor;
    static const qreal qModeCornerR;
    static const QColor qModeTextColor;
    static const qreal qModeOtherPercentWidth;
    static const QColor qModeRectColor0;
    static const QColor qModeRectColor1;
    static const QColor qModeRectColor2;
    static const qreal vPadding;
};

#endif // GRAPHICNODE_H
