#include "connector.h"

#include <QMap>

#include "grSimRobot.h"
#include "erForceRobot.h"
#include "defaultRobot.h"
#include <QThread>

const QList<QString> Connector::robotBoxIPs = {
    QStringLiteral("10.0.120.210")
};

Connector::Connector(SharedRes * sharedRes)
	: mSharedRes(sharedRes)
	, mUdpSocket(this)
	, mStatisticsTimer(this)
{
    //connect(&mUdpSocket, SIGNAL(readyRead()), this, SLOT(newRobotFeedbackDatagram()));
}

Connector::~Connector()
{}

void Connector::newRobotFeedbackDatagram()
{
    while (mUdpSocket.hasPendingDatagrams()) {
        //int datagramSize = static_cast<int>(mUdpSocket.pendingDatagramSize());
        //QByteArray datagram;
        //datagram.resize(datagramSize);
        //mUdpSocket.readDatagram(datagram.data(), datagram.size());
        /*
        QSharedPointer<sslsim::RobotFeedback> packet(new sslsim::RobotFeedback());
        auto parseResult = packet->ParseFromArray(datagram.data(), datagramSize);
        if (!parseResult) {
            qDebug() << "ERROR: Failed to parse feedback packet from datagram; skipping";
            continue;
        }

        if (!packet->IsInitialized()) {
            qDebug() << "ERROR: Feedback packet is uninitialized; skipping";
            continue;
        }
        */

        //qDebug() << "Kek" << endl;
    }
}

const QString & Connector::getGrSimIP()
{
	return mGrSimIP;
}

unsigned short Connector::getGrSimPort()
{
	return mGrSimPort;
}

unsigned short Connector::getRobotPort()
{
	return mRobotPort;
}

void Connector::run(int N, const QByteArray & command)
{
    for (int i = 0; i < robotBoxIPs.length(); i++) {
        mUdpSocket.writeDatagram(command, QHostAddress(robotBoxIPs[i]), DefaultRobot::robotPort);
    }
}

void Connector::runSim(const QByteArray & command, bool isYellow)
{
    mUdpSocket.writeDatagram(command, QHostAddress(mGrSimIP), isYellow ? mGrSimPortYellow: mGrSimPort);
}

void Connector::onConnectorChange(bool isSim, const QString &ip, int port, int portYellow, const QString &)
{
	mIsSim = isSim;

	if (mIsSim) {
        mUdpSocket.close();
		mGrSimIP = ip;
		mGrSimPort = port;
        mGrSimPortYellow = portYellow;
        mUdpSocket.bind(QHostAddress(mGrSimIP), mGrSimPort, QUdpSocket::ShareAddress);
	}
}

void Connector::sendNewCommand(const QVector<Rule> & rule)
{
	if (!mIsPause) {
		for (int k = 0; k < rule.size(); k++) {
			QByteArray command;
			bool simFlag = mIsSim;
			if (!simFlag) {
                if (!mIsPause) {
                    DefaultRobot::formControlPacket(command, k, rule[k].mSpeedX, rule[k].mSpeedY, rule[k].mSpeedR,
							rule[k].mKickUp, rule[k].mKickForward, rule[k].mKickerVoltageLevel,
													rule[k].mDribblerEnable, rule[k].mSpeedDribbler, rule[k].mAutoKick,
													rule[k].mKickerChargeEnable, rule[k].mBeep);
				} else {
                    DefaultRobot::formControlPacket(command, k, 0, 0, 0, 0, 0, 0, 0);
				}
			} else {
				if (!mIsPause) {
//                    GrSimRobot::formControlPacket(command, k, rule[k].mSpeedX, rule[k].mSpeedY, rule[k].mSpeedR,
//                                                  rule[k].mKickUp, rule[k].mKickForward, rule[k].mKickerVoltageLevel,
//                                                  rule[k].mDribblerEnable, rule[k].mSpeedDribbler, rule[k].mAutoKick,
//                                                  rule[k].mKickerChargeEnable, rule[k].mBeep);
                    ErForceRobot::formControlPacket(command, k, rule[k].mSpeedX, rule[k].mSpeedY, rule[k].mSpeedR,
                                                  rule[k].mKickUp, rule[k].mKickForward, rule[k].mKickerVoltageLevel,
                                                  rule[k].mDribblerEnable, rule[k].mSpeedDribbler, rule[k].mAutoKick,
                                                  rule[k].mKickerChargeEnable, rule[k].mBeep);
                } else {
//                    GrSimRobot::formControlPacket(command, k, 0, 0, 0, 0, 0, 0, 0);
                    ErForceRobot::formControlPacket(command, k, 0, 0, 0, 0, 0, 0, 0);
				}
			}

            if (rule[k].mSpeedX != 0 ||
                rule[k].mSpeedY != 0 ||
                rule[k].mSpeedR != 0 ||
                rule[k].mKickUp != 0 ||
                rule[k].mKickForward != 0 ||
                rule[k].mDribblerEnable != 0 ||
                rule[k].mAutoKick != 0 ||
                rule[k].mSpeedX != oldRule[k].mSpeedX ||
                rule[k].mSpeedY != oldRule[k].mSpeedY ||
                rule[k].mSpeedR != oldRule[k].mSpeedR ||
                rule[k].mKickUp != oldRule[k].mKickUp ||
                rule[k].mKickForward != oldRule[k].mKickForward ||
                rule[k].mKickerVoltageLevel != oldRule[k].mKickerVoltageLevel ||
                rule[k].mDribblerEnable != oldRule[k].mDribblerEnable ||
                rule[k].mSpeedDribbler != oldRule[k].mSpeedDribbler ||
                rule[k].mAutoKick != oldRule[k].mAutoKick ||
                rule[k].mKickerChargeEnable != oldRule[k].mKickerChargeEnable ||
                rule[k].mBeep != oldRule[k].mBeep) {

                if (!simFlag) {
                    emit run(k, command);
                } else {
                    emit runSim(command, k >= rule.size()/2);
                }
                oldRule[k] = rule[k];
            }
		}

	}
}

void Connector::onPauseChanged(bool status)
{
    const int maxAttemptsCount = 5;
	qDebug() << "onPauseChanged" << status;
	mIsPause = status;

	if (mIsPause) {
		QByteArray command;
        for (int attempt = 0; attempt < maxAttemptsCount; ++attempt) {
            if (!mIsSim) {
                for (int i = 1; i <= Constants::maxNumOfRobots; i++) {
                    DefaultRobot::formControlPacket(command, i, 0, 0, 0, 0, 0, 0, 0);
                    run(i, command);
                }
            } else {
                for (int i = 0; i <= Constants::maxNumOfRobots; i++) {
    //                GrSimRobot::formControlPacket(command, i, 0, 0, 0, 0, 0, 0, 0);
                    ErForceRobot::formControlPacket(command, i, 0, 0, 0, 0, 0, 0, 0);
                    runSim(command, i >= Constants::maxRobotsInTeam);
                }
            }
            QThread::msleep(100);
        }
	}
}
