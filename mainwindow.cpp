#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "AppSystem.h"

QLineEdit *_linkLineEdit;
QLineEdit *_regexLineEdit;
AppSystem *_appSystem;
QString matchType = "None";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QWidget *widget = new QWidget();
    widget->setLayout(ui->gridLayout);
    setCentralWidget(widget);

    manager = new QNetworkAccessManager(this);
    _linkLineEdit = ui->linkLineEdit;
    _regexLineEdit = ui->regexLineEdit;

    _appSystem = new AppSystem();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_parsingButton_clicked()
{
    try {
        QRadioButton *_firstMatchRadioButton = ui->firstMatchRadioButton;

        bool linkLineEditIsEmpty = _appSystem->isEmpty(_linkLineEdit->text());
        bool regexLineEditIsEmpty = _appSystem->isEmpty(_regexLineEdit->text());

        if (linkLineEditIsEmpty || regexLineEditIsEmpty) {
            throw "Все поля должны быть заполнены";
        }

        if (!_linkLineEdit->text().startsWith("http://") && !_linkLineEdit->text().startsWith("https://")) {
            throw "Ссылка должна начинаться с http:// или https://";
        }

        matchType = _firstMatchRadioButton->isChecked() ? "First" : "All";

        QUrl url(_linkLineEdit->text());
        QNetworkRequest request(url);
        QNetworkReply *reply = manager->get(request);
        connect(reply, SIGNAL(finished()), this, SLOT(replyFinished()));
    }
    catch (char const* exception) {
        QMessageBox::critical(this, "Ошибка", exception);
        return;
    }
}

void MainWindow::replyFinished() {
    try {
        QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
        QRegExp regex(_regexLineEdit->text());
        QStringList list;
        int pos = 0;
        bool listIsNotEmpty = false;
        QString error = "";
        QString domainName = _appSystem->getSiteDomainName(_linkLineEdit->text());
        QFile file(domainName + ".txt");
        QTextStream textStream(&file);

        if (reply->error() == QNetworkReply::NoError) {
            QString parsedData = reply->readAll();
            while ((pos = regex.indexIn(parsedData, pos)) != -1) {
                if (matchType == "None" && matchType != "First" && matchType != "All") {
                    error = "Не удалось определить тип парсинга";
                }

                list << regex.cap(1);
                pos += regex.matchedLength();

                if (matchType == "First") {
                    break;
                }
            }

            for (int i = 0; i < list.count(); i++) {
                if (!_appSystem->isEmpty(list[i])) {
                    listIsNotEmpty = true;
                    break;
                }
            }

            if (!listIsNotEmpty) {
                error = "Не удалось распарсить страницу или совпадений не найдено";
                throw error;
            }

            if (file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
                for (int i = 0; i < list.count(); i++) {
                    textStream << list[i] + "\n";
                }

                file.resize(0);
                file.close();
            }
            else {
                error = "Не удалось открыть/создать файл";
                throw error;
            }
        }
        else {
            throw reply->errorString();
        }

        reply->deleteLater();
    }
    catch (QString exception) {
        QMessageBox::critical(this, "Ошибка", exception);
        return;
    }
}
