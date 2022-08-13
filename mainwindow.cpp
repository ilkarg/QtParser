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
        QRadioButton *_allMatchRadioButton = ui->allMatchRadioButton;

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

        if (reply->error() == QNetworkReply::NoError) {
            QString parsedData = reply->readAll();
            while ((pos = regex.indexIn(parsedData, pos)) != -1) {
                 list << regex.cap(1);
                 pos += regex.matchedLength();
             }

            for (int i = 0; i < list.count(); i++) {
                if (!_appSystem->isEmpty(list[i])) {
                    break;
                }
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
