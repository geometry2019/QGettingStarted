#include <QFileInfo>
#include <QCoreApplication>
#include <QDir>
#include <QMutexLocker>
#include <QMutex>

#include "QGSDownloadTask.h"
#include "QGSUuidGenerator.h"
#include "QGSOperatingSystem.h"

static const QString SEPARATOR{ QGSOperatingSystem::getInstance().getSeparator() };

QGSDownloadTask::QGSDownloadTask(QFile * targetFile, const DownloadInfo & downloadInfo, const QNetworkProxy & proxy, QObject * parent)
	:mTargetFilePtr(targetFile), mDownloadInfo(downloadInfo), mProxy(proxy), mBytesReceived(0), mDelete(false), mState(State::Stop)
{
	if (!mTargetFilePtr)
	{

	}
	mDownloadInfo.setPath(mTargetFilePtr->fileName());

}

QGSDownloadTask::~QGSDownloadTask()
{
	QMutexLocker mutexLocker{ &mMutex };

	if (mReply)
	{
		mReply->deleteLater();
	}
}

QFile * QGSDownloadTask::getTargetFile()
{
	QMutexLocker mutexLocker{ &mMutex };

	return mTargetFilePtr;
}

QGSDownloadTask::State QGSDownloadTask::getState()
{
	QMutexLocker mutexLocker{ &mMutex };

	return mState;
}

DownloadInfo QGSDownloadTask::getDownloadInfo()
{
	QMutexLocker mutexLocker{ &mMutex };

	return mDownloadInfo;
}

QString QGSDownloadTask::generateRandomFileName()
{
	return QString{ QGSUuidGenerator::generateUuid() + ".qtmp" };
}

void QGSDownloadTask::templateStart(QGSTask * task)
{
	QMutexLocker mutexLocker{ &mMutex };
	QGSNetwork network;

	if (mState == State::Start)
	{
		emit error(this);
		return;
	}
	auto request{ QGSNetwork::generateNetworkRequestWithSSL() };

	auto && url{ mDownloadInfo.getUrl() };
	if (!url.isValid()
		|| url.isEmpty()
		|| url.isLocalFile())
	{
		emit error(this);
		return;
	}

	request.setUrl(url);
	request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
	mDownloadInfo.setUrl(url);

	mTargetFilePtr->setFileName(mTargetFilePtr->fileName() + ".qtmp");
	if (!mTargetFilePtr->isOpen())
	{
		QFileInfo{ *mTargetFilePtr }.absoluteDir().mkpath("." + SEPARATOR);
		if (!mTargetFilePtr->open(QIODevice::WriteOnly | QIODevice::Truncate))
		{
			emit error(this);
			return;
		}
	}

	if (mBytesReceived)
	{
		QString strRange = QString("bytes=%1-").arg(mBytesReceived);
		request.setRawHeader("Range", strRange.toLatin1());
	}
	mReply = network.setProxy(mProxy).get(request);
	if (!mReply)
	{
		emit error(this);
		return;
	}

	connect(mReply, &QNetworkReply::downloadProgress, this, &QGSDownloadTask::slotDownloadProgress);
	connect(mReply, &QNetworkReply::finished, this, &QGSDownloadTask::slotFinished);
	connect(mReply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error), this, &QGSDownloadTask::slotError);
	connect(mReply, &QNetworkReply::sslErrors, this, &QGSDownloadTask::slotSslErrors);
	connect(mReply, &QNetworkReply::redirected, this, &QGSDownloadTask::slotRedirected);

	mState = State::Start;

	mutexLocker.unlock();

	emit started(this);
}

