#include "Controller.h"
#include <QDebug>
Controller::Controller(MainWindow *view, QObject *parent) : QObject(parent), m_view(view), m_network(new Network(this)) {

    /*Связываем графический интерфейс с контроллером*/
    connect(m_view, &MainWindow::connectRequested, this, &Controller::onConnectedRequested);
    connect(m_view, &MainWindow::disconnectRequested, this, &Controller::onDisconnectedRequested);
    connect(m_view, &MainWindow::performRequested, this, &Controller::onPerformRequested);
    connect(m_view, &MainWindow::clearRequested, this, &Controller::onClearRequested);

    /*Cвязываем контроллер с классом, отвечающим за взаимодействие с сетью*/
    connect(m_network, &Network::connected, this, &Controller::onNetworkConnected);
    connect(m_network, &Network::disconnected, this, &Controller::onNetworkDisconnected);
    connect(m_network, &Network::dataReceived, this, &Controller::onNetworkDataReceived);
    connect(m_network, &Network::errorOccurred, this, &Controller::onNetworkError);
}
Controller::~Controller()
{
}

//  НАстройка View -> Network


/*Пользователь в GUI нажал "подключиться", сигнал был получен, слот выполняет подключение*/
void Controller::onConnectedRequested(const QString &ip, const quint16 &port){
    qDebug() << "Подключение к" << ip << ":" << port;
    m_network->connectToServer(ip, port);
}

/*Пользователь в GUI нажал "отключиться", сигнал был получен, слот выполняет отключение*/
void Controller::onDisconnectedRequested(){
    m_network->disconnectFromServer();
}

void Controller::onPerformRequested(const QString &text, bool isEncrypt){
    /*
     *TODO: засунуть сборку пакета (не имею возможности сейчас
     *из-за отсутсивя проткола (жду реализацию))
    */
}

/*очищаем окно ввода и вывода текста*/
void Controller::onClearRequested(){
    m_view->clearPlainTextEdit();
}



/*  НАстройка Network -> View  */



void Controller::onNetworkConnected()
{
    m_view->showConnectionStatus(true, "Подключено к серверу");
}

void Controller::onNetworkDisconnected()
{
    m_view->showConnectionStatus(false, "Отключено от сервера");
}

void Controller::onNetworkError(const QString &errorMessage)
{
    m_view->showError(QString("Ошибка сети: %1").arg(errorMessage));
    m_view->showConnectionStatus(false, "Ошибка подключения");
}

void Controller::onNetworkDataReceived(const QByteArray &data)
{
    /* обработчик пакетов не могу сделать, в виду отсутсвия протокола*/

    QString resultText = QString::fromUtf8(data);
    m_view->displayResult(resultText);
}