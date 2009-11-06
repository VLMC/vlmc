#ifndef PROJECTSETTINGSDEFAULT_H
#define PROJECTSETTINGSDEFAULT_H

class ProjectSettingsDefault
{
    public:
        static void load( const QString& part );

    private:
        static void loadAudioDefaults( const QString& part );
        static void loadVideoDefaults( const QString& part );
        static void loadProjectDefaults( const QString& part );

    private:
        ProjectSettingsDefault();
        ~ProjectSettingsDefault();
        ProjectSettingsDefault( const ProjectSettingsDefault & );
        ProjectSettingsDefault& operator = ( const ProjectSettingsDefault& );
};

#endif

