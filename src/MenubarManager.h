/*
    Theengs - Decode things and devices
    Copyright: (c) Florian ROBERT

    This file is part of Theengs.

    Theengs is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    Theengs is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MENUBAR_MANAGER_H
#define MENUBAR_MANAGER_H
/* ************************************************************************** */

#include <QObject>

class DeviceManager;

QT_FORWARD_DECLARE_CLASS(QMenu)
QT_FORWARD_DECLARE_CLASS(QAction)
QT_FORWARD_DECLARE_CLASS(QQuickWindow)

/* ************************************************************************** */

/*!
 * \brief The MenubarManager class
 */
class MenubarManager: public QObject
{
    Q_OBJECT

    QQuickWindow *m_saved_view = nullptr;
    DeviceManager *m_saved_devicemanager = nullptr;

    QMenu *m_menuSensors = nullptr;
    QAction *m_actionSensorList = nullptr;
    QAction *m_actionSensorScan = nullptr;
    QAction *m_actionSensorRefresh = nullptr;

    QMenu *m_menuHelp = nullptr;
    QAction *m_actionAbout = nullptr;
    QAction *m_actionWebsite = nullptr;
    QAction *m_actionIssueTracker = nullptr;
    QAction *m_actionReleaseNotes = nullptr;
    QAction *m_actionTutorial = nullptr;

    static MenubarManager *instance;

    MenubarManager();
    ~MenubarManager();

signals:
    void sensorsClicked();
    void settingsClicked();
    void aboutClicked();
    void tutorialClicked();

public:
    static MenubarManager *getInstance();
    void setupMenubar(QQuickWindow *view, DeviceManager *dm);

private slots:
    void sensorList();
    void sensorScan();
    void sensorRefresh();
    void settings();
    void about();
    void website();
    void issuetracker();
    void releasenotes();
    void tutorial();
};

/* ************************************************************************** */
#endif // MENUBAR_MANAGER_H