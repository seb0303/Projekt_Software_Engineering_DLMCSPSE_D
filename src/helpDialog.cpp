#include "../include/helpDialog.h"
#include "../ui/ui_helpDialog.h"

HelpDialog::HelpDialog(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HelpDialog)
{
    ui->setupUi(this);
    text_browser = this->ui->textBrowser;
    pushButton_quit = this->ui->pushButton_quit;
    QFile file(":/help/static/help/help.html");
    QString html;

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        html = QString::fromUtf8(file.readAll());
        text_browser->setHtml(html);
        file.close();
    } else {
        qWarning() << "Could not open help file.";
        this->close();
        this->deleteLater();
    }

    connect(pushButton_quit, &QPushButton::clicked, this, [this](){
        this->close();
        this->deleteLater();
    });
}

HelpDialog::~HelpDialog()
{
    delete ui;
}
