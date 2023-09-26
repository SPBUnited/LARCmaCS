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

#include <QByteArray>
#include "ssl_simulation_robot_control.pb.h"
#include "constants.h"

class ErForceRobot
{
public:
    ErForceRobot();
	static void formControlPacket(QByteArray & command, int numOfRobot, int speedX, int speedY, int speedR,
								  bool kickUp, bool kickForward, int kickVoltage,
                                  bool enableSpinner, float spinnerSpeed = 0, int autoKick = 0,
                                  bool kickerCharge = 1, bool beep = 0);
private:
    typedef struct Speed {
        float SpeedX;
        float SpeedY;
        Speed(float SpeedX, float SpeedY): SpeedX(SpeedX), SpeedY(SpeedY){};
    } Speed;

    static Speed fromMLSpeed2ERSpeed(const int MLSpeedX, const int MLSpeedY);
};
