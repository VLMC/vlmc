/*****************************************************************************
 * VideoPage.cpp: Wizard page for configuring video settings
 *****************************************************************************
 * Copyright (C) 2008-2009 the VLMC team
 *
 * Authors: Ludovic Fauvet <etix@l0cal.com>
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

#include "VideoPage.h"
#include "SettingsManager.h"

VideoPage::VideoPage( QWidget *parent ) :
    QWizardPage( parent )
{
    ui.setupUi( this );

    setTitle( tr( "New project wizard" ) );
    setSubTitle( tr( "Configure Video settings" ) );

    setFinalPage( true );

    connect( ui.comboBoxVideoPresets, SIGNAL( currentIndexChanged(int) ),
             this, SLOT( updateVideoPresets() ) );
    connect( ui.comboBoxAudioPresets, SIGNAL( currentIndexChanged(int) ),
             this, SLOT( updateAudioPresets() ) );
}

void VideoPage::changeEvent( QEvent *e )
{
    QWizardPage::changeEvent( e );
    switch ( e->type() )
    {
    case QEvent::LanguageChange:
        ui.retranslateUi( this );
        break;
    default:
        break;
    }
}

int VideoPage::nextId() const
{
    return -1;
}

void VideoPage::initializePage()
{
    SettingsManager* sManager = SettingsManager::getInstance();

    int projectFps = sManager->getValue(    "default",
                                            "VideoProjectFPS" )->get().toDouble();
    int projectHeight = sManager->getValue( "default",
                                            "VideoProjectHeight" )->get().toInt();
    int projectWidth = sManager->getValue(  "default",
                                            "VideoProjectWidth" )->get().toInt();
    int sampleRate = sManager->getValue(      "default",
                                            "AudioSampleRate" )->get().toInt();

    ui.comboBoxVideoPresets->setCurrentIndex( 0 );
    ui.comboBoxAudioPresets->setCurrentIndex( 0 );
    ui.spinBoxVideoWidth->setValue( projectWidth );
    ui.spinBoxVideoHeight->setValue( projectHeight );
    ui.doubleSpinBoxVideoFPS->setValue( projectFps );

    switch ( sampleRate )
    {
    case 48000:
        ui.comboBoxAudioSamplerate->setCurrentIndex( HZ_48000 );
        break;
    case 22000:
        ui.comboBoxAudioSamplerate->setCurrentIndex( HZ_22000 );
        break;
    case 11000:
        ui.comboBoxAudioSamplerate->setCurrentIndex( HZ_11000 );
        break;
    case 44000:
    default:
        ui.comboBoxAudioSamplerate->setCurrentIndex( HZ_44000 );
        break;
    }

}

bool VideoPage::validatePage()
{
    SettingsManager* sManager = SettingsManager::getInstance();
    QVariant    projectFps( ui.doubleSpinBoxVideoFPS->value() );
    QVariant    projectHeight( ui.spinBoxVideoHeight->value() );
    QVariant    projectWidth( ui.spinBoxVideoWidth->value() );

    sManager->setValue( "project", "VideoProjectFPS", projectFps );
    sManager->setValue( "project", "VideoProjectHeight", projectHeight );
    sManager->setValue( "project", "VideoProjectWidth", projectWidth);
    sManager->commit();

    return true;
}

void VideoPage::cleanupPage()
{

}

void VideoPage::setVideoFormEnabled( bool enabled )
{
    ui.spinBoxVideoWidth->setEnabled( enabled );
    ui.spinBoxVideoHeight->setEnabled( enabled );
}

void VideoPage::setAudioFormEnabled( bool enabled )
{
    ui.spinBoxAudioChannels->setEnabled( enabled );
    ui.comboBoxAudioSamplerate->setEnabled( enabled );
}

void VideoPage::updateVideoPresets()
{
    if ( ui.comboBoxVideoPresets->currentIndex() == 0 )
        setVideoFormEnabled( true );
    else
        setVideoFormEnabled( false );

    switch ( ui.comboBoxVideoPresets->currentIndex() )
    {
    case PRESET_VideoCustom: break;
    case PRESET_480i:
        setVideoResolution( 720, 480 );
        break;
    case PRESET_576i:
        setVideoResolution( 720, 576 );
        break;
    case PRESET_480p:
        setVideoResolution( 720, 480 );
        break;
    case PRESET_576p:
        setVideoResolution( 720, 576 );
        break;
    case PRESET_720p:
        setVideoResolution( 1280, 720 );
        break;
    case PRESET_1080i:
        setVideoResolution( 1920, 1080 );
        break;
    case PRESET_1080p:
        setVideoResolution( 1920, 1080 );
        break;
    }
}

void VideoPage::updateAudioPresets()
{
    if ( ui.comboBoxAudioPresets->currentIndex() == 0 )
        setAudioFormEnabled( true );
    else
        setAudioFormEnabled( false );

    switch ( ui.comboBoxAudioPresets->currentIndex() )
    {
    case PRESET_AudioCustom: break;
    case PRESET_STEREO:
        ui.spinBoxAudioChannels->setValue( 2 );
        break;
    case PRESET_MONO:
        ui.spinBoxAudioChannels->setValue( 1 );
        break;
    }
}

void VideoPage::setVideoResolution( int width, int height )
{
    ui.spinBoxVideoWidth->setValue( width );
    ui.spinBoxVideoHeight->setValue( height );
}
