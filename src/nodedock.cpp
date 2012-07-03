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

#include "nodedock.h"

#include <QFrame>
#include <QHeaderView>
#include <QHBoxLayout>

#include "graphicsnode.h"
#include "node.h"

#include "valuemodel.h"
#include "edgemodel.h"
#include "probabilitymodel.h"

const int NodeDock::spacing = 20;

NodeDock::NodeDock(QWidget *parent) : QDockWidget(parent) {
    // Set title for dock - appears on many places
    setWindowTitle(tr("Node"));

    // Create stack (toggle selected/not-selected node)
    stack = new QStackedWidget(this);

    // Create stack widget when no network is selected
    QLabel *noNetLabel = new QLabel(tr("No node selected"));
    noNetLabel->setAlignment(Qt::AlignCenter);
    stack->addWidget(noNetLabel);

    // Create editor and layout
    QVBoxLayout *editorLayout = new QVBoxLayout;
    QFrame *editor = new QFrame;
    editor->setLayout(editorLayout);

    // Create name edit line and delete button
    nameEdit = new QLineEdit;
    connect(nameEdit, SIGNAL(textChanged(QString)), this, SLOT(nameChanged()));
    editorLayout->addWidget(nameEdit);
    nodeDelBtn = new QPushButton(tr("Delete node"));
    editorLayout->addWidget(nodeDelBtn);
    editorLayout->addSpacing(spacing);

    // Create values list and delete button
    editorLayout->addWidget(new QLabel(tr("Values")));
    valueTable = new QTableView;
    editorLayout->addWidget(valueTable);
    QHBoxLayout *valueBtnLayout = new QHBoxLayout();
    valAddBtn = new QPushButton(tr("Add value"));
    valueBtnLayout->addWidget(valAddBtn);
    connect(valAddBtn, SIGNAL(clicked()), this, SLOT(addValue()));
    valDelBtn = new QPushButton(tr("Delete value"));
    valueBtnLayout->addWidget(valDelBtn);
    connect(valDelBtn, SIGNAL(clicked()), this, SLOT(delValue()));
    editorLayout->addLayout(valueBtnLayout);
    editorLayout->addSpacing(spacing);

    // Create edges table
    editorLayout->addWidget(new QLabel(tr("Edges")));
    edgeTable = new QTableView;
    editorLayout->addWidget(edgeTable);
    edgeDelBtn = new QPushButton(tr("Delete edge"));
    connect(edgeDelBtn, SIGNAL(clicked()), this, SLOT(delEdge()));
    editorLayout->addWidget(edgeDelBtn);
    editorLayout->addSpacing(spacing);

    // Add probability table
    editorLayout->addWidget(new QLabel(tr("Probability table")));
    probabilityTable = new QTableView;
    editorLayout->addWidget(probabilityTable);

    // Add editor to stack
    stack->addWidget(editor);

    // Put stack as main widget
    setWidget(stack);
}

/*
 * Set node which is edited
 */
void NodeDock::toggleNode(GraphicsNode *n) {
    if ( n == NULL ) { // Node is not selected (show message)
        stack->setCurrentIndex(0);

    } else { // Node is selected (show editor)

        // Store reference to node
        editNode = n;

        // Update values
        nameEdit->setText(editNode->getNode()->name());

        // Values
        ValueModel *vModel = new ValueModel(editNode->getNode());
        valueTable->setModel(vModel);
        valueTable->resizeColumnsToContents();
        valueTable->resizeRowsToContents();

        // Edges
        EdgeModel* eModel = new EdgeModel(editNode);
        edgeTable->setModel(eModel);
        edgeTable->resizeColumnsToContents();
        edgeTable->resizeRowsToContents();

        // Probability
        ProbabilityModel *pModel = new ProbabilityModel(editNode);
        probabilityTable->setModel(pModel);
        connect(probabilityTable, SIGNAL(clicked(QModelIndex)),
                pModel, SLOT(setSelection(QModelIndex)));
        probabilityTable->resizeColumnsToContents();
        probabilityTable->resizeRowsToContents();

        stack->setCurrentIndex(1);
    }
}

/*
 * Called when node name changed
 */
void NodeDock::nameChanged() {
    QString name = nameEdit->text();

    if ( name.trimmed().length() ) {
        editNode->getNode()->setName(name);
        emit nodeUpdate();
    }
}

/*
 * Called when new value button is clicked
 */
void NodeDock::addValue() {
    int i = valueTable->model()->rowCount();
    valueTable->model()->insertRow(i);
    emit nodeUpdate();
}

/*
 * Called when delete value button is clicked
 */
void NodeDock::delValue() {
    foreach(QModelIndex i, valueTable->selectionModel()->selectedRows()) {
        valueTable->model()->removeRow(i.row());
    }
    emit nodeUpdate();
}

/*
 * Called when delete edge is clicked
 */
void NodeDock::delEdge() {
    foreach ( QModelIndex i, edgeTable->selectionModel()->selectedIndexes()) {
        edgeTable->model()->removeRow(i.row());
    }
    emit nodeUpdate();
}

