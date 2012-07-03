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

#ifndef EDGEMODEL_H
#define EDGEMODEL_H

#include <QAbstractTableModel>

class GraphicsNode;

class EdgeModel : public QAbstractTableModel {
    Q_OBJECT

public:
    EdgeModel(GraphicsNode *n, QObject *parent = 0);

protected:
    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section,
                        Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex & index) const ;
    bool removeRows(int row, int count, const QModelIndex &parent);

signals:

public slots:

private:
    GraphicsNode *node;
};

#endif // EDGEMODEL_H
