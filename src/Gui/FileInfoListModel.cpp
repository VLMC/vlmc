#include "FileInfoListModel.h"
#include "QDebug"
FileInfoListModel::FileInfoListModel( QObject* parent )
        : QAbstractListModel( parent )
{ }

FileInfoListModel::FileInfoListModel(const QFileInfoList& fileInfoList, QObject* parent )
        : QAbstractListModel( parent ), m_fileInfoList( fileInfoList )
{ }

int FileInfoListModel::rowCount( const QModelIndex& ) const
{
    return m_fileInfoList.count();
}

QVariant FileInfoListModel::data( const QModelIndex& index, int role ) const
{
    if ( !index.isValid() )
        return QVariant();
    if ( index.row() >= m_fileInfoList.size() )
        return QVariant();
    if ( role == Qt::DisplayRole )
    {
        // TODO: It's a hack that needs to be fixed
        QFileInfo fileInfo = m_fileInfoList.at( index.row() );
        return static_cast<QString>( fileInfo.fileName() );
    }
    else
        return QVariant();
}

QVariant FileInfoListModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if ( role != Qt::DisplayRole )
        return QVariant();
    if ( orientation == Qt::Horizontal )
        return QString( "Column %1" ).arg( section );
    else
        return QString( "Row %1" ).arg( section );
}

Qt::ItemFlags FileInfoListModel::flags( const QModelIndex& index ) const
{
    if ( !index.isValid() )
        return Qt::ItemIsEnabled;
    return QAbstractItemModel::flags( index ) | Qt::ItemIsEditable;
}

bool FileInfoListModel::setData( const QModelIndex& index, const QVariant& value, int role )
{
    if ( index.isValid() && role == Qt::DisplayRole )
    {
        m_fileInfoList.replace( index.row(), value.toString() );
        emit dataChanged( index, index );
        return true;
    }
    return false;
}

 void   FileInfoListModel::setFileInfoList( const QFileInfoList& fileInfoList )
 {
    m_fileInfoList = fileInfoList;
    reset();
 }

 QFileInfo  FileInfoListModel::fileInfo( const QModelIndex& index ) const
 {
    if ( index.isValid() )
    {
        return m_fileInfoList[ index.row() ];
    }
    return QFileInfo();
 }
