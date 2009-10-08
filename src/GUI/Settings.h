/*****************************************************************************
 * Settings.h: generic preferences interface
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Clement CHAVANCE <kinder@vlmc.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

#ifndef SIMPLEPREFERENCES_H
#define SIMPLEPREFERENCES_H

#include <QDialog>
#include <QString>
#include <QHash>
#include <QLabel>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QButtonGroup>
#include <QStackedWidget>
#include <QAbstractButton>
#include <QVector>

#include "Panel.h"
#include "PreferenceWidget.h"

class   Settings : public QDialog
{
    Q_OBJECT
    Q_DISABLE_COPY( Settings )

    public:
        Settings( QWidget* parent = 0, Qt::WindowFlags f = 0 );
        virtual ~Settings();
        void                addWidget( const QString& name,
                                        PreferenceWidget* pWidget,
                                        const QString& icon,
                                        const QString& label );
        void                build();

    private:
        QVBoxLayout*    buildRightHLayout();
        void    save( void );

    private:
        QHash<int, QString>         m_widgets;
        QVector<PreferenceWidget*>  m_pWidgets;
        QWidget*                    m_currentWidget;
        Panel*                      m_panel;
        QLabel*                     m_title;
        QStackedWidget*             m_stackedWidgets;
        QDialogButtonBox*           m_buttons;
        bool                        m_saved;

    public slots:
        void    switchWidget( int widget );
        void    loadSettings();

    private slots:
        void    buttonClicked( QAbstractButton* button );

    signals:
        void    widgetSwitched( int widget );
};

#endif /* !SIMPLEPREFERENCES_H */
