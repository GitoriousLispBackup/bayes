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

#include "networkdock.h"

#include <QLabel>

#include "network.h"

NetworkDock::NetworkDock(QWidget *parent) : QDockWidget(parent) {

    // Set title for dock - appears on many places
    setWindowTitle(tr("Network"));

    // Create stack (toggle selected/not-selected network)
    stack = new QStackedWidget(this);

    // Create stack widget when no network is selected
    QLabel *noNetLabel = new QLabel(tr("No network selected"));
    noNetLabel->setAlignment(Qt::AlignCenter);
    stack->addWidget(noNetLabel);

    // Layout of network edit form
    QFrame *selectedFrame = new QFrame();
    layout = new QFormLayout();
    selectedFrame->setLayout(layout);

    // Network name row
    nameLabel = new QLabel(tr("Network name"));
    nameEdit = new QLineEdit();
    connect(nameEdit, SIGNAL(textChanged(QString)), this, SLOT(nameChanged()));
    layout->addRow(nameLabel, nameEdit);

    // Algorithm selection and param input
    algorithmCombo = new QComboBox;
    layout->addRow(tr("Algorithm"), algorithmCombo);
    connect(algorithmCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(algorithmChanged(int)));
    algorithmParam = new QLineEdit;
    algorithmParam->setVisible(false);
    algorithmParamLabel = new QLabel(tr("N"));
    algorithmParamLabel->setVisible(false);
    layout->addRow(algorithmParamLabel, algorithmParam);

    // Edit / query button row
    editQueryBtn = new QPushButton("Query");
    connect(editQueryBtn, SIGNAL(clicked()), this, SLOT(queryEditPushed()));
    layout->addWidget(editQueryBtn);

    // Add node & edge buttons (in one row)
    QHBoxLayout *addNodeEdgeLayout = new QHBoxLayout();
    newNodeBtn = new QPushButton(tr("Add new node"));
    connect(newNodeBtn, SIGNAL(clicked()), this, SLOT(newNodePushed()));
    addNodeEdgeLayout->addWidget(newNodeBtn);
    newEdgeBtn = new QPushButton(tr("Add new edge"));
    connect(newEdgeBtn, SIGNAL(clicked()), this, SLOT(newEdgePushed()));
    addNodeEdgeLayout->addWidget(newEdgeBtn);
    layout->addRow(addNodeEdgeLayout);

    // Add stack widget when network selected
    stack->addWidget(selectedFrame);

    // Put stack as main widget
    setWidget(stack);

    // Set initial state
    toggleNetwork(NULL);
    netState = UndefinedState;
    setState(EditState);
}

/*
 * Get network state
 */
NetworkDockState NetworkDock::state() {
    return netState;
}

/*
 * Changes network state from outside
 */
void NetworkDock::setState(NetworkDockState newState) {

    // Nothinbg to do if state remains unchanged
    if ( netState == newState ) {
        return;
    }
    netState = newState;

    // Set UI for edit mode
    if ( netState == EditState ) {
        nameLabel->setEnabled(true);
        nameEdit->setEnabled(true);
        editQueryBtn->setText(tr("Query"));
        newEdgeBtn->setEnabled(true);
        newNodeBtn->setEnabled(true);

    // Set UI to query mode
    } else if ( netState == QueryState ) {
        nameLabel->setEnabled(false);
        nameEdit->setEnabled(false);
        editQueryBtn->setText(tr("Edit"));
        newEdgeBtn->setEnabled(false);
        newNodeBtn->setEnabled(false);
    }

    // Notify about change
    emit stateChanged();
}

/*
 * Toggle selected network
 */
void NetworkDock::toggleNetwork(Network *net) {
    if ( net != NULL ) {
        network = net;
        nameEdit->setText(network->name());
        stack->setCurrentIndex(1);
    } else {
        network = NULL;
        stack->setCurrentIndex(0);
    }
}

/*
 * Adds new algorithm to dock
 */
void NetworkDock::addAlgorithm(QString name, bool hasParam) {
    algorithmCombo->addItem(name, QVariant(hasParam));
}

/*
 * Gets selected algorithm name
 */
QString NetworkDock::algorithmName() {
    return algorithmCombo->currentText();
}

/*
 * Checks if selected algorithm takes param
 */
bool NetworkDock::algorithmHasParam() {
    return algorithmCombo->itemData(algorithmCombo->currentIndex()).toBool();
}

/*
 * Gets parameter for selected algorithm
 */
int NetworkDock::algorithmParamVal() {
    bool ok;
    int x = algorithmParam->text().toInt(&ok);
    return ok ? x : 0;
}

// Private SLOTS ///////////////////////////////////////////////////////////////

/*
 * Called when name edited
 */
void NetworkDock::nameChanged() {
    QString newName = nameEdit->text().trimmed();

    if ( newName.length() > 0 && network!=NULL ) {
        network->setName(newName);
        emit networkChanged();
    }
}

/*
 * Called when new node button is pushed
 */
void NetworkDock::newNodePushed() {
    emit newNode();
}

/*
 * Called when new edge button is pushed
 */
void NetworkDock::newEdgePushed() {
    emit newEdge();
}

/*
 * Called when algorithm changed
 */
void NetworkDock::algorithmChanged(int i) {
    bool v = algorithmCombo->itemData(i).toBool();

    algorithmParam->setVisible(v);
    algorithmParamLabel->setVisible(v);
}

/*
 * Called when query/edit button pushed
 */
void NetworkDock::queryEditPushed() {
    if ( netState == EditState ) {
        setState(QueryState);
    } else {
        setState(EditState);
    }
}
