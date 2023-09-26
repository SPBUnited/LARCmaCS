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

#include "matlabEngine.h"
#include <QDebug>
#include <QApplication>
#include <zmqpp/zmqpp.hpp>
#include <string>
#include <iostream>

using namespace std;

MatlabEngine::MatlabEngine(SharedRes * sharedRes)
    : EngineInterface(sharedRes)
{
}

MatlabEngine::~MatlabEngine()
{
}

void MatlabEngine::evaluate()
{
	processPacket(loadVisionData());
}

void MatlabEngine::evalString(const QString & str)
{
}

QSharedPointer<PacketSSL> MatlabEngine::loadVisionData()
{
	QSharedPointer<PacketSSL> packetSSL(new PacketSSL());

	QSharedPointer<SSL_WrapperPacket> geometryPacket = mSharedRes->getGeometry();
	if (!geometryPacket || !geometryPacket->IsInitialized()) {
		if (geometryPacket) {
			qDebug() << "Packet is uninitialized!";
		}
	} else {
		if (geometryPacket->has_geometry()) {
			packetSSL->fieldInfo[0] = geometryPacket->geometry().field().field_length();
			packetSSL->fieldInfo[1] = geometryPacket->geometry().field().field_width();
		}
	}


	int balls_n, idCam, robots_blue_n, robots_yellow_n;
	for (int i = 0; i < mSharedRes->getDetectionSize(); i++) {
		QSharedPointer<SSL_WrapperPacket> packet = mSharedRes->getDetection(i);
		if (!packet || !packet->IsInitialized()) {
			if (packet) {
				qDebug() << "Packet is uninitialized!";
			}
			continue;
		}

		if (packet->has_detection()) {
			SSL_DetectionFrame mDetection = packet->detection();

			idCam = mDetection.camera_id() + 1;
			balls_n = mDetection.balls_size();

            qDebug() << "Number of balls: " << balls_n;
            qDebug() << "Camera: " << idCam;

			// [Start] Ball info
            //qDebug() << balls_n << ' ';
            for (int j = 0; j < Constants::ballAlgoPacketSize; ++j)
            {
                for (int k = 0; k < Constants::maxBallsInCamera; ++k) {
                    packetSSL->balls[j*Constants::maxBallsInField+(idCam - 1) * Constants::maxBallsInCamera + k] = 0;
                }
            }
            for (int ball_id = 0; ball_id < balls_n; ++ball_id)
            {
                SSL_DetectionBall ball = mDetection.balls(ball_id);
                qDebug() << "Ball: " << ball.x() << " " << ball.y();
//                0 256 512
//                64 320 576
//                128 384 640
//                192 448 704
                qDebug() << ball_id+(idCam - 1) * Constants::maxBallsInCamera << " " << ball_id+Constants::maxBallsInField+(idCam - 1) * Constants::maxBallsInCamera << " " << ball_id+2*Constants::maxBallsInField+(idCam - 1) * Constants::maxBallsInCamera;
                packetSSL->balls[ball_id+(idCam - 1) * Constants::maxBallsInCamera] = idCam;
                packetSSL->balls[ball_id+Constants::maxBallsInField+(idCam - 1) * Constants::maxBallsInCamera] = ball.x();
                packetSSL->balls[ball_id+2*Constants::maxBallsInField+(idCam - 1) * Constants::maxBallsInCamera] = ball.y();
            }
			// [End] Ball info

			// [Start] Robot info
			robots_blue_n = mDetection.robots_blue_size();
			robots_yellow_n = mDetection.robots_yellow_size();

            for (int i = 0; i < robots_blue_n; i++) {
                SSL_DetectionRobot robot = mDetection.robots_blue(i);
				if (robot.has_robot_id() && robot.robot_id() >= 0 && robot.robot_id() <= Constants::maxRobotsInTeam) {
					packetSSL->robots_blue[robot.robot_id()] = idCam;
					packetSSL->robots_blue[robot.robot_id() + Constants::maxRobotsInTeam] = robot.x();
					packetSSL->robots_blue[robot.robot_id() + Constants::maxRobotsInTeam * 2] = robot.y();
					packetSSL->robots_blue[robot.robot_id() + Constants::maxRobotsInTeam * 3] = robot.orientation();
				} else {
					if (robot.has_robot_id()) {
						qDebug() << "Error: incorrect blue robot id" << robot.robot_id();
					}
				}
			}

			for (int i = 0; i < robots_yellow_n; i++) {
				SSL_DetectionRobot robot = mDetection.robots_yellow(i);
				if (robot.has_robot_id() && robot.robot_id() >= 0 && robot.robot_id() <= Constants::maxRobotsInTeam) {
					packetSSL->robots_yellow[robot.robot_id()] = idCam;
                    packetSSL->robots_yellow[robot.robot_id() + Constants::maxRobotsInTeam] = robot.x();
                    packetSSL->robots_yellow[robot.robot_id() + Constants::maxRobotsInTeam * 2] = robot.y();
                    packetSSL->robots_yellow[robot.robot_id() + Constants::maxRobotsInTeam * 3] = robot.orientation();
				} else {
					if (robot.has_robot_id()) {
						qDebug() << "Error: incorrect yellow robot id" << robot.robot_id();
					}
				}
			}
			// [End] Robot info
		}
	}

	return packetSSL;
}


