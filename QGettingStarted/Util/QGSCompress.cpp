#include "QGSCompress.h"

QGSCompress::QGSCompress()
{
}

QGSCompress & QGSCompress::getInstance()
{
	static QGSCompress instance;
	return instance;
}

QGSCompress::~QGSCompress()
{
}

bool QGSCompress::compressDirectory(QString & file, QString & directory, bool recursive)
{
	bool ret = true;

	try
	{
		ret = JlCompress::compressDir(file, directory);
	}
	catch (...)
	{
		ret = false;
	}

	return ret;
}

QStringList QGSCompress::extractDirectory(QString & file, QString & directory)
{
	try
	{
		return JlCompress::extractDir(file, directory);
	}
	catch (...)
	{
		throw QGSExceptionCompress(file, directory);
	}
}