#include "QGSLiteLoaderDownloadTask.h"

QGSLiteLoaderDownloadTask::QGSLiteLoaderDownloadTask(QFile * targetFile, const QGSDownloadInfo & downloadInfo, int threadCount, const QNetworkProxy & proxy, QObject * parent)
	:QGSDownloadTask(targetFile, downloadInfo, threadCount, proxy, parent)
{

}

QGSLiteLoaderDownloadTask::~QGSLiteLoaderDownloadTask()
{
}
