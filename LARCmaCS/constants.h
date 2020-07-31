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

class Constants
{
public:
	static const int maxNumOfRobots = 16;
	static const int maxRobotsInTeam = maxNumOfRobots; //maxNumOfRobots / 2;
	static const int robotAlgoPacketSize = 5 * maxRobotsInTeam;
	static const int ballAlgoPacketSize = 3;
	static const int fieldInfoSize = 2;
	static const unsigned  SSLVisionPort = 10006;
	static const unsigned  SimVisionPort = 10020;
	static const unsigned  refereePort = 10003;
	static const int numOfCameras = 4;
	static const int ruleLength = 13;
	static const int ruleAmount = 16;
	static const int matlabOutputBufferSize = 2048;
};
