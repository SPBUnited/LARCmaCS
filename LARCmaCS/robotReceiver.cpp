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

#include "robotReceiver.h"

RobotReceiver::RobotReceiver(){}

RobotReceiver::~RobotReceiver()
{
	stop();
}

void RobotReceiver::stop()
{
	emit wstop();
}

void RobotReceiver::start()
{
	mThread.start();
}

void RobotReceiver::setBallInsideData(const QString & ip, bool isBallInside)
{
	mSharedRes->setBallInsideData(ip, isBallInside);
}

void RobotReceiver::init(SharedRes * sharedRes)
{
	mSharedRes = sharedRes;
	mWorker.moveToThread(&mThread);
	connect(&mThread, SIGNAL(started()), &mWorker, SLOT(start()));
	connect(this, SIGNAL(wstop()), &mWorker, SLOT(stop()));
	connect(&mWorker, SIGNAL(finished()), &mThread, SLOT(quit()));
	connect(&mThread, SIGNAL(finished()), &mThread, SLOT(deleteLater()));
	connect(&mWorker, SIGNAL(finished()), &mWorker, SLOT(deleteLater()));
	connect(&mWorker, SIGNAL(setBallInsideData(const QString &, bool)),
			this, SLOT(setBallInsideData(const QString &, bool)));
}