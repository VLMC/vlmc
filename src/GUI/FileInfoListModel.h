#ifndef FILEINFOLISTMODEL_H
#define FILEINFOLISTMODEL_H

#include <QAbstractItemModel>
#include <QFileInfoList>

class FileInfoListModel : public QAbstractListModel
{
    Q_OBJECT

    public:
        FileInfoListModel( QObject* parent = 0 );
        FileInfoListModel(const QFileInfoList& fileInfoList, QObject* parent = 0 );
        int             rowCount( const QModelIndex& parent = QModelIndex() ) const;
        QVariant        data( const QModelIndex& index, int role ) const;
        QVariant        headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
        Qt::ItemFlags   flags( const QModelIndex& index ) const;
        bool            setData( const QModelIndex& index, const QVariant& value, int role = Qt::EditRole );
        void            setFileInfoList( const QFileInfoList& fileInfosList );
        QFileInfo       fileInfo( const QModelIndex& index ) const;

    private:
        QFileInfoList   m_fileInfoList;
    };

#endif // FILEINFOLISTMODEL_H
