#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initUI();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initUI() {

    /*Настройка поля ввода IP-адреса*/
    ui->lineEdit_Ip_addr->setPlaceholderText("192.168.1.10");
    ui->lineEdit_Ip_addr->setText("192.168.1.10");
    ui->lineEdit_Port->setToolTip("IP адрес сервера");

    /*Настройка поля ввода порта*/
    ui->lineEdit_Port->setPlaceholderText("55555");
    ui->lineEdit_Port->setText("55555");
    ui->lineEdit_Port->setValidator(new QIntValidator(1, 65535,this));
    ui->lineEdit_Port->setToolTip("Порт сервера от 1 до 65535");
    ui->lineEdit_Port->setMaxLength(5);  // Максимум 5 символов


    ui->plainTextEdit_original_text->setPlaceholderText("Введите текст для обработки...");
    ui->plainTextEdit_original_text->setToolTip("Текст для шифрования/расшифрования");

    ui->plainTextEdit_result->setReadOnly(true);
    ui->plainTextEdit_result->setPlaceholderText("Здесь появится результат...");
    ui->plainTextEdit_result->setToolTip("Результат обработки");

    ui->radioButton_Encrypt->setChecked(true);  // По умолчанию шифрование
    ui->radioButton_Encrypt->setToolTip("Режим шифрования");
    ui->radioButton_Decrypt->setToolTip("Режим расшифрования");

    ui->pushButton_Connect->setToolTip("Подключиться к серверу");
    ui->pushButton_Disconnect->setToolTip("Отключиться от сервера");
    ui->pushButton_Perform->setToolTip("Выполнить операцию");
    ui->pushButton_Perform->setEnabled(false);  /* Неактивна до подключения*/

    ui->plainTextEdit_original_text->setFocus();
}

void MainWindow::updateConnectionUI(bool connected)
{
    if (connected) {
        ui->pushButton_Connect->setEnabled(false);
        ui->pushButton_Disconnect->setEnabled(true);
        ui->lineEdit_Ip_addr->setEnabled(false);
        ui->lineEdit_Port->setEnabled(false);
        ui->pushButton_Perform->setEnabled(true);
    } else {
        ui->pushButton_Connect->setEnabled(true);
        ui->pushButton_Disconnect->setEnabled(false);
        ui->lineEdit_Ip_addr->setEnabled(true);
        ui->lineEdit_Port->setEnabled(true);
        ui->pushButton_Perform->setEnabled(false);
    }
}

/*Обрабатываем нажатие кнопки "Подключиться"*/
void MainWindow::on_pushButton_Connect_clicked()

{
    /*Обработку ввода порта сделаю потом*/

    /*Берем ип адрес и порт*/
    QString ip = ui->lineEdit_Ip_addr->text().trimmed();
    quint16 port = ui->lineEdit_Port->text().toUShort();
    /*эмитем в контроллер*/
    emit connectRequested(ip, port);
}


void MainWindow::on_pushButton_Disconnect_clicked()
{
    emit disconnectRequested();
}

/*Обрабатываем кнопку "Выполнить"*/
void MainWindow::on_pushButton_Perform_clicked()
{
    QString text = ui->plainTextEdit_original_text->toPlainText();
    bool isEncrypt = ui->radioButton_Encrypt->isChecked();

    emit performRequested(text,isEncrypt);

}


void MainWindow::on_pushButton_Clear_clicked()
{
    /* как я понял прямо так делать нехорошо,
     * так как контроллер должен принять решение что очистить
    ui->plainTextEdit_original_text->clear();
    ui->plainTextEdit_result->clear();*/


    emit clearRequested();
}



void MainWindow::clearPlainTextEdit()
{
    ui->plainTextEdit_original_text->clear();
    ui->plainTextEdit_result->clear();
}

void MainWindow::showConnectionStatus( bool isConnected, const QString &text){
    ui->label_Status->setText(text);
    if (isConnected) {
        ui->label_Status->setStyleSheet("color: green;");
    } else {
        ui->label_Status->setStyleSheet("color: red;");
    }
    updateConnectionUI(isConnected);
}

void MainWindow::displayResult(const QString &resultText)
{
    ui->plainTextEdit_result->setPlainText(resultText);
}

void MainWindow::showError(const QString &errorMessage)
{
    ui->label_Status_Action->setText(errorMessage);
}

