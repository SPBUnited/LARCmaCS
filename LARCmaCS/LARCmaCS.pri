# Copyright 2019 Irina Khonakhbeeva

# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at

# http://www.apache.org/licenses/LICENSE-2.0

# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

win32 {
  #add libs
  LIBS += -lws2_32
  LIBS += -L$${MATLAB_DIR}/lib/win$${BIT}/microsoft/ -llibeng \
                  -L$${MATLAB_DIR}/lib/win$${BIT}/microsoft/ -llibmat \
                  -L$${MATLAB_DIR}/lib/win$${BIT}/microsoft/ -llibmx
}

unix {
#  LIBS += -L$${MATLAB_DIR}/../bin/glnxa64 -leng \
#          -L$${MATLAB_DIR}/../bin/glnxa64 -lmat \
#          -L$${MATLAB_DIR}/../bin/glnxa64 -lmx \
#          -L$${MATLAB_DIR}/../bin/glnxa64 -lmwresource_core \
#          -L$${MATLAB_DIR}/../bin/glnxa64 -licudata \
LIBS +=   -L/usr/local/lib -lzmqpp \
          -lzmq
}

QT += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LARCmaCS
TEMPLATE = app

#directories of sources of the vision client
INCLUDEPATH += \
    $${SHARED_DIR}/util \
    $${SHARED_DIR}/rfprotocol \
#    $${MATLAB_DIR}/include \
    $$PWD/robots \
    $$PWD/referee \
    $$PWD \

SOURCES +=  \
	$$PWD/main.cpp \
	$$PWD/larcmacs.cpp \
        $$PWD/fieldScene.cpp \
	$$PWD/sceneView.cpp \
	$$PWD/receiver.cpp \
	$$PWD/mainAlg.cpp \
	$$PWD/remotecontrol.cpp \
	$$PWD/message.cpp \
	$$PWD/reference.cpp \
	$$PWD/sceneViewWorker.cpp \
	$$PWD/robot.cpp \
	$$PWD/receiverWorker.cpp \
	$$PWD/mainAlgWorker.cpp \
	$$PWD/packetSSL.cpp \
	$$PWD/settings.cpp \
	$$PWD/robots/grSimRobot.cpp \
	$$PWD/robots/defaultRobot.cpp \
        $$PWD/robots/erForceRobot.cpp \
	$$PWD/sharedRes.cpp \
	$$PWD/robotReceiverWorker.cpp \
	$$PWD/robotReceiver.cpp \
        $$PWD/connector.cpp \
        $$PWD/matlabEngine.cpp \
        $$PWD/referee/gameState.cpp \
        $$PWD/referee/refereeClient.cpp \
        $$PWD/referee/refereeClientWorker.cpp \
        $$PWD/referee/refereeMessage.cpp \
        $$PWD/referee/teamInfo.cpp \

HEADERS  += \
	$${SHARED_DIR}/util/timer.h \
	$${SHARED_DIR}/util/field_default_constants.h \
	$$PWD/larcmacs.h \
	$$PWD/fieldScene.h \
	$$PWD/sceneView.h \
	$$PWD/packetSSL.h \
	$$PWD/receiver.h \
	$$PWD/mainAlg.h \
	$$PWD/remotecontrol.h \
	$$PWD/message.h \
	$$PWD/reference.h \
	$$PWD/sceneViewWorker.h \
	$$PWD/receiverWorker.h \
	$$PWD/robot.h \
	$$PWD/mainAlgWorker.h \
        $$PWD/settings.h \
	$$PWD/constants.h \
	$$PWD/robots/grSimRobot.h \
	$$PWD/robots/defaultRobot.h \
        $$PWD/robots/erForceRobot.h \
	$$PWD/sharedRes.h \
	$$PWD/robotReceiverWorker.h \
        $$PWD/robotReceiver.h \
        $$PWD/connector.h \
        $$PWD/engineInterface.h \
        $$PWD/matlabEngine.h \
        $$PWD/referee/gameState.h \
        $$PWD/referee/refereeClient.h \
        $$PWD/referee/refereeClientWorker.h \
        $$PWD/referee/refereeInfo.h \
        $$PWD/referee/refereeMessage.h \
        $$PWD/referee/refereeUtils.h \
        $$PWD/referee/teamInfo.h

FORMS += \
	$$PWD/larcmacs.ui \
	$$PWD/remotecontrol.ui \
	$$PWD/reference.ui \
