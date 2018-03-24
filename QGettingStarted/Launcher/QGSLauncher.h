#pragma once

#include <QSharedPointer>

#include "QGSILauncherStrategy.h"
#include "QGSGameDirectory.h"

class QGSLauncher
{
public:
	QGSLauncher();

	QGSLauncher(const QGSLauncher & right) = delete;

	QGSLauncher(QGSLauncher && right) = delete;

	QGSLauncher & operator=(const QGSLauncher & right) = delete;

	QGSLauncher & operator=(QGSLauncher && right) = delete;

	~QGSLauncher();

	virtual LauncherError::ErrorFlags generateLaunchCommand(const QGSGameVersion & version, QGSGameDirectory & gameDirectory, const QGSLaunchOptions * launchOptions, QString & command);
private:

};

