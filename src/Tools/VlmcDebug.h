/*****************************************************************************
 * VlmcDebug.h: Debug tools for VLMC
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Hugo Beauzee-Luyssen <hugo@vlmc.org>
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

#ifndef VLMCDEBUG_H
#define VLMCDEBUG_H

#include <QObject>
#include <QDebug>
#include <QFile>

#include "Singleton.hpp"

/**
 *  \warning    Do not use qDebug() qWarning() etc... from here, unless you know exactly what you're doing
 *              Chances are very high that you end up with a stack overflow !!
 */
class   VlmcDebug : public QObject, public Singleton<VlmcDebug>
{
    Q_OBJECT

    public:
        static void     vlmcMessageHandler( QtMsgType type, const char* msg );
        void            setup();
    private:
        VlmcDebug();
        virtual ~VlmcDebug();

        QFile*          m_logFile;

    private slots:
        void            logFileChanged( const QVariant& logFile );

        friend class    Singleton<VlmcDebug>;
};

#endif // VLMCDEBUG_H
