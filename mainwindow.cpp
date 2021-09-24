#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_dialog.h"

#include <QFileDialog>
#include <QPainter>
#include <QMessageBox>
#include <QMatrix>
#include <QToolTip>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->statusbar->addPermanentWidget(ui->SelectButton);
    ui->statusbar->addPermanentWidget(ui->MediaButton);
    ui->statusbar->addPermanentWidget(ui->InfoButton);
    ui->statusbar->addPermanentWidget(ui->isNotPausedButton);
    ui->statusbar->addPermanentWidget(ui->deviceButton);
    //ui->statusbar->setStyleSheet("background-color:black;color:white");

    ui->SelectButton->setToolTip(QString::fromStdWString(L"选择存放ffmpeg与sdl2动态库目录"));
    ui->MediaButton->setToolTip(QString::fromStdWString(L"选择要播放的媒体文件"));
    ui->InfoButton->setToolTip(QString::fromStdWString(L"显示文件信息"));
    ui->isNotPausedButton->setToolTip(QString::fromStdWString(L"静音/取消静音"));
    ui->deviceButton->setToolTip(QString::fromStdWString(L"关闭/开启所有音频设备"));

    m_control = new control();

    ui->isNotPausedButton->setText(m_control->isNotPausedButtonText);

    _conn();
}

void MainWindow::_conn()
{
    connect(ui->SelectButton, &QPushButton::clicked, this, &MainWindow::on_Selected_Clicked);
    connect(ui->MediaButton, &QPushButton::clicked, this, &MainWindow::on_Media_Clicked);
    connect(ui->InfoButton, &QPushButton::clicked, this, &MainWindow::on_Info_Clicked);
    connect(ui->isNotPausedButton, &QPushButton::clicked, this, [this]
            { this->m_control->isPausedAudio(); });
    connect(ui->deviceButton, &QPushButton::clicked, this, [this]
            { this->m_control->changeAudioDevice(); });
    //错误提示
    connect(m_control, &control::noSupported, this, [this]
            { QMessageBox::critical(this, "Error", "The file is not a media file format"); }); //视频音频任意一个
    connect(m_control, &control::noSuitVideoCodec, this, [this]
            { QMessageBox::critical(this, "Error", "Can't get this video decoder"); });
    connect(m_control, &control::noSuitAudioCodec, this, [this]
            { QMessageBox::critical(this, "Error", "Can't get this video decoder"); });
    connect(m_control, &control::Custom_Signal, this, [this](QString Qstr)
            { QMessageBox::critical(this, "Error", Qstr); });

    //
    connect(m_control, &control::s, this, &MainWindow::slotGetOneFrame);
    connect(m_control, &control::changeButtonText, this, [this]
            { ui->isNotPausedButton->setText(m_control->isNotPausedButtonText); });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_Selected_Clicked()
{
    srcDirPath = QFileDialog::getExistingDirectory(this, "choose src Directory", "./");
    if (srcDirPath.isEmpty())
        return;

    QDir dir(srcDirPath);
    dir.setFilter(QDir::Files);
    dir.setSorting(QDir::Name);
    dir.setNameFilters(QStringList("*.dll"));
    if (dir.isEmpty())
    {
        QMessageBox::critical(this, "Error", QString::fromStdWString(L"未检测到dll文件"));
        return;
    }
    QStringList temp = dir.entryList();
    for (int i = 0; i < temp.size(); i++)
    {
        m_control->getVideoHandle()->m_help->rec.append(temp[i]);
        m_control->getAudioHandle()->m_help->rec.append(temp[i]);
    }
    srcDirPath.replace('/', "\\\\");
    m_control->getVideoHandle()->m_help->loadFfmpegDll(srcDirPath);
    m_control->getAudioHandle()->m_help->loadFfmpegDll(srcDirPath);
    m_control->getAudioHandle()->loadSDL(srcDirPath);

    QMessageBox::information(this, " ", QString::fromStdWString(L"动态库加载完成"));
}

void MainWindow::on_Media_Clicked()
{
    if (m_control->getVideoHandle()->m_help->dllMap.empty() && m_control->getAudioHandle()->m_help->dllMap.empty())
    {
        QMessageBox::critical(this, "Error", QString::fromStdWString(L"未加载所需dll !"));
        return;
    }
    m_control->forceFinishPlay();
    QString MediaPath = QFileDialog::getOpenFileName(this, "choose a media file", "./", "All files(*.*)");
    if (MediaPath.isEmpty())
        return;
    MediaPath.replace('/', "\\\\");
    m_control->getVideoHandle()->getFileName(MediaPath.toStdString());
    m_control->getAudioHandle()->getFileName(MediaPath.toStdString());

    m_control->begin();
    m_control->getMediaInfo();
    m_control->play();
}

void MainWindow::on_Info_Clicked()
{
    this->dial = new Dialog(this);

#define INSERT(s, info)                                       \
    dial->ui->textEdit->append(QString::fromStdWString(L#s)); \
    dial->ui->textEdit->insertPlainText(info)

    INSERT(容器：, m_control->m_format);
    INSERT(时长：, QString::number(m_control->m_duration, 10, 3) + "s");
    INSERT(总帧数：, QString::number(m_control->m_frameNums));
    if (m_control->getVideoHandle()->videoIndex >= 0 && m_control->getAudioHandle()->audioIndex < 0)
    {
        INSERT(视频编码：, m_control->m_videoDecode);
        INSERT(旋转角度：, QString::number(m_control->m_rotate));
        INSERT(帧率：, QString::number(m_control->m_frameRate, 10, 3));
        dial->show();
        return;
    }
    if (m_control->getVideoHandle()->videoIndex < 0 && m_control->getAudioHandle()->audioIndex >= 0)
    {
        INSERT(音频编码：, m_control->m_audioDecode);
        dial->show();
        return;
    }
    INSERT(视频编码：, m_control->m_videoDecode);
    INSERT(旋转角度：, QString::number(m_control->m_rotate));
    INSERT(帧率：, QString::number(m_control->m_frameRate, 10, 3));
    INSERT(音频编码：, m_control->m_audioDecode);
    dial->show();

#undef INSERT
    return;
}

void MainWindow::showFileError()
{
    double num = (std::max)(m_control->getVideoHandle()->m_realVideoDuration, m_control->getAudioHandle()->m_realAudioDuration);
    QMessageBox::critical(this, "Error", "Media file have error,currently playing to:" + QString::number(num, 10, 3) + "s");
}

void MainWindow::slotGetOneFrame(QImage &img)
{
    mImage = img;
    update();
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    if (m_control->m_rotate > 0)
    {
        QMatrix matrix;
        matrix.rotate(m_control->m_rotate);
        mImage = mImage.transformed(matrix, Qt::FastTransformation);
    }
    painter.setBrush(Qt::white);
    painter.drawRect(0, 0, this->width(), this->height());

    if (mImage.size().width() <= 0)
        return;
    const QImage &img = mImage.scaled(this->size(), Qt::KeepAspectRatio);

    int x = this->width() - img.width();
    int y = this->height() - img.height();

    x /= 2;
    y /= 2;

    painter.drawImage(QPoint(x, y), img);
}
