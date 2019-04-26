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

#include <QObject>
#include <QThread>
#include "sharedRes.h"
#include "robotReceiverWorker.h"

class RobotReceiver : public QObject
{
	Q_OBJECT

private:
	QThread mThread;
	RobotReceiverWorker mWorker;
	SharedRes * mSharedRes;
public:
	RobotReceiver();
	~RobotReceiver();
	void start();
	void stop();
	void init(SharedRes * sharedRes);

private slots:
	void setBallInsideData(const QString & ip, bool isBallInside);

signals:
	void wstop();
};