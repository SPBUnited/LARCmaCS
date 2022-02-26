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
	RCConfig rcconfig;
	MlData mtl(rcconfig);
	mMatlabData = mtl;
    runMatlab();
}

MatlabEngine::~MatlabEngine()
{
	engClose(mMatlabData.ep);
}

void MatlabEngine::evaluate()
{
	processPacket(loadVisionData());
}

void MatlabEngine::runMatlab()
{
	if (!(mMatlabData.ep = engOpen(NULL))) {
		qDebug() << "Can't open Matlab Engine" << endl;
		return;
	}

    mMatlabOutputBuffer[Constants::matlabOutputBufferSize - 1] = '\0';
	engOutputBuffer(mMatlabData.ep, mMatlabOutputBuffer, Constants::matlabOutputBufferSize - 1);
	qDebug() << "Matlab Engine is opened\n" << endl;

	//-----create Rules-----
	char sendString[256];
	sprintf (sendString, "Rules=zeros(%d, %d)", Constants::ruleAmount, Constants::ruleLength);
	evalString(sendString);

	QString dirPath = "cd " + QCoreApplication::applicationDirPath() + "/MLscripts";
	evalString(dirPath);
}

void MatlabEngine::evalString(const QString & str)
{
	engEvalString(mMatlabData.ep, str.toUtf8().data());
	QString tmp = QString(mMatlabOutputBuffer);
	if (!tmp.contains("\nispause =") && tmp != "") {
		emit consoleMessage(tmp);
	}
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

//    memcpy(mxGetPr(mMatlabData.Ball), packetssl->balls, Constants::maxBallsInField * Constants::ballAlgoPacketSize * sizeof(double));
//    memcpy(mxGetPr(mMatlabData.Blue), packetssl->robots_blue, Constants::robotAlgoPacketSize * sizeof(double));
//    memcpy(mxGetPr(mMatlabData.Yellow), packetssl->robots_yellow, Constants::robotAlgoPacketSize * sizeof(double));
//    memcpy(mxGetPr(mMatlabData.fieldInfo), packetssl->fieldInfo, Constants::fieldInfoSize * sizeof(double));

//    double state = mSharedRes->getRefereeState();
//    qDebug() << "REFEREE STATE " << state << endl;
//    memcpy(mxGetPr(mMatlabData.state), &state, sizeof(double));

//    double team = mSharedRes->getRefereeTeam();
//    qDebug() << "REFEREE TEAM " << team << endl;
//    memcpy(mxGetPr(mMatlabData.team), &team, sizeof(double));

//    double partOfField = (double)mSharedRes->getRefereePartOfFieldLeft();
//    qDebug() << "REFEREE part of field " << partOfField << endl;
//    memcpy(mxGetPr(mMatlabData.partOfFieldLeft), &partOfField, sizeof(double));

//    qDebug() << packetssl->balls[0] << ' '
//             << packetssl->balls[  Constants::maxBallsInField] << ' '
//             << packetssl->balls[2*Constants::maxBallsInField] << ' '
//             << packetssl->balls[1] << ' '
//             << packetssl->balls[1+Constants::maxBallsInField] << ' '
//             << packetssl->balls[1+2*Constants::maxBallsInField] << ' ';


//    engPutVariable(mMatlabData.ep, "Balls", mMatlabData.Ball);
//    engPutVariable(mMatlabData.ep, "Blues", mMatlabData.Blue);
//    engPutVariable(mMatlabData.ep, "Yellows", mMatlabData.Yellow);
//    engPutVariable(mMatlabData.ep, "FieldInfo", mMatlabData.fieldInfo);
//    engPutVariable(mMatlabData.ep, "RefState", mMatlabData.state);
//    engPutVariable(mMatlabData.ep, "RefCommandForTeam", mMatlabData.team);
//    engPutVariable(mMatlabData.ep, "RefPartOfFieldLeft", mMatlabData.partOfFieldLeft);
//    evalString(mMatlabData.config.file_of_matlab);

// Забираем Rules и очищаем его в воркспейсе

    mMatlabData.Rule = engGetVariable(mMatlabData.ep, "Rules");

    const string endpoint = "tcp://localhost:5667";
    zmqpp::context context;
    zmqpp::socket_type type = zmqpp::socket_type::subscribe;
    zmqpp::socket socket (context, type);
    socket.subscribe("");
    socket.connect(endpoint);
    zmqpp::message message;

    socket.receive(message);

        double * ruleArray =
                            (double *)malloc(32 * 13  * sizeof(double));
       memcpy(ruleArray, message.raw_data(), 32 * 13 * sizeof(double));
//       message.get(ruleArray, 32 * 13);

//    double * ruleArray =
//            (double *)malloc(Constants::ruleAmount * Constants::ruleLength * sizeof(double));
//    if (mMatlabData.Rule != 0) {
//        memcpy(ruleArray, mxGetPr(mMatlabData.Rule), Constants::ruleAmount * Constants::ruleLength * sizeof(double));
//    } else {
//        memset(ruleArray, 0, Constants::ruleAmount * Constants::ruleLength * sizeof(double));
//    }

    cout << "Received text:" << ruleArray << endl;

     for (int i = 0; i < 32; ++i)
     {
         for (int j = 0; j < 13; ++j)
         {
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

    for (int i = 0; i < Constants::ruleAmount; i++) {
        if (ruleArray[i * Constants::ruleLength] == 1) {
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

//    for (int i = 0; i < Constants::ruleAmount; i++) {
//        if (ruleArray[i] == 1) {
//            rule[i].mSpeedX = ruleArray[2 * Constants::ruleAmount + i];
//            rule[i].mSpeedY = ruleArray[3 * Constants::ruleAmount + i];
//            rule[i].mSpeedR = ruleArray[5 * Constants::ruleAmount + i];
//            rule[i].mKickUp = ruleArray[6 * Constants::ruleAmount + i];
//            rule[i].mKickForward = ruleArray[4 * Constants::ruleAmount + i];
//            rule[i].mAutoKick = ruleArray[7 * Constants::ruleAmount + i];
//            rule[i].mKickerVoltageLevel = ruleArray[8 * Constants::ruleAmount + i];
//            rule[i].mDribblerEnable = ruleArray[9 * Constants::ruleAmount + i];
//            rule[i].mSpeedDribbler = ruleArray[10 * Constants::ruleAmount + i];
//            rule[i].mKickerChargeEnable = ruleArray[11 * Constants::ruleAmount + i];
//            rule[i].mBeep = ruleArray[12 * Constants::ruleAmount + i];
//        }
//    }

    emit newData(rule);

    free(ruleArray);
//    mxDestroyArray(mMatlabData.Rule);

//	updatePauseState();
}

void MatlabEngine::updatePauseState()
{
//	evalString("ispause=RP.Pause");
//	mxArray *mxitpause = engGetVariable(mMatlabData.ep, "ispause");
//	bool pauseStatus = true;
//	if (mxitpause != 0) {
//		double* itpause = mxGetPr(mxitpause);
//		if (itpause != 0) {
//			if ((*itpause) == 0) {
//				mxArray* mxzMain_End = engGetVariable(mMatlabData.ep, "zMain_End");
//				if (mxzMain_End != 0) {
//					double* zMain_End = mxGetPr(mxzMain_End);
//					if (zMain_End != 0) {
//						if ((*zMain_End) == 0) {
//							emit algoStatus("main br");
//						} else {
//							pauseStatus = false;
//							emit algoStatus("WORK");
//						}
//					} else {
//						emit algoStatus("-err-z");
//					}
//				} else {
//					emit algoStatus("-err-mz");
//				}
//			} else {
//				emit algoStatus("PAUSE");
//			}
//		} else {
//			emit algoStatus("-err-p"); //Matlab answer corrupted
//		}
//	} else {
//		emit algoStatus("-err-mp"); //Matlab does not respond
//	}

//	if (mIsPause != pauseStatus) {
//		emit isPause(pauseStatus);
//		mIsPause = pauseStatus;
//	}
}

void MatlabEngine::pauseUnpause()
{
    mIsPause = !mIsPause;
//    qDebug() << "---------- PAUSE" << mIsPause << endl;
    emit isPause(mIsPause);
//	evalString("PAUSE();");
}

void MatlabEngine::setDirectory(const QString & path)
{
	QString command = "cd " + path;
	qDebug() << "New Matlab directory = " << path;
//	evalString(command);
}
