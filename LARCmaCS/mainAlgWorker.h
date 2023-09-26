#pragma once

#include <QObject>
#include <QSharedPointer>
#include <QTimer>

#include "packetSSL.h"
#include "sharedRes.h"
#include "engineInterface.h"

class MainAlgWorker : public QObject
{
	Q_OBJECT

public:
	MainAlgWorker(SharedRes * sharedRes);
	~MainAlgWorker();
	void run();

signals:
	void newPauseState(const QString & state);
	void sendStatistics(const QString & statistics);
	void toConsole(const QString & str);
	void newData(const QVector<Rule> & data);
	void pause(bool status);

public slots:
	void start();
	void setDirectory(const QString & path);
	void setDebugFrequency(int frequency);
	void formStatistics();
	void receiveMSGToConsole(const QString & str);
	void stop();
	void pauseUnpause();


private:
	EngineInterface * mEngine;
	int mFrequency = 100;
	bool mShutdownFlag;
	int mTotalPacketsNum = 0;
	int mPacketsPerSecond = 0;
	QTimer mStatisticsTimer;
	SharedRes * mSharedRes;
};
