#pragma once

#include <QThread>

#include "refereeClientWorker.h"
#include "sharedRes.h"

struct RefereeClient : public QObject
{
	Q_OBJECT

public:
	explicit RefereeClient(SharedRes * sharedRes);
	~RefereeClient();

public slots:
	void updateReferee(const QSharedPointer<RefereeInfo> & refInfo);
    // TODO: Configure network update on separate signal
    void changeSimulatorMode(bool isSim, const QString &, int, int, const QString &);

signals:
    void changeNetInterface(const QString &netInterface);

private:
	RefereeClientWorker * mWorker;
	QThread mThread;
    SharedRes * mSharedRes;
};
