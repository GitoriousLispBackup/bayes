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

#include "valuemodel.h"

#include "node.h"

ValueModel::ValueModel(Node *n, QObject *parent) : QAbstractTableModel(parent) {
    node = n;
}

/*
 * Returns number of rows in table (number of values in node)
 */
int ValueModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);

    return node->valueList().count();
}

/*
 * Return number of columns (constant - 1)
 */
int ValueModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);

    return 1;
}

/*
 * Returns data from each index
 */
QVariant ValueModel::data(const QModelIndex &index, int role) const {

    // Only for display (nothing else here)
    if ( role == Qt::DisplayRole ) {
        return node->valueList().at(index.row());
    }

    // Generic value
    return QVariant();
}

/*
 * Determine heradres
 */
QVariant ValueModel::headerData(int section,
                                Qt::Orientation orientation, int role) const {
    Q_UNUSED(section);
    Q_UNUSED(orientation);
    Q_UNUSED(role);

    return QVariant();
}

/*
 * Returns flags describing each column & row
 */
Qt::ItemFlags ValueModel::flags(const QModelIndex & index) const {
    Q_UNUSED(index);

    // All items are enabled and editable
    return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;
}

/*
 * Called upon editing and similar actions
 */
bool ValueModel::setData(const QModelIndex & index,
                        const QVariant &value, int role) {

    // Edit value
    if ( role == Qt::EditRole ) {
        QString newVal = value.toString().trimmed();

        // Edit if new name provided
        if ( newVal.length() > 0 ) {
            node->renameValue(index.row(), newVal);
            return true;
        }
    }

    return false;
}

/*
 * Called on row insert
 */
bool ValueModel::insertRows ( int row, int count, const QModelIndex & parent) {
    if ( count > 1 ) {
        return false;
    }

    beginInsertRows(parent, row, row);
    node->addValue(tr("New value"), row);
    endInsertRows();

    return true;
}

/*
 * Called on row remove
 */
bool ValueModel::removeRows(int row, int count, const QModelIndex &parent) {

    if ( count > 1 ) {
        return false;
    }

    beginRemoveRows(parent, row, row);
    node->removeValue(row);
    endRemoveRows();

    return true;
}
