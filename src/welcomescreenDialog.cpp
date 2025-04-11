#include "../include/welcomescreenDialog.h"
#include "../ui/ui_welcomescreenDialog.h"

WelcomeScreen::WelcomeScreen(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WelcomeScreen)
{
    ui->setupUi(this);

    // Button konfigurieren für neue Datenbank
    btn_addDb_ = this->ui->pushButton_addDb;
    btn_startDemo_ = this->ui->pushButton_start_demo;

    connect(btn_addDb_, &QPushButton::clicked, this, [this](){
        this->btnClickEventAddDb();
    });

    connect(btn_startDemo_, &QPushButton::clicked, this, [this](){
        this->btnClickEventStartDemo();
    });
}

WelcomeScreen::~WelcomeScreen()
{
    delete ui;
}

void WelcomeScreen::btnClickEventAddDb()
{
    QPointer<AddDatabaseDialog> w = new AddDatabaseDialog();
    w->show();

    // Sauberes löschen aus dem Heap
    this->close();
    this->deleteLater();
}

void WelcomeScreen::btnClickEventStartDemo()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);

    QPointer<MainWindow> w = new MainWindow(
        nullptr,
        constants::DB_PATH_DEMO,
        constants::DB_DEMO_NAME + constants::DB_FILE_EXT,
        false,
        "",
        "demo"
        );

    w->show();
    QApplication::restoreOverrideCursor();
    QMessageBox::information(nullptr, "Willkommen", "Willkommen zur PennyWise Demoversion. "
    "Hier kannst du alle Funktionen ausprobieren und Testdatensätze auswerten. "
    "Bitte beachte, dass Veränderungen in der Testdatenbank dauerhaft gespeichert werden.");

    // Sauberes löschen aus dem Heap
    this->close();
    this->deleteLater();

}

void WelcomeScreen::btnClickEventOpenDb()
{
    // Hole das Verzeichnis der .exe
    QString exeDir = QCoreApplication::applicationDirPath();
    exeDir += "/" + constants::DB_PATH;

    // Dateifilter definieren
    QString filter = "PennyWise-Datenbank (*.pwdb)";

    // Öffne den Dialog
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "Öffne .pwdb Datei",
        exeDir,
        filter
        );

    // Wenn eine Datei ausgewählt wurde
    if (!filePath.isEmpty()) {
        QPointer<MainWindow> w = new MainWindow(
            nullptr,
            QFileInfo(filePath).absolutePath(),
            QFileInfo(filePath).fileName(),
            false,
            "",
            "default"
            );
        w->show();
        this->close();
        this->deleteLater();
    } else {
        qDebug() << "No file chosen.";
        return;
    }
}

void WelcomeScreen::showOpenDbDialog()
{
    QFrame* target = this->ui->frame_top;
    QPushButton* btn_open = new QPushButton("Datenbank öffnen", this);
    btn_open->setCursor(Qt::PointingHandCursor);
    btn_open->setProperty("class", "pushButton_action pushButton_demo");
    target->layout()->addWidget(btn_open);
    connect(btn_open, &QPushButton::clicked, this, &WelcomeScreen::btnClickEventOpenDb);
}
