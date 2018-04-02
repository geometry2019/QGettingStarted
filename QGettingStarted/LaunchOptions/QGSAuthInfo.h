#pragma once

#include <QString>
#include <QMetaType>

namespace UserType
{
	const QString Mojang("Mojang");
	const QString Legacy("Legacy");
}

class QGSAuthInfo
{
public:
	class QGSProfile
	{
	public:
		QGSProfile(const QString & id = "{}", const QString name = "{}", const bool legacy = false);

		QGSProfile(const QGSProfile & right) = default;

		QGSProfile(QGSProfile && right) = default;

		QGSProfile & operator=(const QGSProfile & right) = default;

		QGSProfile & operator=(QGSProfile && right) = default;

		~QGSProfile();

		QGSProfile & setId(const QString & id);

		QGSProfile & setName(const QString & name);

		QGSProfile & setId(const bool legacy);

		QString getId()const;

		QString getName()const;

		bool isLegacy()const;

	private:
		QString mId;
		QString mName;
		bool mLegacy;
	};
public:
	QGSAuthInfo(const QString & accessToken = "{}",
		const QString & clientToken = "{}",
		const QString & userType = UserType::Legacy,
		const QGSProfile & selectedProfile = QGSProfile(),
		const QString & twitchAccessToken = "{}");
	
	~QGSAuthInfo();

	QGSAuthInfo & setAccessToken(const QString & accessToken);
	QGSAuthInfo & setClientToken(const QString & clientToken);
	QGSAuthInfo & setUserType(const QString & userType);
	QGSAuthInfo & setSelectedProfile(const QGSProfile & selectedProfile);
	QGSAuthInfo & setTwitchAccessToken(const QString & twitchAccessToken);

	QString getAccessToken()const;
	QString getClientToken()const;
	QString getUserType()const;
	QGSProfile getSelectedProfile()const;
	QString getTwitchAccessToken()const;

private:
	QString mAccessToken;
	QString mClientToken;
	QString mUserType;
	QGSProfile mSelectedProfile;
	QString mTwitchAccessToken;
};

Q_DECLARE_METATYPE(QGSAuthInfo::QGSProfile)
Q_DECLARE_METATYPE(QGSAuthInfo)