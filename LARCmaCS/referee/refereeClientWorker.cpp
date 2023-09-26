#include "refereeClientWorker.h"

#include <zmqpp/zmqpp.hpp>

using namespace std;

const QString RefereeClientWorker::hostName = QStringLiteral("224.5.23.1");
const QString RefereeClientWorker::defaultInterface = QStringLiteral("eth1");


RefereeClientWorker::RefereeClientWorker()
	: mSocket(this)
    , socket(context, zmqpp::socket_type::publish)
	, mGroupAddress(hostName)
{
	connect(&mSocket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));


    const std::string endpoint = "tcp://*:4243";

    zmqpp::context context;

    std::cout << "Opening connection to " << endpoint << "..." << std::endl;
    socket.bind(endpoint);
    std::cout << "Connected" << std::endl;
}

RefereeClientWorker::~RefereeClientWorker()
{}

void RefereeClientWorker::start()
{
    open(Constants::refereePort, defaultInterface);
}

// TODO: Deduplicate
QNetworkInterface RefereeClientWorker::getInterfaceByName(const QString &netInterface)
{
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    for (int i = 0; i < interfaces.length(); ++i) {
        if (interfaces.at(i).name() == netInterface) {
            qInfo() << "Found interface " << netInterface;
            return interfaces.at(i);
        };
    }
    qInfo() << "ERROR: No interface found for " << netInterface << ". Using first interface";
    return interfaces.at(0);
}

bool RefereeClientWorker::open(qint16 port, const QString & netInterface)
{
	close();
    QNetworkInterface interface = getInterfaceByName(netInterface);
	return mSocket.bind(QHostAddress::AnyIPv4, port, QUdpSocket::ShareAddress)
                && mSocket.joinMulticastGroup(mGroupAddress, interface);
}

void RefereeClientWorker::close()
{
	mSocket.close();
}

void RefereeClientWorker::processPendingDatagrams()
{
	while (mSocket.hasPendingDatagrams()) {
		QByteArray datagram;
		int datagramSize = static_cast<int>(mSocket.pendingDatagramSize());
		datagram.resize(datagramSize);

		Referee refPacket;
		mSocket.readDatagram(datagram.data(), datagramSize);
		refPacket.ParseFromArray(datagram.data(), datagramSize);

		RefereeMessage message;
		message = RefereeMessage(refPacket);

		GameState gState;
		RefereeInfo tempRefInfo;

		if (mRefInfo != nullptr) {
			gState.updateGameState(message, mRefInfo->state);
		} else {
			gState.updateGameState(message, HALT);
		}

		gState.updateRefereeInfoFromState(tempRefInfo);


        // TODO: Send to ZeroMQ here
        // send a message
//        std::cout << "Sending text and a number..." << std::endl;
        zmqpp::message send_message;
//        send_message << tempRefInfo.state << tempRefInfo.commandForTeam << tempRefInfo.isPartOfFieldLeft;

        std::string json_ref = "{"
                               "\"state\":" + std::to_string(tempRefInfo.state) + "," +
                               "\"team\":"  + std::to_string(tempRefInfo.commandForTeam) + "," +
                               "\"is_left\":" + std::to_string(tempRefInfo.isPartOfFieldLeft) +
                               "}";

        send_message  << json_ref;

        // compose a message from a string and a number
//        message << "Hello World!" << 42;
//        zmqpp::message message(datagramSize);
//        std::memcpy(message, datagram.data(), datagramSize);
//        double * ruleArray =
//                                (double *)malloc(32 * 13  * sizeof(double));
//        message.raw_data(0);
//        message.size(0);
//        send_message.add_raw(datagram.data(), datagramSize);
//        message << datagram.data();
        socket.send(send_message);

//        std::cout << "Sent message. " << datagramSize << " " << datagram.size() << " " << sizeof(datagram.data())/sizeof(datagram.data()[0]) << std::endl;
//        std::cout << "Finished." << std::endl;


		if (mRefInfo != nullptr) {
			if (tempRefInfo.state != mRefInfo->state
					|| tempRefInfo.commandForTeam != mRefInfo->commandForTeam
					|| tempRefInfo.isPartOfFieldLeft != mRefInfo->isPartOfFieldLeft) {
				mRefInfo = QSharedPointer<RefereeInfo>(new RefereeInfo(tempRefInfo));
				emit refereeInfoUpdate(mRefInfo);
			}
		} else {
			mRefInfo = QSharedPointer<RefereeInfo>(new RefereeInfo(tempRefInfo));
			emit refereeInfoUpdate(mRefInfo);
		}
	}
}

void RefereeClientWorker::changeNetInterface(const QString & netInterface)
{
    qInfo() << "Updating referee interface to" << netInterface;
    open(Constants::refereePort, netInterface);
}
