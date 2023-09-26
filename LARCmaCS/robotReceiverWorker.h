// Copyright 2019 Anastasiia Kornilova

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

#include <QHostAddress>
#include <QUdpSocket>
#include <QSharedPointer>

class RobotReceiverWorker : public QObject
{
	Q_OBJECT

public:
	RobotReceiverWorker();
	~RobotReceiverWorker();

public slots:
	void start();

signals:
	void newBarrierState(const QVector<bool> & barrierState);
	void newKickerChargeStatus(const QVector<int> & kickerChargeStatus);
	void newConnectionState(const QVector<int> & connectionState);
    void newChargeLevel(const QVector<int> & connectionState);

private slots:
	void processPendingDatagrams();

private:
    static const int mDatagramSize = 120;
    static const int mPort = 57004;
    static const int mRobotsInPacket = 6;
	static const int mOnePacketLength = mDatagramSize / mRobotsInPacket;
	static const QString mSocketIp;

	QUdpSocket mUdpSocket;

	QVector<bool> mBarrierState;
	QVector<int> mKickerChargeStatus;
	QVector<int> mConnectionState;
	QVector<int> mChargeLevel;
};
