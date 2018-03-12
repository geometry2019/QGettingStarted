#pragma once

#include <QMap>

#include "QGSIDownloadSource.h"

class QGSDownloadSourceBMCLAPI : public QGSIDownloadSource
{
	Q_OBJECT

public:
	QGSDownloadSourceBMCLAPI(QObject *parent = nullptr);

	QGSDownloadSourceBMCLAPI(const QGSDownloadSourceBMCLAPI & right) = delete;

	QGSDownloadSourceBMCLAPI(QGSDownloadSourceBMCLAPI && right) = delete;

	QGSDownloadSourceBMCLAPI & operator=(const QGSDownloadSourceBMCLAPI & right) = delete;

	QGSDownloadSourceBMCLAPI & operator=(QGSDownloadSourceBMCLAPI && right) = delete;

	virtual ~QGSDownloadSourceBMCLAPI();

	virtual QUrl getLibraryUrlBase(QUrl & originUrl)override;

	virtual QUrl getVersionManifestUrl()override;

	virtual QUrl getVersionUrlBase(QUrl & originUrl)override;

	virtual QUrl getAssetIndexUrlBase(QUrl & originUrl)override;

	virtual QUrl getAssetUrlBase(QUrl & originUrl)override;

	virtual QUrl getApiUrlBase(const QString & apiName)override;
private:
	QMap<QString, QUrl> mApiUrlMap;
};