void QGSDownloadTask::templateStop(QGSTask * task)
{
	QMutexLocker mutexLocker{ &mMutex };

	mState = State::Stop;

	mTargetFilePtr->flush();
	mBytesReceived = mTargetFilePtr->size();
	if (mReply)
	{
		disconnect(mReply, &QNetworkReply::downloadProgress, this, &QGSDownloadTask::slotDownloadProgress);
		disconnect(mReply, &QNetworkReply::finished, this, &QGSDownloadTask::slotFinished);
		disconnect(mReply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error), this, &QGSDownloadTask::slotError);
		disconnect(mReply, &QNetworkReply::sslErrors, this, &QGSDownloadTask::slotSslErrors);
		disconnect(mReply, &QNetworkReply::redirected, this, &QGSDownloadTask::slotRedirected);
		mReply->deleteLater();
		mReply = nullptr;
	}
}

void QGSDownloadTask::templateCancel(QGSTask * task)
{
	QMutexLocker mutexLocker{ &mMutex };

	mState = State::Stop;

	mTargetFilePtr->flush();
	mTargetFilePtr->close();
	mBytesReceived = 0;
	mDelete = false;
	mTargetFilePtr->remove();
	if (mReply)
	{
		disconnect(mReply, &QNetworkReply::downloadProgress, this, &QGSDownloadTask::slotDownloadProgress);
		disconnect(mReply, &QNetworkReply::finished, this, &QGSDownloadTask::slotFinished);
		disconnect(mReply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error), this, &QGSDownloadTask::slotError);
		disconnect(mReply, &QNetworkReply::sslErrors, this, &QGSDownloadTask::slotSslErrors);
		disconnect(mReply, &QNetworkReply::redirected, this, &QGSDownloadTask::slotRedirected);
		mReply->deleteLater();
		mReply = nullptr;
	}
}

void QGSDownloadTask::downloadTemplateFinished()
{
}

void QGSDownloadTask::downloadTemplateDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
}

void QGSDownloadTask::downloadTemplateError(QNetworkReply::NetworkError code)
{
}

void QGSDownloadTask::downloadTemplateSslErrors(const QList<QSslError>& errors)
{
}

void QGSDownloadTask::downloadTemplateRedirected(const QUrl & url)
{
}

void QGSDownloadTask::slotFinished()
{
	QMutexLocker mutexLocker{ &mMutex };
	
	downloadTemplateFinished();

	mTargetFilePtr->flush();
	mTargetFilePtr->close();
	mTargetFilePtr->remove(mDownloadInfo.getPath());
	mTargetFilePtr->rename(mDownloadInfo.getPath());
	if (!mDownloadInfo.getSHA1().isEmpty())
	{

	}
	if (mDelete)
	{
		mTargetFilePtr->remove();
	}
	if (mReply)
	{
		mReply->deleteLater();
		mReply = nullptr;
	}

	mutexLocker.unlock();

	emit finished(this);
}

void QGSDownloadTask::slotDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
	QMutexLocker mutexLocker{ &mMutex };

	downloadTemplateDownloadProgress(bytesReceived, bytesTotal);

	if (mReply)
	{
		mTargetFilePtr->write(mReply->readAll());
		mTargetFilePtr->flush();
	}

	mutexLocker.unlock();

	emit downloadProgress(mBytesReceived + bytesReceived, bytesTotal, this);
}

void QGSDownloadTask::slotError(QNetworkReply::NetworkError code)
{
	QMutexLocker mutexLocker{ &mMutex };

	auto && errorString{ mReply ? mReply->errorString() : "" };
	cancel();
	downloadTemplateError(code);

	mutexLocker.unlock();

	emit downloadError(QGSNetworkError{ code,errorString }, this);
}

void QGSDownloadTask::slotSslErrors(const QList<QSslError>& errors)
{
	QMutexLocker mutexLocker{ &mMutex };

	cancel();
	downloadTemplateSslErrors(errors);

	mutexLocker.unlock();

	emit sslErrors(errors, this);
}

void QGSDownloadTask::slotRedirected(const QUrl & url)
{
	QMutexLocker mutexLocker{ &mMutex };

	downloadTemplateRedirected(url);
}
