#include "SettingsManager.h"
#include "ProjectSettingsDefault.h"

void    ProjectSettingsDefault::load( const QString& part )
{
    SettingsManager::getInstance()->addNewSettingsPart( part );
    ProjectSettingsDefault::loadAudioDefaults( part );
    ProjectSettingsDefault::loadVideoDefaults( part );
    ProjectSettingsDefault::loadProjectDefaults( part );

    return ;
}

void    ProjectSettingsDefault::loadAudioDefaults( const QString& part )
{
    SettingsManager* setMan = SettingsManager::getInstance();
    QVariant defaultSampleRate = 0;
    setMan->setValue( part, "AudioSampleRate", defaultSampleRate );

    return ;
}

void    ProjectSettingsDefault::loadVideoDefaults( const QString& part )
{
    SettingsManager*  settingsMan = SettingsManager::getInstance();
    QVariant defaultProjectFPS = 25;
    QVariant defaultProjectHeight = 800;
    QVariant defaultProjectWidth = 600;

    settingsMan->setValue( part,
                           "VideoProjectFPS",
                           defaultProjectFPS );
    settingsMan->setValue( part,
                           "VideoProjectHeight",
                           defaultProjectHeight );
    settingsMan->setValue( part,
                           "VideoProjectWidth",
                           defaultProjectWidth );

    return ;
}

void    ProjectSettingsDefault::loadProjectDefaults( const QString& part )
{
    SettingsManager*  setMan = SettingsManager::getInstance();
    QVariant   defaultName = "project";
    QVariant   defaultPath = "./";

    setMan->setValue( part, "ProjectName", defaultName );
    setMan->setValue( part, "ProjectDirectory", defaultPath );

    return ;
}

