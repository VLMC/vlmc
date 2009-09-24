/*****************************************************************************
 * SettingsManager.cpp * : Backend settings manager
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

#include <QHash>

#include "SettingsManager.h"

int   SettingsManager::createNewSettings()
{
  this->m_settings.append(new SettingsContainer());
  return this->m_settings.size() - 1;
}

SettingsManager::SettingsManager( QObject* parent )
  : QObject( parent )
{
}

SettingsManager::~SettingsManager()
{
}

void  SettingsManager::saveSettings( QDomDocument& xmlfile, int index )
{
  SettingsContainer* settings = m_settings[index];
  settings->lock.lockForRead();

  //SAVE SETTINGS TO DomDocument
  settings->lock.unlock();
}
