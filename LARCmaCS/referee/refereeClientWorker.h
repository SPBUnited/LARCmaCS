#pragma once

#include <QUdpSocket>
#include <QNetworkInterface>
#include <zmqpp/zmqpp.hpp>

#include "constants.h"
#include "gameState.h"
#include "refereeInfo.h"
#include "refereeUtils.h"
#include "refereeMessage.h"

struct RefereeClientWorker : public QObject
{
	Q_OBJECT

public:
	RefereeClientWorker();
	~RefereeClientWorker();

public slots:
	void start();
    void changeNetInterface(const QString & netInterface);

private slots:
	void processPendingDatagrams();

signals:
	void refereeInfoUpdate(const QSharedPointer<RefereeInfo> & refInfo);

private:
    bool open(qint16 port, const QString & netInterface);
	void close();
    QNetworkInterface getInterfaceByName(const QString &netInterface);

	static const QString hostName;
    static const QString defaultInterface;
	QUdpSocket mSocket;
    zmqpp::context context;
    zmqpp::socket socket;
    QHostAddress mGroupAddress;
	QSharedPointer<RefereeInfo> mRefInfo;

};
