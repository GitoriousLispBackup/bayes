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

#include "settingsdialog.h"

#include <QFormLayout>
#include <QDialogButtonBox>
#include <QMessageBox>

#include "settings.h"

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent) {

    // Layout
    QFormLayout *dialogLayout = new QFormLayout(this);

    // Add engine path item
    enginePath = new QLineEdit(this);
    enginePath->setText(Settings::enginePath(false));
    dialogLayout->addRow(tr("Engine path"), enginePath);

    // Add small value item
    diffSmallValue = new QLineEdit(this);
    diffSmallValue->setText(QString::number(Settings::diffSmallValue()));
    dialogLayout->addRow(tr("*diff-small-value*"), diffSmallValue);

    // Add check period item
    diffCheckPeriod = new QLineEdit(this);
    diffCheckPeriod->setText(QString::number(Settings::diffCheckPeriod()));
    dialogLayout->addRow(tr("*diff-check-period*"), diffCheckPeriod);

    // Add buttons
    QDialogButtonBox *buttonBox = new QDialogButtonBox(this);
    buttonBox->setStandardButtons(QDialogButtonBox::Ok |
                                  QDialogButtonBox::Cancel);
    dialogLayout->addWidget(buttonBox);

    // Connect stuff
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    // Display layout and set stuff
    setModal(true);
    setMinimumWidth(500);
    setWindowTitle(tr("Bayes-GUI settings"));
    setLayout(dialogLayout);
}

/*
 * Settings are accepted
 */
void SettingsDialog::accept() {
    bool ok;

    // Validate
    int checkPeriod = diffCheckPeriod->text().toInt(&ok);
    if ( !ok || checkPeriod < 0 ) {
        QMessageBox::critical(this, tr("Settings error"),
                              tr("*diff-check-period* should be non-negative "\
                                 "integer."));
        return;
    }

    double smallValue = diffSmallValue->text().toDouble(&ok);
    if ( !ok || smallValue < 0 ) {
        QMessageBox::critical(this, tr("Settings error"),
                              tr("*diff-small-value* should be non-negative "\
                                 "number."));
        return;
    }

    // And store
    Settings::setEnginePath(enginePath->text());
    Settings::setDiffCheckPeriod(checkPeriod);
    Settings::setDiffSmallValue(smallValue);
    QDialog::accept();
}


