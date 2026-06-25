#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QIntValidator>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;




signals:

    /*сигнал подключения к серверу*/
    void connectRequested(const QString& ip, const quint16 &port);
    /*сигнал отключения от сервера*/
    void disconnectRequested();
    /*сигнал "выполнить" тут либо зашифрование либо расшифрование*/
    void performRequested(const QString& text, bool isEncrypt);
    /*очистка формы*/
    void clearRequested();

public slots:
    /*слоты выводящие информацию в GUI от контроллера*/
    void clearPlainTextEdit();
    void showConnectionStatus(bool isConnected, const QString &statusText);
    void displayResult(const QString &resultText);
    void showError(const QString &errorMessage);

private slots:
    void on_pushButton_Connect_clicked();

    void on_pushButton_Disconnect_clicked();

    void on_pushButton_Perform_clicked();

    void on_pushButton_Clear_clicked();

private:
    void initUI();
    void updateConnectionUI(bool connected);

    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