void MatlabEngine::processPacket(const QSharedPointer<PacketSSL> & packetssl)
{
	if (packetssl.isNull()) {
		return;
	}
// Заполнение массивов Balls Blues и Yellows и запуск main-функции

	bool isCurrentTeamBlue = true;

	QVector<bool> barrierState = mSharedRes->getBarrierState();
	if (isCurrentTeamBlue) {
		for (int i = 0; i < Constants::maxRobotsInTeam; i++) {
			packetssl->robots_blue[i + Constants::maxRobotsInTeam * 4] = barrierState[i];
		}
	} else {
		for (int i = 0; i < Constants::maxRobotsInTeam; i++) {
			packetssl->robots_yellow[i + Constants::maxRobotsInTeam * 4] = barrierState[i];
		}
	}

    const string endpoint = "tcp://localhost:5667";
    zmqpp::context context;
    zmqpp::socket_type type = zmqpp::socket_type::subscribe;
    zmqpp::socket socket (context, type);
    socket.set(zmqpp::socket_option::receive_timeout, 500);
    socket.subscribe("");
    socket.connect(endpoint);
    zmqpp::message message;
    bool received = socket.receive(message);
    double * ruleArray = (double *)malloc(32 * 13  * sizeof(double));

    if (received) {
        memcpy(ruleArray, message.raw_data(), 32 * 13 * sizeof(double));
    }

    cout << "Received text:" << ruleArray << endl;

    for (int i = 0; i < 32; ++i) {
         for (int j = 0; j < 13; ++j) {
             std::cout << ruleArray[i * 13 + j] << ' ';
         }
         std::cout << std::endl;
     }

    qDebug() << "Rules" << ruleArray << endl;

    char sendString[256];
    sprintf(sendString, "Rules=zeros(%d, %d);", Constants::ruleAmount, Constants::ruleLength);
    evalString(sendString);

// Разбор пришедшего пакета и переправка его строк на connector

	QVector<Rule> rule(Constants::ruleAmount);
    if (received) {
        for (int i = 0; i < Constants::ruleAmount; i++) {
            rule[i].mSpeedX = ruleArray[i * Constants::ruleLength + 1];
            rule[i].mSpeedY = ruleArray[i * Constants::ruleLength + 2];
            rule[i].mSpeedR = ruleArray[i * Constants::ruleLength + 3];
            rule[i].mKickUp = ruleArray[i * Constants::ruleLength + 4];
            rule[i].mKickForward = ruleArray[i * Constants::ruleLength + 5];
            rule[i].mAutoKick = ruleArray[i * Constants::ruleLength + 6];
            rule[i].mKickerVoltageLevel = ruleArray[i * Constants::ruleLength + 7];
            rule[i].mDribblerEnable = ruleArray[i * Constants::ruleLength + 8];
            rule[i].mSpeedDribbler = ruleArray[i * Constants::ruleLength + 9];
            rule[i].mKickerChargeEnable = ruleArray[i * Constants::ruleLength + 10];
            rule[i].mBeep = ruleArray[i * Constants::ruleLength + 11];
        }
    }

    emit newData(rule);

    free(ruleArray);
}

void MatlabEngine::updatePauseState()
{
}

void MatlabEngine::pauseUnpause()
{
    mIsPause = !mIsPause;
    emit isPause(mIsPause);
}

void MatlabEngine::setDirectory(const QString & path)
{
   QString command = "cd " + path;
   qDebug() << "New Matlab directory = " << path;
}

