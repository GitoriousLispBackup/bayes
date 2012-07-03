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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QSize>

class Settings : public QSettings {
    Q_OBJECT

public:
    static Settings* getInstance();

    // Settings getter/setter methods
    static void setMainWindowSize(QSize size);
    static QSize mainWindowSize(QSize defval);

    static void setEnginePath(QString path);
    static QString enginePath(bool expand = true);

    static void setDiffSmallValue(double val);
    static double diffSmallValue();

    static void setDiffCheckPeriod(int val);
    static int diffCheckPeriod();

    static void setSavePath(QString path);
    static QString savePath();

    static void setOpenPath(QString path);
    static QString openPath();

protected:
    Settings(QObject *parent = 0);

signals:

public slots:

private:
    static Settings *instance;

    static QString expandPath(QString path);

    // Some default values
    static QString defaultEnginePath;
};

#endif // SETTINGS_H
