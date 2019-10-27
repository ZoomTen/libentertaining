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
#include "entertaining.h"

#include "private/gamepadlistener.h"
#include <the-libs_global.h>
#include <QTranslator>

void Entertaining::initialize()
{
    Q_INIT_RESOURCE(libentertaining_resources);
    Q_INIT_RESOURCE(libentertaining_translations);
    new GamepadListener();

    QTranslator* translator = new QTranslator();
    translator->load(QLocale::system().name(), ":/libentertaining/translations/");
    QApplication::installTranslator(translator);

    QApplication::setStyle(QStyleFactory::create("contemporary"));

    //Set application palette
    QPalette pal;

    pal.setColor(QPalette::Window, QColor(40, 40, 40));
    pal.setColor(QPalette::Base, QColor(40, 40, 40));
    pal.setColor(QPalette::AlternateBase, QColor(60, 60, 60));
    pal.setColor(QPalette::WindowText, Qt::white);
    pal.setColor(QPalette::Text, Qt::white);
    pal.setColor(QPalette::ToolTipText, Qt::white);

    pal.setColor(QPalette::Disabled, QPalette::WindowText, QColor(150, 150, 150));

    pal.setColor(QPalette::Button, QColor(0, 50, 150));
    pal.setColor(QPalette::ButtonText, Qt::white);
    pal.setColor(QPalette::Highlight, QColor(0, 80, 170));
    pal.setColor(QPalette::HighlightedText, Qt::white);
    pal.setColor(QPalette::Disabled, QPalette::Button, QColor(0, 30, 100));
    pal.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(150, 150, 150));

    QApplication::setPalette(pal);
}

Entertaining::Entertaining() {

}
