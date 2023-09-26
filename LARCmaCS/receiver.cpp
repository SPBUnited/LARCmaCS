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

#include "receiver.h"

Receiver::Receiver(SharedRes * sharedRes)
{
	mWorker = new ReceiverWorker();
	mSharedRes = sharedRes;

	mWorker->moveToThread(&mThread);

	connect(&mThread, SIGNAL(started()), mWorker, SLOT(start()));
	connect(&mThread, SIGNAL(finished()), mWorker, SLOT(deleteLater()));

	connect(mWorker, SIGNAL(updateSSLFPS(const QString &)), this, SIGNAL(updateSSLFPS(const QString &)));
    connect(this, SIGNAL(updateSimulatorMode(bool, const QString &)), mWorker, SLOT(changeSimulatorMode(bool, const QString &)));
	connect(mWorker, SIGNAL(updateDetection(const QSharedPointer<SSL_WrapperPacket> &, int))
				, this, SLOT(updateDetection(const QSharedPointer<SSL_WrapperPacket> &, int)));
	connect(mWorker, SIGNAL(updateGeometry(const QSharedPointer<SSL_WrapperPacket> &))
                , this, SLOT(updateGeometry(const QSharedPointer<SSL_WrapperPacket> &)));

	mThread.start();
}

Receiver::~Receiver()
{
	mThread.quit();
	mThread.wait();
}

void Receiver::updateDetection(const QSharedPointer<SSL_WrapperPacket> & detection, int camID)
{
	mSharedRes->setDetection(detection, camID);
}

void Receiver::updateGeometry(const QSharedPointer<SSL_WrapperPacket> & geometry)
{
	mSharedRes->setGeometry(geometry);
}

void Receiver::setDisplayFlag()
{
	mDisplayFlag = true;
}

void Receiver::changeSimulatorMode(bool isSim, const QString &, int, int, const QString &netInterface)
{
    emit updateSimulatorMode(isSim, netInterface);
}
