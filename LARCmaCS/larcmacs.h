#pragma once

#include <QWidget>

#include "fieldScene.h"
#include "receiver.h"
#include "mainAlg.h"
#include "sceneView.h"
#include "connector.h"
#include "remotecontrol.h"
#include "reference.h"
#include "robotReceiver.h"
#include "referee/refereeClient.h"

#define NUM_CONTROL_ROBOTS 12

namespace Ui
{
	class LARCmaCS;
}

class LARCmaCS : public QWidget
{
	Q_OBJECT

public:
	RemoteControl remotecontol;
	explicit LARCmaCS(QWidget *parent = 0);
	~LARCmaCS();

	QString * macsArray;

protected:
	bool scalingRequested;
	SharedRes sharedRes;
	SceneView sceneview;
	Receiver receiver;
	FieldScene * fieldscene;
	MainAlg mainalg;
	Connector connector;

private:
	Ui::LARCmaCS * ui;
	float drawscale;
	qreal sizescene;
	QString wifiaddrdata[NUM_CONTROL_ROBOTS];
	QUdpSocket socket;
	RobotReceiver robotReceiver;
	bool mIsSim;
	RefereeClient referee;

private slots:
	void UpdateSSLFPS(const QString & message);
	void UpdateStatusBar(const QString & message);
	void UpdatePauseState(const QString & message);
	void matlabConsoleMenuRequested(const QPoint & point);
	void updateView();
	void scaleView(int);
	void updateConnectionState(const QVector<int> & connectionState);
	void updateChargeLevel(const QVector<int> & chargeLevel);

private slots:
	void on_pushButton_Pause_clicked();
	void on_pushButton_SetMLdir_clicked();
	void toConsole(const QString & str);
	void remcontrolsender(int l, int r,int k, int b, bool kickUp);
	void on_checkBox_SimEnable_stateChanged(int state);
	void on_pushButton_RemoteControl_clicked();
	void on_matlabOutputFrequencyLineEdit_textEdited(const QString & text);
	void on_but_reference_clicked();

signals:
	void updateDebugFrequency(int frequency);
    void ChangeSimulatorMode(bool state, const QString &netInterface);
	void pauseUnpause();
	void setDirectory(const QString & s);
	void updateRobots();
  void connectorChanged(bool isSim, const QString & ip, int port, int portYellow, const QString & netInterface);
	void run(int N, const QByteArray & command);
    void runSim(const QByteArray & command, bool);
};
