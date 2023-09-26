// Copyright 2019 Dmitrii Iarosh, Anastasiia Kornilova

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

#include <QThread>

#include "sharedRes.h"
#include "robotReceiverWorker.h"

class RobotReceiver : public QObject
{
	Q_OBJECT

public:
	RobotReceiver(SharedRes * sharedRes);
	~RobotReceiver();
	void start();

public slots:
    void changeBarrierState(const QVector<bool> & barrierState);

signals:
	void newKickerChargeStatus(const QVector<int> & kickerChargeStatus);
	void newConnectionState(const QVector<int> & connectionState);
	void newChargeLevel(const QVector<int> & connectionState);

private:
	QThread mThread;
	RobotReceiverWorker * mWorker;
	SharedRes * mSharedRes;
};
