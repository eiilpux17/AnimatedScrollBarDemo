#ifndef WIDGET_H
#define WIDGET_H

#include <QFrame>
#include <QScrollBar>

class QVariantAnimation;

namespace Ui {
class Widget;
}

class Widget : public QFrame
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    bool eventFilter(QObject *watched, QEvent *event);

private slots:
    void on_btnCommit_clicked();

private:
    void removeSelectedPaths();
    void removeAllPaths();

private:
    Ui::Widget *ui;
    QAction *removeSelectionAct;
    QAction *removeAllAct;
};

class AnimatedScrollBar : public QScrollBar
{
    Q_OBJECT
    Q_PROPERTY(int expandedWidth READ expandedWidth WRITE setExpandedWidth)
public:
    AnimatedScrollBar(QWidget *parent = Q_NULLPTR);
    QSize sizeHint() const;
    int expandedWidth(){
        return _expandedWidth;
    }
    void setExpandedWidth(int val);

protected:
    bool event(QEvent *e);
private:
    QVariantAnimation *varAnima;
    int preferWidth = -1;
    int _expandedWidth = 15;
};

#endif // WIDGET_H
