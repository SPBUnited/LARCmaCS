// Copyright 2019 Dmitrii Iarosh

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

// http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <QTimer>
#include <QUdpSocket>
#include <QNetworkInterface>
#include <zmqpp/zmqpp.hpp>

#include "messages_robocup_ssl_wrapper.pb.h"

class ReceiverWorker : public QObject
{
	Q_OBJECT
public:
	explicit ReceiverWorker();
	~ReceiverWorker();

public slots:
	void start();
    void changeSimulatorMode(bool isSim, const QString &netInterface);

private slots:
	void formStatistics();
	void processPendingDatagrams();

signals:
	void clientOpen(unsigned short port);
	void clientClose();
	void updateDetection(const QSharedPointer<SSL_WrapperPacket> & detection, int camID);
	void updateGeometry(const QSharedPointer<SSL_WrapperPacket> & geometry);
	void updateSSLFPS(const QString & message);

private:    
    bool open(unsigned short port, const QString &netInterface);
	void close();
    QNetworkInterface getInterfaceByName(const QString &netInterface);

	static const QString visionIP;
    static const QString defaultInterface;
    QUdpSocket mSocket;
    zmqpp::context context;
    zmqpp::socket socket;
    QUdpSocket mSocketFeedback;
	QTimer mStatisticsTimer;
	QHostAddress mGroupAddress;
	int mTotalPacketsNum = 0;
	int mPacketsPerSecond = 0;
	bool mIsSimEnabledFlag = false;
};
