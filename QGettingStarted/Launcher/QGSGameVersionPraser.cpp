#include <QJsonObject>
#include <QSharedPointer>

#include "QGSGameVersionPraser.h"
#include "QGSGeneralGameVersionPraseStrategy.h"

QGSGameVersionPraser::QGSGameVersionPraser(QObject * parent) :QObject(parent)
{

}

QGSGameVersionPraser::~QGSGameVersionPraser()
{
}

bool QGSGameVersionPraser::praseVersion(QGSGameVersion & version, QJsonDocument & jsonDocument) const
{
	QJsonObject object = jsonDocument.object();
	int minimumLauncherVersion =
		object.contains("minimumLauncherVersion") ?
		object.value("minimumLauncherVersion").toInt() : 0;

	QSharedPointer<QGSIGameVersionPraseStrategy>versionPraseStrategy(new QGSGeneralGameVersionPraseStrategy);

	versionPraseStrategy->praseVersion(version, jsonDocument);

	return true;
}