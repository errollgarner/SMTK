//=========================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//=========================================================================
// .NAME girderrequest.h
// .SECTION Description
// .SECTION See Also

#ifndef __smtk_extension_cumulus_girderrequest_h
#define __smtk_extension_cumulus_girderrequest_h

#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtNetwork/QNetworkReply>

class QNetworkAccessManager;
class QNetworkCookieJar;
class QNetworkReply;

namespace cumulus
{

class GirderRequest : public QObject
{
  Q_OBJECT

public:
  GirderRequest(const QString& girderUrl, const QString& girderToken, QObject* parent = 0);
  ~GirderRequest();

  void virtual send() = 0;

signals:
  void complete();
  void error(const QString& msg, QNetworkReply* networkReply = NULL);
  void info(const QString& msg);

protected:
  QString m_girderUrl;
  QString m_girderToken;
  QNetworkAccessManager* m_networkManager;
};

class ListItemsRequest : public GirderRequest
{
  Q_OBJECT

public:
  ListItemsRequest(const QString& girderUrl, const QString& girderToken, const QString folderId,
    QObject* parent = 0);
  ~ListItemsRequest();

  void send();

signals:
  void items(const QList<QString>& itemIds);

private slots:
  void finished(QNetworkReply* reply);
  QString folderId() const { return m_folderId; };

private:
  QString m_folderId;
};

class ListFoldersRequest : public GirderRequest
{
  Q_OBJECT

public:
  ListFoldersRequest(const QString& girderUrl, const QString& girderToken, const QString folderId,
    QObject* parent = 0);
  ~ListFoldersRequest();

  void send();

signals:
  void folders(const QMap<QString, QString>& folders);

private slots:
  void finished(QNetworkReply* reply);

private:
  QString m_folderId;
};

class ListFilesRequest : public GirderRequest
{
  Q_OBJECT

public:
  ListFilesRequest(const QString& girderUrl, const QString& girderToken, const QString itemId,
    QObject* parent = 0);
  ~ListFilesRequest();

  void send();
  QString itemId() const { return m_itemId; };
  QString path() const { return m_path; };

signals:
  void files(const QMap<QString, QString>& files);

private slots:
  void finished(QNetworkReply* reply);

private:
  QString m_itemId;
  QString m_path;
};

class DownloadFolderRequest : public GirderRequest
{
  Q_OBJECT

public:
  DownloadFolderRequest(QNetworkCookieJar* cookieJar, const QString& girderUrl,
    const QString& girderToken, const QString& downloadPath, const QString& folderId,
    QObject* parent = 0);
  ~DownloadFolderRequest();

  void send();
  QString folderId() const { return m_folderId; };
  QString downloadPath() const { return m_downloadPath; };

private slots:
  void items(const QList<QString>& itemIds);
  void folders(const QMap<QString, QString>& folders);
  void downloadItemFinished();
  void downloadFolderFinished();

private:
  QString m_folderId;
  QString m_downloadPath;
  QList<QString>* m_itemsToDownload;
  QMap<QString, QString>* m_foldersToDownload;
  QNetworkCookieJar* m_cookieJar;

  bool isComplete();
};

class DownloadFileRequest : public GirderRequest
{
  Q_OBJECT

public:
  DownloadFileRequest(QNetworkCookieJar* cookieJar, const QString& girderUrl,
    const QString& girderToken, const QString& path, const QString& fileName, const QString& fileId,
    QObject* parent = 0);
  ~DownloadFileRequest();

  void send();
  QString fileName() const { return m_fileName; };
  QString fileId() const { return m_fileId; };
  QString downloadPath() const { return m_downloadPath; };

private slots:
  void finished(QNetworkReply* reply);

private:
  QString m_fileName;
  QString m_fileId;
  QString m_downloadPath;
  QNetworkCookieJar* m_cookieJar;
  int m_retryCount;
};

class DownloadItemRequest : public GirderRequest
{
  Q_OBJECT

public:
  DownloadItemRequest(QNetworkCookieJar* cookieJar, const QString& girderUrl,
    const QString& girderToken, const QString& path, const QString& itemId, QObject* parent = 0);
  ~DownloadItemRequest();

  void send();
  QString itemId() const { return m_itemId; };
  QString downloadPath() const { return m_downloadPath; };

private slots:
  void files(const QMap<QString, QString>& fileIds);
  void fileDownloadFinish();

private:
  QString m_itemId;
  QString m_downloadPath;
  // <fileId => fileName>
  QMap<QString, QString> m_filesToDownload;
  QNetworkCookieJar* m_cookieJar;
};
}

#endif
