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

#include <QSharedPointer>

#include "engineInterface.h"
//#include "mlData.h"
#include "packetSSL.h"

class MatlabEngine : public EngineInterface
{
public:
	MatlabEngine(SharedRes * sharedRes);
	~MatlabEngine() override;
	void evaluate() override;
	void pauseUnpause() override;
	void setDirectory(const QString & path) override;

private:
	void runMatlab();
	void evalString(const QString & str);
	QSharedPointer<PacketSSL> loadVisionData();
	void processPacket(const QSharedPointer<PacketSSL> & packetssl);
	void updatePauseState();

	char mMatlabOutputBuffer[Constants::matlabOutputBufferSize];
//	MlData mMatlabData;
	bool mIsPause {false};
};
