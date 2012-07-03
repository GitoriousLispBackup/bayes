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

#include "edgemodel.h"

#include "graphicsnode.h"
#include "graphicsedge.h"
#include "node.h"

EdgeModel::EdgeModel(GraphicsNode *n, QObject *parent)
                                                : QAbstractTableModel(parent) {
    node = n;
}

/*
 * Returns number of rows in table (number of values in node)
 */
int EdgeModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);

    return node->edges().count();
}

/*
 * Return number of columns (constant - 2)
 */
int EdgeModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);

    return 2;
}

/*
 * Returns data from each index
 */
QVariant EdgeModel::data(const QModelIndex &index, int role) const {

    // Only for display (nothing else here)
    if ( role == Qt::DisplayRole ) {
        GraphicsEdge *e = node->edges().at(index.row());

        if ( index.column() == 0 ) { // First column - describes direction
            if ( e->source() == node ) {
                return tr("To");
            } else {
                return tr("From");
            }
        } else {
            if (e->source() == node) { // Second column - other node name
                return e->destination()->getNode()->name();
            } else {
                return e->source()->getNode()->name();
            }
        }
    }

    // Generic value
    return QVariant();
}

/*
 * Determine heradres
 */
QVariant EdgeModel::headerData(int section,
                                Qt::Orientation orientation, int role) const {

    if ( role == Qt::DisplayRole ) {
        if ( orientation == Qt::Horizontal ) { // Hortizontal header
            if ( section == 0 ) { // Direction
                return tr("Direction");
            } else {
                return tr("Node");
            }
        }
    }

    // Generic value
    return QVariant();
}

/*
 * Returns flags describing each column & row
 */
Qt::ItemFlags EdgeModel::flags(const QModelIndex & index) const {
    Q_UNUSED(index);

    // All items are enabled and editable
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

/*
 * Called on row remove
 */
bool EdgeModel::removeRows(int row, int count, const QModelIndex &parent) {

    if ( count > 1 ) { // Delete only one item
        return false;
    }

    beginRemoveRows(parent, row, row);
    delete node->edges().at(row);
    endRemoveRows();

    return true;
}
