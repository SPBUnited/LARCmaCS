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

#include "erForceRobot.h"
#include <cmath>
#include <QDebug>

ErForceRobot::ErForceRobot(){}

ErForceRobot::Speed ErForceRobot::fromMLSpeed2ERSpeed(const int MLSpeedX, const int MLSpeedY)
{
    float oldNorma = sqrt(MLSpeedX*MLSpeedX+MLSpeedY*MLSpeedY);
    float newNorma = std::min(oldNorma, 100.0f)*0.05f;
    float multiplier = newNorma / std::max(oldNorma, 0.001f);
    return Speed(MLSpeedX*multiplier, -MLSpeedY*multiplier);
};

void ErForceRobot::formControlPacket(QByteArray & command, int numOfRobot, int speedX, int speedY, int speedR,
								   bool kickUp, bool kickForward, int kickVoltage,
                                   bool enableSpinner, float spinnerSpeed, int autoKick,
								   bool kickerCharge, bool beep)
{
    RobotControl packet;
    bool yellow = false;
	if (numOfRobot >= Constants::maxRobotsInTeam) { //TODO use field TEAM from rule
		yellow = true;
	}
    RobotCommand* robotCommand = packet.add_robot_commands();

    // TODO: What about team? Perhaps deletion is not needed
    robotCommand->set_id(numOfRobot % Constants::maxRobotsInTeam);
    // TODO: Convert (kickVoltage, kickForward, kickUp to speed and angle)
    robotCommand->set_kick_speed(autoKick+kickUp+kickForward ? kickVoltage: 0);
    robotCommand->set_kick_angle(autoKick == 2 || kickUp ? 45 : 0);
    // TODO: How dribbler and spenner are related?
    robotCommand->set_dribbler_speed(enableSpinner ? spinnerSpeed: 0);

    MoveLocalVelocity* localVelocity = robotCommand->mutable_move_command()->mutable_local_velocity();
    Speed erSpeed = fromMLSpeed2ERSpeed(speedX, speedY);
    localVelocity->set_forward(erSpeed.SpeedX);
    localVelocity->set_left(erSpeed.SpeedY);
    localVelocity->set_angular(std::max(std::min(speedR, 100), -100)/5.0f);

	command.resize(packet.ByteSize());
	packet.SerializeToArray(command.data(), command.size());
}
