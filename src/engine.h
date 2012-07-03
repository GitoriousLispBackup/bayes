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

#ifndef ENGINE_H
#define ENGINE_H

#include <QProcess>
#include <QVariantList>

#include "sexp.h"

class Node;

class Engine : public QObject {
    Q_OBJECT

public:
    Engine(QObject *parent = 0);
    ~Engine();

    void loadFile(QString fielName);
    void algorithms();
    void loadNetwork(QString name, QList<Node*>);
    void query(QVariantList l);
    void saveFile(QString fileName);

    void setOption(QString name, QVariant value);

protected:

signals:
    void command(QString cmd, QStringList args, bool rawArg=false);

public slots:
    void exit();

private slots:
    void dataReady();
    void errorReady();

private:
    void commandReceived(sexp_t* cmd);
    void command(QString cmd, QVariantList args = QVariantList());
    QString esc(QString s);
    QString toArg(QVariantList l);

    QProcess *process;

    sexp_t *sexp;  // Temp S-Expression
    pcont_t *cont; // Continuation help
};

#endif // ENGINE_H
