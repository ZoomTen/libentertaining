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
#ifndef ACCOUNTDIALOG_H
#define ACCOUNTDIALOG_H

#include <libentertaining_global.h>
#include <QWidget>

namespace Ui {
    class AccountDialog;
}

struct AccountDialogPrivate;
class LIBENTERTAINING_EXPORT AccountDialog : public QWidget
{
        Q_OBJECT

    public:
        explicit AccountDialog(QWidget *parent = nullptr);
        ~AccountDialog();

    private slots:
        void on_logOutButton_clicked();

        void on_backButton_clicked();

        void on_setup2faButton_clicked();

        void on_changeUsernameButton_clicked();

        void on_changePasswordButton_clicked();

        void on_resendVerificationButton_clicked();

        void on_enterVerificationButton_clicked();

        void on_changeEmailButton_clicked();

    signals:
        void done();

    private:
        Ui::AccountDialog *ui;
        AccountDialogPrivate* d;

        void loadData();
};

#endif // ACCOUNTDIALOG_H
