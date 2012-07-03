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

#include "networkeditortabs.h"

#include "networkeditor.h"
#include "network.h"
#include "node.h"

#include <QTabBar>

NetworkEditorTabs::NetworkEditorTabs(QWidget *parent) : QTabWidget(parent) {

    // Set basic settings
    setTabsClosable(true);
    setMovable(true);
}

/*
 * Creates new (named) tab
 */
void NetworkEditorTabs::createTab() {

    // Create new editor
    NetworkEditor *editor = new NetworkEditor(this);
    connect(editor, SIGNAL(message(QString)),
            this, SLOT(messageReceived(QString)));
    connect(editor, SIGNAL(nodeUpdate()), this, SLOT(nodeUpdate()));
    connect(editor, SIGNAL(evidenceChanged()), this, SLOT(evidenceChange()));

    // Create tab
    int newIndex = addTab(editor, "");
    setCurrentIndex(newIndex);
    setTabTitle();
}

/*
 * Get current selected network (editor)
 */
NetworkEditor* NetworkEditorTabs::currentNetwork() {
    return (NetworkEditor*) currentWidget();
}

/*
 * Sets current tab name
 */
void NetworkEditorTabs::setTabTitle() {
    NetworkEditor *t = currentNetwork();

    if ( t != NULL ) {
        QString title = t->getNetwork()->name();
        /*if ( t->modified() ) {
            title += "*";
        }*/

        setTabText(currentIndex(), title);
    }
}

/*
 * Get selected node from children
 */
GraphicsNode* NetworkEditorTabs::selectedNode() {
    NetworkEditor *t = currentNetwork();

    if ( t != NULL ) {
        return currentNetwork()->selectedGNode();
    }

    return NULL;
}

/*
 * Called when node is updated (generated from children)
 */
void NetworkEditorTabs::nodeUpdate() {
    setTabTitle();
    emit networkChange();
}

/*
 * Called when node is changed (generated from outside)
 */
void NetworkEditorTabs::nodeChanged() {
    NetworkEditor *t = currentNetwork();

    if ( t != NULL ) {
        t->updateNode();
    }
}

/*
 * Call ed when network has changed (from outside)
 */
void NetworkEditorTabs::networkChanged() {
    NetworkEditor *t = currentNetwork();

    if ( t != NULL ) {
        t->networkModified(true);
    }

    setTabTitle();
}

/*
 * Enables/disables bar
 */
void NetworkEditorTabs::enableBar(bool onOff) {
    tabBar()->setEnabled(onOff);
}

/*
 * Called when one of nodes from editor has changed evidence
 */
void NetworkEditorTabs::evidenceChange() {
    emit evidenceChanged();
}

/*
 * Message is received from one of children
 */
void NetworkEditorTabs::messageReceived(QString msg) {
    emit message(msg);
}
