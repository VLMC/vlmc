/*****************************************************************************
 * Library.h: Multimedia library
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

/** \file
 * This file the library contains class declaration/definition.
 * It's the the backend part of the Library widget of vlmc.
 * It can load and unload Clips (Clip.h/Clip.cpp)
 */

#ifndef LIBRARY_H
#define LIBRARY_H

#include "Singleton.hpp"

#include <QHash>
#include <QObject>
#include <QUuid>

class QDomDocument;
class QDomElement;

class Clip;
class Media;

/**
 *  \class Library
 *  \brief Library Object that handles medias
 */
class Library : public QObject, public Singleton<Library>
{
    Q_OBJECT
    Q_DISABLE_COPY( Library );
public:
    /**
     *  \brief  returns the media that match the unique identifier
     *  \param  uuid    the unique identifier of the media
     *  \return a pointer to the required media, or NULL if no medias matches
     *  \sa     getClip( const QUuid& uuid )
     */
    Media*  getMedia( const QUuid& uuid );
    /**
     *  \brief  returns the clip that match the unique identifier
     *  \param  uuid    the unique identifier of the media
     *  \return a pointer to the required clip, or NULL if no clips matches
     *  \sa     getMedia( const QUuid& uuid )
     */
    Clip*   getClip( const QUuid& uuid );
    /**
     *  \brief  Add the media with already computed metadatas to the library
     *  \param  media The media to add.
     *  \sa     addClip( Clip* clip )
     *  \sa     getMedia( const QUuid& uuid)
     *  \sa     getClip( const QUuid& uuid )
     */
    void    addMedia( Media* media );
    /**
     *  \brief  Add the clip to the library
     *  \param  clip The clip to add.
     *  \sa     addMedia( Media* media )
     *  \sa     getMedia( const QUuid& uuid)
     *  \sa     getClip( const QUuid& uuid )
     */
    void    addClip( Clip* clip );
    /**
     *  \brief  Delete the media.
     *  \param  the media to delete
     *  \warning this method does not seem to remove the media from the library
     */
    void    deleteMedia( Media* media );

private:
    /**
     *  \brief Library Constructor
     */
    Library();
    /**
     *  \brief  Tells the media has already been loaded into library or not
     *  \param  filePath    The path of the media file
     *  \return true if the media is already loaded, false otherwhise
     */
    bool                    mediaAlreadyLoaded( const QString& filePath );
    /**
     *  \brief  This method is used to load a media directly from it's
     *          path, with a specified UUID.
     *          It shouldn't used for something else that loading a project file
     *  \param  path The path of the media file
     *  \param  uuid The uuid you want for the new media
     */
    void                    loadMedia( const QString& path, const QUuid& uuid );

    /**
     *  \brief The List of medias loaded into the library
     */
    QHash<QUuid, Media*>    m_medias;
    /**
     *  \brief The List of clips loaded into the library
     *  \warning This list should be removed to used clips existing inside medias
     */
    QHash<QUuid, Clip*>     m_clips;
    /**
     *  \brief The list of media being queued for deletion
     */
    QList<Media*>           m_mediaToDelete;
    /**
     *  \brief  This method allows to get whereas Media or clip by uuid
     *  \param container The type of container used for storage, where T is Clip or Media
     *  \param uuid The uuid of the element you are looking for
     */
    template <typename T>
    T                       getElementByUuid( const QHash<QUuid, T>& container ,
                                              const QUuid& uuid )
    {
        typename QHash<QUuid, T>::const_iterator   it = container.find( uuid );
        if ( it == container.end() )
            return NULL;
        return *it;
    }

public slots:
    /**
     *  \brief This slot must be called when you want a new media to be loaded
     *  \param  filePath The path of the media
     *  \param  uuid    The uuid of the newly created media, by default create a new one
     */
    void                    newMediaLoadingAsked( const QString& filePath,
                                                  const QString& uuid = QString() );
    /**
     *  \brief This slot must be called when you want a media to be removed from library
     *  \param uuid The uuid of the media to be removed
     */
    void                    removingMediaAsked( const QUuid& uuid );

    /**
     *  \brief  Load the medias contained in the project to the library
     *  \param  project The project file dom element
     *  \sa saveProject( QDomDocument& doc, QDomElement& rootNode )
     */
    void                    loadProject( const QDomElement& project );
    /**
     *  \brief  Save the medias contained in the library to the project
     *  \param  doc  The project dom document
     *  \param  rootNode    The rootNode of the project
     */
    void                    saveProject( QDomDocument& doc, QDomElement& rootNode );
    /**
     *  \brief  Clear the library (remove all the loaded media and delete them)
     */
    void                    clear();
private slots:
    /**
     *  \brief      This seems to be a crappy function that create a clip from media
     *  \param      media   The media freshly loaded
     *  \warning    What the fuck?!!! What is the purpose of that Method?
     *              Clips should be handled into Medias
     */
    void                    metaDataComputed( Media* media );
    /**
     *  \brief  This slot should be call when the audio spectrum of a Media has been
     *          Computed
     *  \param  media   The media which have a computed audio spectrum
     */
    void                    audioSpectrumComputed( Media* media );

signals:
    /**
     *  \brief This signal should be emitted to begin metadata computing
     *  \param media    The media you want to compute the metadata
     */
    void                    metadataRequired( Media* media );
    /**
     *  \brief This signal should be emiteted to tell a new media have been loaded
     *  \param media The newly loaded media
     */
    void                    newMediaLoaded( Media* media );
    /**
     *  \brief This signal should be emiteted when a media has been removed
     *  \param uuid The removed media uuid
     */
    void                    mediaRemoved( const QUuid& );
    /**
     *  \brief This signal should be emitted when the project has been loaded
     */
    void                    projectLoaded();

    friend class            Singleton<Library>;
};

#endif // LIBRARY_H
