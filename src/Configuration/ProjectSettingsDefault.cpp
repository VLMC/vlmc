#include "SettingsManager.h"
#include "ProjectSettingsDefault.h"

void    ProjectSettingsDefault::load( void )
{
    SettingsManager::getInstance()->addNewSettingsPart( "default" );
    ProjectSettingsDefault::loadAudioDefaults();
    ProjectSettingsDefault::loadVideoDefaults();
    ProjectSettingsDefault::loadProjectDefaults();

    return ;
}

void    ProjectSettingsDefault::loadAudioDefaults( void )
{
    QVariant defaultSampleRate = 0;
    //SettingsManager::getInstance()->setValue( "AudioSampleRate", defaultSampleRate );

    return ;
}

void    ProjectSettingsDefault::loadVideoDefaults( void )
{
    SettingsManager*  settingsMan = SettingsManager::getInstance();
    QVariant defaultProjectFPS = 25;
    QVariant defaultProjectHeight = 800;
    QVariant defaultProjectWidth = 600;

    settingsMan->setValue( "default",
                           "VideoProjectFPS",
                           defaultProjectFPS );
    settingsMan->setValue( "default",
                           "VideoProjectHeight",
                           defaultProjectHeight );
    settingsMan->setValue( "default",
                           "VideoProjectWidth",
                           defaultProjectWidth );

    return ;
}

void    ProjectSettingsDefault::loadProjectDefaults( void )
{
    SettingsManager*  settingsMan = SettingsManager::getInstance();
    QVariant   defaultName = "project";
    QVariant   defaultPath = "./";

    //settingsMan->setValue( "ProjectName", defaultName );
    //settingsMan->setValue( "ProjectDirectory", defaultPath );

    return ;
}

