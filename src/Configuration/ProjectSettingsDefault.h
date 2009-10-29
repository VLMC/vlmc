#ifndef PROJECTSETTINGSDEFAULT_H
#define PROJECTSETTINGSDEFAULT_H

class ProjectSettingsDefault
{
    public:
        static void load( void );

    private:
        static void loadAudioDefaults( void );
        static void loadVideoDefaults( void );
        static void loadProjectDefaults( void );

    private:
        ProjectSettingsDefault();
        ~ProjectSettingsDefault();
        ProjectSettingsDefault( const ProjectSettingsDefault & );
        ProjectSettingsDefault& operator = ( const ProjectSettingsDefault& );
};

#endif

