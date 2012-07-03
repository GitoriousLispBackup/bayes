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

#include "settings.h"

#include <QDir>
#include <QApplication>

// Reference to (single) instance of settings
Settings* Settings::instance = 0;

// Some default values
QString Settings::defaultEnginePath = "bayes-cmd.exe";

Settings::Settings(QObject *parent) : QSettings(parent) {
    Q_UNUSED(parent);
}

/*
 * Saves main window size to settings
 */
void Settings::setMainWindowSize(QSize size) {
    getInstance()->setValue("mainwindow/size", size);
}

/*
 * Gets main window size from settings
 */
QSize Settings::mainWindowSize(QSize defval) {
    return getInstance()->value("mainwindow/size", defval).value<QSize>();
}

/*
 * Saves engine path to settings
 */
void Settings::setEnginePath(QString path) {
    getInstance()->setValue("engine/path", path);
}

/*
 * Gets engine path from settings
 */
QString Settings::enginePath(bool expand) {
    QString path =getInstance()->value("engine/path",
                                       defaultEnginePath).toString();
    return expand ? expandPath(path) : path;
}

/*
 * Set *diff-small-value* param
 */
void Settings::setDiffSmallValue(double val) {
    getInstance()->setValue("engine/small-value", val);
}

/*
 * Get *diff-small-value* param
 */
double Settings::diffSmallValue() {
    return getInstance()->value("engine/small-value", 0.0).toDouble();
}

/*
 * Set *diff-check-period* param
 */
void Settings::setDiffCheckPeriod(int val) {
    getInstance()->setValue("engine/check-period", val);
}

/*
 * Get *diff-check-period* param
 */
int Settings::diffCheckPeriod() {
    return getInstance()->value("engine/check-period", 0).toInt();
}

/*
 * Saves file save path to settings
 */
void Settings::setSavePath(QString path) {
    getInstance()->setValue("mainwindow/savepath", path);
}

/*
 * Gets file save path from settings
 */
QString Settings::savePath() {
    return getInstance()->value("mainwindow/savepath",
                                QDir::homePath()).toString();
}

/*
 * Saves file open path to settings
 */
void Settings::setOpenPath(QString path) {
    getInstance()->setValue("mainwindow/openpath", path);
}

/*
 * Gets file save path from settings
 */
QString Settings::openPath() {
    return getInstance()->value("mainwindow/openpath",
                                QDir::homePath()).toString();
}

/*
 * Expand some simple stuff in path:
 * %APP% - QApplication::applicationDirPath()
 */
QString Settings::expandPath(QString path) {
    path = path.replace("%APP%", QApplication::applicationDirPath());

    return path;
}

/*
 * Gets instance of Settings and creates one if it doesn't exist yet.
 */
Settings* Settings::getInstance() {
    if ( !instance ) {
        instance = new Settings;
    }

    return instance;
}
