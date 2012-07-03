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

#include "engine.h"

#include "settings.h"

#include <stdio.h>
#include <QApplication>

#include "node.h"

/*
 * Inits communication witj Lisp engine
 */
Engine::Engine(QObject *parent) : QObject(parent) {

    // Internal S-Expression parser stuff
    sexp = NULL;
    cont = NULL;

    // Set up process
    process = new QProcess(this);
    QString enginePath = Settings::enginePath();

    // Setup communication
    connect(process, SIGNAL(readyReadStandardOutput()),
            this, SLOT(dataReady()));
    connect(process, SIGNAL(readyReadStandardError()),
            this, SLOT(errorReady()));

    // Start process
    //process->setWorkingDirectory(QApplication::applicationDirPath()
    //                                                    + "/../src/lisp/");
    //printf("wdir: %s\n", process->workingDirectory().toUtf8().data());
    process->start(enginePath);
    process->waitForStarted(-1);
}

/*
 * Destroys communication with Lisp engine and cleans internal stuff
 */
Engine::~Engine() {

    exit();

    if ( cont != NULL ) {
        destroy_continuation(cont);
    }

    if ( sexp != NULL ) {
        destroy_sexp(sexp);
    }
}

// COMMANDS ////////////////////////////////////////////////////////////////////

/*
 * Sends exit command to engine
 */
void Engine::exit() {
    command("quit");
    process->waitForFinished(-1);
}

/*
 * Sends command to load file
 */
void Engine::loadFile(QString fileName) {
    QVariantList args = QVariantList();
    args << fileName << true;
    command("load-file", args);
}

/*
 * Sends command to save file
 */
void Engine::saveFile(QString fileName) {
    QVariantList args = QVariantList();
    args << fileName;
    command("save-file", args);
}

/*
 * Sends command to list algorithms
 */
void Engine::algorithms() {
    command("algorithms");
}

/*
 * Send query command
 */
void Engine::query(QVariantList l) {
    command("query", l);
}

/*
 * Sets named option to value
 */
void Engine::setOption(QString name, QVariant value) {
    QVariantList args;
    args << name << value;
    command("set-option", args);
}

/*
 * Sends command to load network
 */
void Engine::loadNetwork(QString name, QList<Node*> nodes) {
    // Here we have to build arguments manually because of complexity
    QVariantList args;

    QVariantList net;
    net << QVariant() << "network" << QVariant() << ":name" << name;
    args << QVariant(net);

    foreach (Node *n, nodes) {
        QVariantList node;
        node << QVariant() << "node" << QVariant() << ":name" << n->name();

        QVariantList vals;
        foreach(QString val, n->valueList()) {
            vals << val;
        }
        node << QVariant() << ":vals" << QVariant(vals);

        QVariantList parents;
        foreach (Node *p, n->getParents()) {
            parents << p->name();
        }
        node << QVariant() << ":parents" << QVariant(parents);

        QVariantList table;
        foreach ( double d, n->getTable() ) {
            table << d;
        }
        node << QVariant() << ":table" << QVariant(table);

        QVariantList m;
        QVariantHash meta = n->getMeta();
        foreach (QString name, meta.keys()) {
            m << QVariant() << (":"+name) << meta[name];
        }
        node << QVariant() << ":meta" << QVariant(m);

        args << QVariant(node);
    }

    printf("Network cmd:\n%s\n", toArg(args).toUtf8().data());

    command("load-network", args);
}

/*
 * Sends command named `cmd' and list of `args' to engine
 */
void Engine::command(QString cmd, QVariantList args) {
    // TODO: Check if engine is running and probablly restart it?

    QString t = QString("");

    // Start building command string
    t += "(" + esc(cmd) + " " + toArg(args) + ")\n";
    printf("-> %s\n", t.toUtf8().data());

    // Send command
    process->write(t.toUtf8().data());
}

/*
 * Creates arguments from list
 */
QString Engine::toArg(QVariantList l) {
    QString t = QString("");
    bool symNext = false;

    foreach (QVariant v, l) {
        t += " ";

        if (symNext) {
            symNext = false;
            t += v.toString();

        } else if (v.type() == QVariant::Int) {
            t += QString::number(v.toInt());

        } else if ( v.type() == QVariant::Double ) {
            t += QString::number(v.toDouble());

        } else if ( v.type() == QVariant::List ) {
            t += "(" + toArg(v.toList()) + ")";

        } else if ( v.isNull() ) {
            symNext = true;

        } else if ( v.type() == QVariant::Bool ) {
            t += (v.toBool() ? "T" : "NIL");

        } else {
            t += "\"" + esc(v.toString()) + "\"";

        }
    }

    return t;
}

/*
 * Escapes string to be sent to engine
 *
 */
QString Engine::esc(QString s) {
    s = s.replace("\\", "\\\\");
    s = s.replace("\"", "\\\"");

    return s;
}

/*
 * Analyses whole command received from engine
 */
void Engine::commandReceived(sexp_t* cmd) {
    QStringList vals = QStringList();

    while ( cmd ) {
        if ( cmd->ty == SEXP_LIST ) {
            cmd = cmd->list;

        } else if ( cmd->ty == SEXP_VALUE ) {
            vals.append(QString(cmd->val));
            cmd = cmd->next;

        } else {
            return;
        }
    }

    if ( vals.empty() ) {
        return;
    }

    QString cmds = vals.first().toLower();
    vals.pop_front();

    //printf("Got commdand: %s\n", cmds.toUtf8().data());
    //printf("With args: %s\n\n", vals.join(", ").toUtf8().data());

    emit command(cmds, vals);
}

// SLOTS ///////////////////////////////////////////////////////////////////////

/*
 * Private slot called when data on stdout of process is ready
 */
void Engine::dataReady() {

    // Fetch data and information
    QByteArray ba = process->readAllStandardOutput();
    int len = ba.length();
    char *data = ba.data();

    // Output data
    //printf("[out] %s\n", data);
    printf("<- %s\n", data);

    // Init continuation
    if ( cont == NULL ) {
        cont = init_continuation(data);
    }

    // Start parsings S-expression(s)
    while ( true ) {

        // Try to get one expression
        sexp = (sexp_t*) iparse_sexp(data, len, cont);

        // If one not available yet - stop parsing
        if ( ! sexp ) {
            break;
        }

        // We have a command available
        commandReceived(sexp);
        destroy_sexp(sexp);
    }
}

/*
 * Private slot called when data om stderr of process is ready
 */
void Engine::errorReady() {
    char *data = process->readAllStandardError().data();
    fprintf(stderr, "[err] %s\n", data);
    //emit error(data);
}
