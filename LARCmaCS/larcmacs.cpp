#include "larcmacs.h"

#include <QFileDialog>
#include <QMenu>
#include <math.h>

#include "ui_larcmacs.h"
#include "packetSSL.h"
#include "message.h"
#include "settings.h"
#include "grSimRobot.h"

LARCmaCS::LARCmaCS(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::LARCmaCS)
	, scalingRequested(true)
	, sizescene(10)
	, drawscale(1)
	, receiver(&sharedRes)
	, robotReceiver(&sharedRes)
	, fieldscene(new FieldScene(&sharedRes))
	, mainalg(&sharedRes)
	, connector(&sharedRes)
    , mIsSim(false)
	, referee(&sharedRes)
{
	qRegisterMetaType<QVector<Rule> >("QVector<Rule>");

	ui->setupUi(this);
	ui->fieldView->setScene(fieldscene);
	scaleView(8);
	macsArray = new QString[Constants::maxNumOfRobots];

	sceneview.init();

	//algorithm connect
	connect(this, SIGNAL(setDirectory(const QString &)), &mainalg, SIGNAL(setDirectory(const QString &)));
	connect(this, SIGNAL(updateDebugFrequency(int)), &mainalg, SIGNAL(updateDebugFrequency(int)));
	connect(this, SIGNAL(pauseUnpause()), &mainalg, SIGNAL(pauseUnpause()));

	connect(ui->matlabConsole, SIGNAL(customContextMenuRequested(const QPoint &)),
			this, SLOT(matlabConsoleMenuRequested(const QPoint &)));

	//gui connector
	connect(&sceneview.worker, SIGNAL(updateView()), this, SLOT(updateView()));
	connect(ui->sceneslider, SIGNAL(valueChanged(int)), this, SLOT(scaleView(int)));

	connect(&mainalg, SIGNAL(toConsole(const QString &)), this, SLOT(toConsole(const QString &)));

	//info GUI
	connect(&mainalg, SIGNAL(updatePauseState(const QString &)), this, SLOT(UpdatePauseState(const QString &)));
	connect(&mainalg, SIGNAL(engineStatistics(const QString &)), this, SLOT(UpdateStatusBar(const QString &)));
	connect(&receiver, SIGNAL(updateSSLFPS(const QString &)), this, SLOT(UpdateSSLFPS(const QString &)));

	//remotecontrol
	connect(&remotecontol, SIGNAL(RC_control(int, int, int, int, bool)),
			this, SLOT(remcontrolsender(int, int, int, int, bool)));
    connect(this, SIGNAL(run(int, const QByteArray &)), &connector, SLOT(run(int, const QByteArray &)));
    connect(this, SIGNAL(runSim(const QByteArray &, bool)), &connector, SLOT(runSim(const QByteArray &, bool)));

	//simulator Enable
  connect(this, SIGNAL(connectorChanged(bool, const QString &, int, int, const QString &))
       , &receiver, SLOT(changeSimulatorMode(bool, const QString &, int, int, const QString &)));
 	connect(&receiver, SIGNAL(clearField()), fieldscene, SLOT(ClearField()));

  connect(this, SIGNAL(connectorChanged(bool, const QString &, int, int, const QString &))
       , &connector, SLOT(onConnectorChange(bool, const QString &, int, int, const QString &)));

  connect(this, SIGNAL(connectorChanged(bool, const QString &, int, int, const QString &))
       , &referee, SLOT(changeSimulatorMode(bool, const QString &, int, int, const QString &)));

	connect(&mainalg, SIGNAL(newData(const QVector<Rule> &))
				, &connector, SLOT(sendNewCommand(const QVector<Rule> &)));

	connect(&mainalg, SIGNAL(pause(bool))
				, &connector, SLOT(onPauseChanged(bool)));

	connect(&robotReceiver, SIGNAL(newConnectionState(const QVector<int> &))
				, this, SLOT(updateConnectionState(const QVector<int> &)));


	connect(&robotReceiver, SIGNAL(newChargeLevel(const QVector<int> &))
				, this, SLOT(updateChargeLevel(const QVector<int> &)));

	sceneview.start();
	UpdateStatusBar("Waiting SSL connection...");
	UpdateSSLFPS("FPS = 0");
}

