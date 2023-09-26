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

#include "robotReceiver.h"

#include <QDebug>

RobotReceiver::RobotReceiver(SharedRes * sharedRes)
	: mSharedRes(sharedRes)
{
	qRegisterMetaType<QVector<int> >("QVector<int>");
	qRegisterMetaType<QVector<bool> >("QVector<bool>");

	mWorker = new RobotReceiverWorker();
	mWorker->moveToThread(&mThread);

	connect(&mThread, SIGNAL(started()), mWorker, SLOT(start()));
	connect(&mThread, SIGNAL(finished()), mWorker, SLOT(deleteLater()));

	connect(mWorker, SIGNAL(newBarrierState(const QVector<bool> &))
				, this, SLOT(changeBarrierState(const QVector<bool> &)));

	connect(mWorker, SIGNAL(newKickerChargeStatus(const QVector<int> &))
				, this, SIGNAL(newKickerChargeStatus(const QVector<int> &)));

	connect(mWorker, SIGNAL(newConnectionState(const QVector<int> &))
				, this, SIGNAL(newConnectionState(const QVector<int> &)));

	connect(mWorker, SIGNAL(newChargeLevel(const QVector<int> &))
				, this, SIGNAL(newChargeLevel(const QVector<int> &)));

	mThread.start();
}

RobotReceiver::~RobotReceiver()
{
	mThread.quit();
	mThread.wait();
}

void RobotReceiver::changeBarrierState(const QVector<bool> &barrierState)
{
	mSharedRes->setBarrierState(barrierState);
}

