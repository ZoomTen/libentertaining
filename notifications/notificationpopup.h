/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2019 Victor Tran
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * *************************************/
#ifndef NOTIFICATIONPOPUP_H
#define NOTIFICATIONPOPUP_H

#include <QWidget>

namespace Ui {
    class NotificationPopup;
}

struct NotificationData;
class NotificationPopup : public QWidget
{
        Q_OBJECT

    public:
        explicit NotificationPopup(NotificationData notification, QWidget *parent = nullptr);
        ~NotificationPopup();

    signals:
        void activatedAction(QString key);
        void dismiss();

    private slots:
        void on_dismissButton_clicked();

    private:
        Ui::NotificationPopup *ui;
        bool event(QEvent* event);
};

#endif // NOTIFICATIONPOPUP_H
