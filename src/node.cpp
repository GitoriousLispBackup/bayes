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

#include "node.h"

#include "network.h"

/*
 * Creates new node
 */
Node::Node(QObject *parent) : QObject(parent) {
    evidence = -1;
}

/*
 * Sets node name (if non-empty)
 */
void Node::setName(QString newName) {
    if ( newName.length() > 0 ) {
        nName = newName;
    }
}

/*
 * Gets node name
 */
QString Node::name() const {
    return nName;
}

/*
 * Set list of node values
 */
void Node::setValueList(QStringList newVals) {
    values = newVals;
}

/*
 * Get list of node values
 */
QStringList Node::valueList() const {
    return values;
}

/*
 * Adds new value to node
 */
void Node::addValue(QString value, int index) {
    evidence = -1;

    if ( index >= 0 ) {
        values.insert(index, value);
    } else {
        values.append(value);
    }
    refreshTable();
    emit valueChanged();
}

/*
 * Removes values from node values
 */
void Node::removeValue(int index) {
    evidence = -1;

    values.removeAt(index);
    refreshTable();
    emit valueChanged();
}

/*
 * Removes value from node
 */
void Node::renameValue(int index, QString newName) {
    values.replace(index, newName);
}

/*
 * Gets entry from probability table
 */
double Node::probaility(int i) {
    if ( table.size() > i ) {
        return table.at(i);
    } else {
        return 0.0;
    }
}

/*
 * Sets entry in probability table
 */
void Node::setProbability(int i, double p) {
    if ( table.size() > i) {
        table[i] = p;
    } else {
        table.insert(i, p);
    }
}

/*
 * Set whole probability table
 */
void Node::setTable(QList<double> const t) {
    table = QList<double>(t);
}

/*
 * Gets whole probability table
 */
QList<double> Node::getTable() const {
    return table;
}

/*
 * Refresh empty probaility table of size n
 */
void Node::refreshTable(int n) {
    table = QList<double>();

    for ( int i=0; i<n; ++i ) {
        table << 0.0;
    }
}

/*
 * Get evidence value
 */
int Node::getEvidence() {
    return evidence;
}

/*
 * Set evidence value
 */
void Node::setEvidence(int e) {
    evidence = e;
}

/*
 *  Set probability of value - result of query
 */
void Node::setQueryP(QString value, double np) {
    int i = values.indexOf(value);

    if ( p.length() > i ) {
        p[i] = np;
    } else {
        p.insert(i, np);
    }
}

/*
 * Gets probability of value from query
 */
double Node::getQueryP(int i) {
    if ( i < p.length() ) {
        return p[i];
    }

    return 0.0;
}

/*
 * Add parent to node
 */
void Node::addParent(Node *n) {
    parents << n;
    refreshTable();
}

/*
 * Remove parent from node
 */
void Node::removeParent(Node *n) {
    parents.removeOne(n);
    refreshTable();
}

/*
 * Get list of parents
 */
QList<Node*> Node::getParents() const {
    return parents;
}

/*
 * Sets meta data
 */
void Node::setMeta(QString key, QVariant value) {
    meta[key] = value;
}

/*
 * Gets meta data
 */
QVariantHash Node::getMeta() const {
    return meta;
}