void LARCmaCS::remcontrolsender(int l, int r,int k, int b, bool kickUp)
{
	QByteArray byteData;
    int rNum = ui->lineEditRobotIp->text().toInt();
	if (!mIsSim) {
        //DefaultRobot::formControlPacket(byteData, 0, l, r, k, kickUp, 0, 4, 0);
        DefaultRobot::formControlPacket(byteData, rNum, r, -l, k,
                kickUp, 0, 15, false, 0, 0, true, false);
	} else {
        GrSimRobot::formControlPacket(byteData, rNum, l, r, k, kickUp, 0, 4, 0);
	}

	unsigned short port;
	if (!mIsSim) {
		port = connector.getRobotPort();
	} else {
		port = connector.getGrSimPort();
	}

	if (!mIsSim) {
        emit run(rNum, byteData);
	} else {
        emit runSim(byteData, false);
	}
}

LARCmaCS::~LARCmaCS()
{
	delete ui;
}

void LARCmaCS::UpdatePauseState(const QString & message)
{
	ui->label_Pause->setText(message);
}

void LARCmaCS::UpdateSSLFPS(const QString & message)
{
	ui->label_SSL_FPS->setText(message);
}

void LARCmaCS::UpdateStatusBar(const QString & message)
{
	ui->StatusLabel->setText(message);
}

void LARCmaCS::scaleView(int _sizescene)
{
	drawscale = pow(0.9, _sizescene - sizescene);
	sizescene = _sizescene;
	scalingRequested = true;
}

void LARCmaCS::updateConnectionState(const QVector<int> & connectionState)
{
	ui->connection1->setText(connectionState[0] == 0 ? "No" : "Yes");
	ui->connection2->setText(connectionState[1] == 0 ? "No" : "Yes");
	ui->connection3->setText(connectionState[2] == 0 ? "No" : "Yes");
	ui->connection4->setText(connectionState[3] == 0 ? "No" : "Yes");
	ui->connection5->setText(connectionState[4] == 0 ? "No" : "Yes");
	ui->connection6->setText(connectionState[5] == 0 ? "No" : "Yes");
}

void LARCmaCS::updateChargeLevel(const QVector<int> &chargeLevel)
{
	ui->battery1->setNum(chargeLevel[0]);
	ui->battery2->setNum(chargeLevel[1]);
	ui->battery3->setNum(chargeLevel[2]);
	ui->battery4->setNum(chargeLevel[3]);
	ui->battery5->setNum(chargeLevel[4]);
	ui->battery6->setNum(chargeLevel[5]);
}

void LARCmaCS::toConsole(const QString & str)
{
	ui->matlabConsole->appendPlainText(str);
}

void LARCmaCS::updateView()
{
	if (scalingRequested) {
		qreal factor = ui->fieldView->matrix().scale (drawscale, drawscale).mapRect (QRectF(0, 0, 1, 1)).width();
		if (factor > 0.07 && factor < 100.0)
			ui->fieldView->scale (drawscale, drawscale);
		scalingRequested = false;
		ui->fieldView->viewport()->update();
	}
}

void LARCmaCS::on_pushButton_SetMLdir_clicked()
{
	Settings settings;
	static const char * key = "MLScriptsDir";
	QString  dir = settings.value(key, QCoreApplication::applicationDirPath()).toString();
	dir = QFileDialog::getExistingDirectory(Q_NULLPTR, QString(), dir);
	if (!dir.isEmpty()) {
		settings.setValue(key, dir);
		emit setDirectory(dir);
	}
}

void LARCmaCS::matlabConsoleMenuRequested(const QPoint & point)
{
	QMenu * menu = new QMenu(this);
	QAction * editDevice = new QAction(trUtf8("Clear console"), this);
	connect(editDevice, SIGNAL(triggered()), ui->matlabConsole, SLOT(clear()));
	menu->addAction(editDevice);
	menu->popup(ui->matlabConsole->viewport()->mapToGlobal(point));
}

void LARCmaCS::on_matlabOutputFrequencyLineEdit_textEdited(const QString & text)
{
	bool isInt = false;
	int frequency = text.split(" ")[0].toInt(&isInt);
	if (isInt && frequency != 0) {
		emit updateDebugFrequency(frequency);
	}
}

void LARCmaCS::on_pushButton_Pause_clicked()
{
	emit pauseUnpause();
}

void LARCmaCS::on_checkBox_SimEnable_stateChanged(int state)
{
	mIsSim = state > 0;
  emit connectorChanged(mIsSim, ui->lineEditSimIP->text(), ui->lineEditSimPort->text().toInt(), ui->lineEditSimPortYellow->text().toInt(), ui->lineEditNetInterface->text());
}

void LARCmaCS::on_pushButton_RemoteControl_clicked()
{
	remotecontol.hide();
	remotecontol.show();
	remotecontol.TimerStart();
}

void LARCmaCS::on_but_reference_clicked()
{
	Reference * refWindow = new Reference();
	refWindow->show();
}
