#ifndef WORKWINDOW_H
#define WORKWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QtDebug>
#include <QPushButton>
#include <QString>
#include <QMessageBox>

namespace Ui {
class WorkWindow;
}

class WorkWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit WorkWindow(QWidget *parent = nullptr);
    ~WorkWindow();
    void paintEvent(QPaintEvent *);
signals:
    void back();
    void sendStartPoint(QString data);
    void sendEndPoint(QString data);
    void startAdministrator();
private:
    Ui::WorkWindow *ui;
};

#endif // WORKWINDOW_H
