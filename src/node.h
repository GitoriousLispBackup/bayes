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

#ifndef NODE_H
#define NODE_H

#include <QObject>
#include <QList>
#include <QStringList>
#include <QVariantHash>

class Node : public QObject {
    Q_OBJECT

public:
    Node(QObject *parent = 0);

    void setName(QString newName);
    QString name() const;

    void setValueList(QStringList);
    QStringList valueList() const;

    void addValue(QString value, int index=-1);
    void removeValue(int index);
    void renameValue(int index, QString newName);

    double probaility(int i);
    void setProbability(int i, double p);

    void setTable(QList<double> const t);
    void refreshTable(int n = 0);
    QList<double> getTable() const;

    int getEvidence();
    void setEvidence(int e);

    void setQueryP(QString value, double p);
    double getQueryP(int i);

    void addParent(Node *n);
    void removeParent(Node *n);
    QList<Node*> getParents() const;

    void setMeta(QString key, QVariant value);
    QVariantHash getMeta() const;

signals:
    void valueChanged();

public slots:

private:
    QString nName;
    QList<QString> values;
    QList<Node*> parents;
    QList<double> table;
    QVariantHash meta;

    int evidence;
    QList<double> p;
};

#endif // NODE_H
