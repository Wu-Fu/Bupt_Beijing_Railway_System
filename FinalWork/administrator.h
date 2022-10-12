#ifndef ADMINISTRATOR_H
#define ADMINISTRATOR_H

#include <QMainWindow>

namespace Ui {
class Administrator;
}

class Administrator : public QMainWindow
{
    Q_OBJECT

public:
    explicit Administrator(QWidget *parent = nullptr);
    ~Administrator();
    bool isLogin;
signals:
    void addInfo(QString point1,QString point2,QString line,QString distance);
    void delInfo(QString point1,QString point2);
private:
    Ui::Administrator *ui;
};

#endif // ADMINISTRATOR_H
