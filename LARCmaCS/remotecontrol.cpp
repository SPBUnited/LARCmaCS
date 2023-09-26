#include "remotecontrol.h"
#include "ui_remotecontrol.h"

#define RC_TIMER_CD 100

#include <QDebug>
RemoteControl::RemoteControl(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::RemoteControl)
{
	ui->setupUi(this);
	qDebug() << "START REMOTE CONTROL";
	connect(&timer, SIGNAL(timeout()), this, SLOT(RC_send()));
	connect(this, SIGNAL(destroyed()), this, SLOT(TimerStop()));
	for(int i = 0; i < 256; i++) {
		keys[i] = 0;
	}
	key_shift = 0;
	effort = 1;
}

void RemoteControl::TimerStart()
{
	if (!timer.isActive()) {
		qDebug() << "<RemContril>: Start";
		timer.start(RC_TIMER_CD);
	}
}
void RemoteControl::TimerStop()
{
	if (timer.isActive()) {
		emit RC_control(0, 0, 0, 0, 0);
		qDebug() << "<RemContril>: Stop";
		timer.stop();
	}
}

RemoteControl::~RemoteControl()
{
	delete ui;
}

#define QT_KEY_SHIFT_NATIVE 16
#define QT_KEY_ESCAPE_NATIVE 27

void RemoteControl::keyPressEvent(QKeyEvent * key)
{
	quint32 nkey = key->nativeVirtualKey();
	if (nkey < 256)
		keys[nkey] = 1;
	if (nkey == QT_KEY_ESCAPE_NATIVE) {
		this->close();
	}
	if (nkey == QT_KEY_SHIFT_NATIVE) {
		key_shift = 1;
	}
}

void RemoteControl::keyReleaseEvent(QKeyEvent * key)
{
	quint32 nkey = key->nativeVirtualKey();
    if (nkey < 256) {
        keys[nkey] = 0;
        qDebug() << nkey << " key release\n";
    }
	if (nkey == QT_KEY_SHIFT_NATIVE) {
		key_shift = 0;
	}
	if(nkey == ' ') {
		qDebug() << "Space release";
	}
}

void RemoteControl::RC_send(void)
{
	int L = 0, R = 0, S = 0, K = 0, B = 0;

	int xVel = 0, yVel = 0, r = 0;
	bool kickUp = false;

	if(keys[' ']) {
		kickUp = true;
	}

	if(keys[38] && key_shift) {
		effort++;
		qDebug() << "Effort: " << effort;
		return;
	}
	if(keys[40] && key_shift) {
		if (effort > 1) {
			effort--;
		}
		qDebug() << "Effort: " << effort;
		return;
	}

    if (keys['w'] || keys['W'] || keys[38] ) {
		yVel = 10 * effort;
	}
    if (keys['s'] || keys['S'] || keys[40]) {
		yVel = -10 * effort;
	}
    if (keys['a'] || keys['A'] || keys[37]) {
		xVel = 20 * effort;
	}
    if (keys['d'] || keys['D'] || keys[39]) {
		xVel = -20 * effort;
	}
    if (keys['r'] || keys['R']) {
		r = 10 * effort;
	}
    if (keys['f'] || keys['F']) {
		r = -10 * effort;
	}

	qDebug() << "emit " << xVel << " " << yVel << " " << r;
	emit RC_control(xVel, yVel, r, 0, kickUp);
}

void RemoteControl::closeEvent(QCloseEvent *)
{
	emit remoteControlOff();
	TimerStop();
}
