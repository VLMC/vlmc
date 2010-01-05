/*****************************************************************************
 * Settings.h: generic preferences interface
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Clement CHAVANCE <kinder@vlmc.org>
 *          Ludovic Fauvet <etix@l0cal.com>
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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QString>
#include <QList>

class QDialogButtonBox;
class QLabel;
class QHBoxLayout;
class QAbstractButton;
class QScrollArea;

class Panel;
class PreferenceWidget;

class   Settings : public QDialog
{
    Q_OBJECT
    Q_DISABLE_COPY( Settings )

    public:
        Settings( bool loadDefaults = false,
                  const QString& name = "default",
                  QWidget* parent = 0,
                  Qt::WindowFlags f = 0 );
        virtual ~Settings();

        void                        addWidget( const QString& name,
                                               PreferenceWidget* pWidget,
                                               const QIcon& icon,
                                               const QString& label );
        void                        show( const QString& part = "default" );

    private:
        inline QHBoxLayout*         buildLayout();
        void                        save();

    private:
        QDialogButtonBox*           m_buttons;
        QList<PreferenceWidget*>    m_pWidgets;
        PreferenceWidget*           m_currentWidget;
        Panel*                      m_panel;
        QLabel*                     m_title;
        bool                        m_defaults;
        QString                     m_name;
        QScrollArea*                m_configPanel;


    public slots:
        void    switchWidget( int index );
        void    load();

    private slots:
        void    buttonClicked( QAbstractButton* button );

    signals:
        void    loadSettings( const QString& part, bool defaults );
};

#endif // SETTINGS_H
