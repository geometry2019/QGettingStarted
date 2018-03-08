#pragma once

#include <QDir>
#include <QFile>

#include "../Version/Version.h"

class QGSGameDirectory 
{
public:
	QGSGameDirectory(const QDir & baseDir);

	QGSGameDirectory(QDir && baseDir);

	QGSGameDirectory(const QGSGameDirectory & right) = default;

	QGSGameDirectory(QGSGameDirectory && right) = default;

	QGSGameDirectory & operator=(const QGSGameDirectory & right) = default;

	QGSGameDirectory & operator=(QGSGameDirectory && right) = default;

	~QGSGameDirectory();

	const Version & getVersion(const QString & version);

	const Version & addVersion(const QString version);

	bool containsVersion(const QString & version)const;

	QFile * generateVersionJarFile(const QString & version)const;

	QFile * generateLibraryFile(const Library & library)const;

	QDir generateNativesDirectory(const QString & version)const;

	QDir getBaseDir()const;
	
	/*need to be updated*/
	QDir generateAssetDirectory(const QString & version, const AssetIndex & assetIndex);
private:
	void init();

private:
	QMap<QString, Version> mVersionMap;
	QDir mBaseDir;
};

