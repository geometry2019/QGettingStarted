#include "QGSGameVersionJsonDownloadTask.h"

QGSGameVersionJsonDownloadTask::QGSGameVersionJsonDownloadTask(QFile * targetFile, const DownloadInfo & downloadInfo, const QNetworkProxy & proxy, QObject * parent)
	:QGSDownloadTask(targetFile, downloadInfo, proxy, parent)
{

}

QGSGameVersionJsonDownloadTask::~QGSGameVersionJsonDownloadTask()
{
	if (mTargetFilePtr)
	{
		mTargetFilePtr->deleteLater();
	}
}