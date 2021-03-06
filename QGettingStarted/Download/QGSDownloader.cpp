#include <QTimer>
#include <QEventLoop>

#include "QGSDownloader.h"
#include "../Util/QGSExceptionInvalidValue.h"

QGSDownloader::QGSDownloader(
	QFile * targetFile, 
	const QGSDownloadInfo & downloadInfo,
	QGSNetworkHelper * networkAccessManagerPtr, 
	qint64 bytesBegin, 
	qint64 bytesEnd, 
	const QNetworkProxy & proxy,
	QObject *parent)

	: QObject(parent), 
	mTargetFilePtr(targetFile), 
	mDownloadInfo(downloadInfo),
	mNetworkHelperPtr(networkAccessManagerPtr), 
	mBytesBegin(bytesBegin), 
	mBytesEnd(bytesEnd), 
	mReply(nullptr),
	mProxy(proxy),
	mBytesReceived(0)
{
	if (!mTargetFilePtr ||
		!mNetworkHelperPtr)
	{
		QGSExceptionInvalidValue();
	}

	if (mBytesBegin < 0)
	{
		mBytesBegin = 0;
	}

	if (mBytesEnd < 0)
	{
		mBytesEnd = 0;
	}
}

QGSDownloader::~QGSDownloader()
{
}

QNetworkReply * QGSDownloader::getNetworkReply() const
{
	return mReply;
}

void QGSDownloader::start()
{
	auto && request(QGSNetworkHelper::generateHttpsNetworkRequest());

	request.setUrl(mDownloadInfo.getUrl());
	request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
	request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
	request.setRawHeader(QByteArray("Connection"), QByteArray("Keep-Alive"));
	request.setRawHeader(QByteArray("Keep-Alive"), QByteArray("Timeout=60"));

	if (mBytesBegin >= 0 && mBytesEnd >= 0 && (mBytesBegin < mBytesEnd))
	{
		QString strRange = QString("bytes=%1-%2").arg(mBytesBegin + mBytesReceived).arg(mBytesEnd);
		request.setRawHeader("Range", strRange.toLatin1());
	}

	mNetworkHelperPtr->getNetworkAccessManager()->setProxy(mProxy);
	mReply = mNetworkHelperPtr->getNetworkAccessManager()->get(request);
	if (!mReply)
	{
		emit downloadError(QGSNetworkError(QNetworkReply::NetworkError::UnknownNetworkError,"Unknown download task error!"), this);
		return;
	}

	connect(mReply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error), this, &QGSDownloader::slotDownloadError, Qt::ConnectionType::DirectConnection);
	connect(mReply, &QNetworkReply::sslErrors, this, &QGSDownloader::slotSslErrors, Qt::ConnectionType::DirectConnection);
	connect(mReply, &QNetworkReply::redirected, this, &QGSDownloader::slotRedirected, Qt::ConnectionType::DirectConnection);

	if (!get())
	{
		if (mReply)
		{
			mReply->disconnect();
			mReply->deleteLater();
			mReply = nullptr;
		}

		emit downloadError(QGSNetworkError(QNetworkReply::NetworkError::UnknownNetworkError,"Unknown download task error!"), this);
	}

	emit started(this);
}

void QGSDownloader::stop()
{
	mTargetFilePtr->flush();
	mBytesReceived = mTargetFilePtr->size();

	if (mReply)
	{
		mReply->disconnect();
		mReply->deleteLater();
		mReply = nullptr;
	}
}

void QGSDownloader::cancel()
{
	mTargetFilePtr->flush();
	mBytesReceived = 0;

	if (mReply)
	{
		mReply->disconnect();
		mReply->deleteLater();
		mReply = nullptr;
	}
}

void QGSDownloader::slotFinished()
{
	mTargetFilePtr->flush();
	mBytesReceived = 0;

	if (mReply)
	{
		mReply->disconnect();
		mReply->deleteLater();
		mReply = nullptr;
	}

	emit finished(this);
}

void QGSDownloader::slotDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
	auto && data(mReply->readAll());

	mTargetFilePtr->seek(mBytesBegin + mBytesReceived);
	mTargetFilePtr->write(data);

	mBytesReceived += data.size();

	emit downloadProgress(data.size(), bytesReceived, bytesTotal, this);
}

void QGSDownloader::slotDownloadError(QNetworkReply::NetworkError _error)
{
	auto && errorString(mReply ? mReply->errorString() : "Unknown error string!");
	cancel();

	emit downloadError(QGSNetworkError(_error,errorString), this);
}

void QGSDownloader::slotSslErrors(const QList<QSslError>& errors)
{
	cancel();

	emit sslErrors(errors, this);
}

void QGSDownloader::slotRedirected(const QUrl & url)
{
	emit redirected(url, this);
}

bool QGSDownloader::get()
{
	QTimer *timer(nullptr);
	QEventLoop eventLoop;
	bool readTimeOut(false);
	const int timeOutms(30000);

	if (timeOutms > 0)
	{
		timer = new QTimer(this);

		connect(timer, &QTimer::timeout, [&readTimeOut]()
		{
			readTimeOut = true;
		});
		connect(timer, &QTimer::timeout, &eventLoop, &QEventLoop::quit);
		timer->setSingleShot(true);
	}

	connect(mNetworkHelperPtr->getNetworkAccessManager(), &QNetworkAccessManager::finished, &eventLoop, &QEventLoop::quit);

	if (!mReply)
	{
		timer->start(timeOutms);
		eventLoop.exec();
	}

	if (mReply)
	{
		connect(mReply, &QNetworkReply::downloadProgress, this, &QGSDownloader::slotDownloadProgress, Qt::ConnectionType::DirectConnection);
		connect(mReply, &QNetworkReply::finished, this, &QGSDownloader::slotFinished, Qt::ConnectionType::DirectConnection);
	}

	if (timer)
	{
		timer->stop();
		timer->deleteLater();
		timer = nullptr;
	}

	return !readTimeOut;
}
