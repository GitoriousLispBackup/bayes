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

#include "probabilitymodel.h"

#include "graphicsnode.h"
#include "graphicsedge.h"
#include "node.h"

ProbabilityModel::ProbabilityModel(GraphicsNode *n, QObject *parent)
                                                : QAbstractTableModel(parent) {
    node = n;
    parents = n->parentNodes();
    selected = false;
}

/*
 * Returns number of rows in table (number of values in node)
 */
int ProbabilityModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);

    int x = node->getNode()->valueList().count();

    foreach ( GraphicsNode *n, parents )  {
        x *= n->getNode()->valueList().count();
    }

    return x;
}

/*
 * Return number of columns (number of parents + 2)
 */
int ProbabilityModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);

    return parents.count() + 2;
}

/*
 * Returns data from each index
 */
QVariant ProbabilityModel::data(const QModelIndex &index, int role) const {

    // Only for display (nothing else here)
    if ( role == Qt::DisplayRole ) {
        if ( index.column() == (columnCount() - 1)) { // Probabilities
            return node->getNode()->probaility(index.row());

        } else { // Value of x-th node
            int n = columnCount() - 2;
            int x = index.column();
            int y = index.row();

            int i = n; // Column index

            while ( true ) {
                Node *t = (i==n) ? node->getNode() : parents.at(i)->getNode();

                if ( i == x ) {
                    return t->valueList().at(y % t->valueList().count());
                }

                y /= t->valueList().count();
                --i;
            }
        }

    // Color lbased on group it belongs to
    } else if ( role == Qt::BackgroundColorRole )  {
        int n = node->getNode()->valueList().count();
        int t1 = index.row() / n;
        int t2 = selection.row() / n;

        if ( selected && t1 == t2) {
            return QColor(200, 200, 255);
        }
    }

    // Generic value
    return QVariant();
}

/*
 * Determine heradres
 */
QVariant ProbabilityModel::headerData(int section,
                                Qt::Orientation orientation, int role) const {

    if ( role == Qt::DisplayRole ) {
        if ( orientation == Qt::Horizontal ) { // Hortizontal header
            int n = columnCount();

            if ( section == (n-1) ) { // Probability
                return tr("Probability");

            } else if (section == (n-2) ) { // Self name
                return node->getNode()->name();

            } else { // Parent name
                return parents.at(section)->getNode()->name();
            }
        } else if ( orientation == Qt::Vertical ) { // Vertical header
            return QString::number(section + 1) + ".";
        }
    }

    // Generic value
    return QVariant();
}

/*
 * Returns flags describing each column & row
 */
Qt::ItemFlags ProbabilityModel::flags(const QModelIndex & index) const {
    Q_UNUSED(index);

    // Last column is enabled and editable, others are not
    if ( index.column() == (columnCount() - 1) ) {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    }

    return 0;
}

/*
 * Called upon editing and similar actions
 */
bool ProbabilityModel::setData(const QModelIndex & index,
                        const QVariant &value, int role) {

    // Edit value
    if ( role == Qt::EditRole ) {
        if ( index.column() == (columnCount() - 1) ) {
            bool ok = false;

            double p = value.toDouble(&ok);

            if (ok && p>=0.0 && p<=1.0) {
                node->getNode()->setProbability(index.row(), p);
            }
        }
    }

    return false;
}

/*
 * Sets selected item - used for coloring part of table that has to sum
 * up to 1.0
 */
void ProbabilityModel::setSelection(QModelIndex i) {
    selected = true;
    selection = i;

    // TODO: Optimize this
    emit dataChanged(createIndex(0, 0), createIndex(rowCount(), columnCount()));
}
