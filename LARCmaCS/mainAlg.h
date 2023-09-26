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

#include <QThread>

#include "mainAlgWorker.h"
#include "sharedRes.h"

#include "messages_robocup_ssl_wrapper.pb.h"

using namespace std;

class MainAlg : public QObject
{
	Q_OBJECT

public:
	explicit MainAlg(SharedRes * sharedRes);
	~MainAlg();

signals:
	void updateBallStatus(bool status);
	void updateDebugFrequency(int frequency);
	void pauseUnpause();
	void setDirectory(const QString & s);
	void sendToConnector(int N, const QByteArray & command);
	void sendToSimConnector(const QByteArray & command);
	void updateEnableSimFlag(bool flag);
	void engineStatistics(const QString & status);
	void updatePauseState(const QString & state);
	void toConsole(const QString & str);
  void connectorChanged(bool, const QString &, int, int, const QString &);
	void newData(const QVector<Rule> & data);
	void pause(bool status);
	void stopped();

private:
	SSL_DetectionFrame mDetection;
	MainAlgWorker * mWorker;
	QThread mThread;
};
