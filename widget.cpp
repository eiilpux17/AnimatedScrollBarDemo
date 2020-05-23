#include "widget.h"
#include "ui_widget.h"

#include <QDragEnterEvent>
#include <QMimeData>
#include <QSettings>
#include <QStringListModel>
#include <QDebug>
#include <QMenu>
#include <QProcess>
#include <QDir>
#include <QVariantAnimation>
#include <QTimer>

Widget::Widget(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    QSettings setting("HKEY_LOCAL_MACHINE\\SOFTWARE\\TortoiseSVN", QSettings::NativeFormat);
    if(setting.contains("ProcPath"))
    {
        ui->label->setText(setting.value("ProcPath").toString());
    }
    this->setAcceptDrops(true);
    ui->listWidget->installEventFilter(this);
    removeSelectionAct = new QAction("remove", this);
    removeAllAct = new QAction("clear", this);
    connect(removeSelectionAct, &QAction::triggered, this,  &Widget::removeSelectedPaths);
    connect(removeAllAct, &QAction::triggered, this,  &Widget::removeAllPaths);

    ui->listWidget->setHorizontalScrollBar(new AnimatedScrollBar(ui->listWidget));
    ui->listWidget->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    ui->listWidget->setVerticalScrollBar(new AnimatedScrollBar(ui->listWidget));
    ui->listWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::dragEnterEvent(QDragEnterEvent *event)
{
    const QMimeData *data = event->mimeData();
    if(data && data->hasUrls())
    {
        event->acceptProposedAction();
    }
}

void Widget::dropEvent(QDropEvent *event)
{
    const QMimeData *data = event->mimeData();
    if(data && data->hasUrls())
    {
        QStringList paths;
        auto urls = data->urls();
        std::for_each(urls.begin(), urls.end(), [&paths](const QUrl &url){
            paths.append(url.toLocalFile());
        });
        ui->listWidget->addItems(paths);
    }
}

bool Widget::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == ui->listWidget)
    {
        if(event->type() == QEvent::ContextMenu)
        {
            auto ctxMenuEvent = static_cast<QContextMenuEvent *>(event);
            QMenu ctxMenu;
            ctxMenu.addAction(removeSelectionAct);
            ctxMenu.addAction(removeAllAct);
            ctxMenu.exec(ctxMenuEvent->globalPos());
        }
    }
    return false;
}

void Widget::removeSelectedPaths()
{
    auto selections = ui->listWidget->selectedItems();
    for(auto item : selections)
    {
        delete ui->listWidget->takeItem(ui->listWidget->row(item));
    }
}

void Widget::removeAllPaths()
{
    ui->listWidget->clear();
}

void Widget::on_btnCommit_clicked()
{
    QString proPath = ui->label->text();
    if(!proPath.isEmpty())
    {

        QStringList paths;
        for(int i = 0; i < ui->listWidget->count(); ++i)
        {
            QString path = ui->listWidget->item(i)->text();
            path = QDir::toNativeSeparators(path);
            paths.append(QString("\"%1\"").arg(path));
        }
        if(!paths.isEmpty())
        {
            QProcess *process = new QProcess();
            process->setProgram(proPath);
            process->setNativeArguments("/command:commit /path:" +paths.join("*"));
            process->start();
            connect(process, SIGNAL(finished(int)), process, SLOT(deleteLater()));
        }
    }
}

//varAnima: 变量动画
//preferWidth: 临时记录动态的滚动条宽度
//_expandedWidth: 滚动条变粗后的宽度
AnimatedScrollBar::AnimatedScrollBar(QWidget *parent):
    QScrollBar(parent)
{
    varAnima = new QVariantAnimation(this); //创建动画
    varAnima->setDuration(100);
    connect(varAnima, &QVariantAnimation::valueChanged, this, [this](const QVariant &val){
        //valueChanged时，动画不一定在运行,需要约束
        if(varAnima->state() == QAbstractAnimation::Running)
        {
            preferWidth = val.toInt();
            updateGeometry();
        }
    });
}

QSize AnimatedScrollBar::sizeHint() const
{
    QSize tmp = QScrollBar::sizeHint(); //样式指定的宽度值，可以通过默认的sizeHint获取
    if(this->orientation() == Qt::Horizontal)
    {
        return QSize(tmp.width(), preferWidth); //仅改变宽度,实际由于布局的存在，长度值并不重要
    }
    return QSize(preferWidth, tmp.height());
}

void AnimatedScrollBar::setExpandedWidth(int val)
{
    _expandedWidth = val;
}

bool AnimatedScrollBar::event(QEvent *e)
{
    if(e->type() == QEvent::Polish)
    {
        //初始化preferWidth，也可以在第一次sizeHint()被调用时初始化
        QSize tmp = QScrollBar::sizeHint();
        preferWidth = this->orientation() == Qt::Horizontal ? tmp.height() : tmp.width();
    }
    else if(e->type() == QEvent::HoverEnter)
    {
        if(varAnima->state() == QAbstractAnimation::Running)
            varAnima->stop();

        varAnima->setStartValue(preferWidth);
        varAnima->setEndValue(_expandedWidth);
        varAnima->start();
    }
    else if(e->type() == QEvent::HoverLeave)
    {
        if(varAnima->state() == QAbstractAnimation::Running)
            varAnima->stop();

        QSize tmp = QScrollBar::sizeHint();
        int  normalWidth = this->orientation() == Qt::Horizontal ? tmp.height() : tmp.width();
        varAnima->setStartValue(preferWidth);
        varAnima->setEndValue(normalWidth);
        varAnima->start();
    }
    return QScrollBar::event(e);
}
