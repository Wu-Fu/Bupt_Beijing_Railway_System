#ifndef RESULTSHOW_H
#define RESULTSHOW_H

#include <QWidget>
#include <map>
#include <QFile>
#include <vector>
#include <QString>
#include <QTextCodec>
#include <QMainWindow>
#include <QDebug>
#include <fstream>
#include <QApplication>
#include <cstring>
#include <queue>
#include <stack>
#include <cmath>
#include <QString>
#include <QMessageBox>
#include <QPainter>
namespace Ui {
class ResultShow;
}

class ResultShow : public QMainWindow
{
    Q_OBJECT

public:
    ResultShow(QWidget *parent = nullptr);
    ~ResultShow();
    struct Edge{
        int nextNode;
        std::string line;
        double distance;
        int time;
        Edge* nextEdge = nullptr;
    };
    struct Node{
        int id;
        Edge* firstEdge = nullptr;
    };
    struct trainDis{
        double dis;
        int x;
        bool operator>(const trainDis& b) const{
            return this->dis > b.dis;
        }
    };
    struct trainTime{
        int time;
        int x;
        bool operator>(const trainTime& b) const{
            return this->time > b.time;
        }
    };
    int searchType;
    mutable int minOfExchange;                                          //用于记录最小换乘次数
    mutable int visInExchange[1000];                                    //用于dfs时标记vis的全局数组
    mutable int endInExchange;                                          //用于dfs时记录终点的全局变量
    mutable std::vector<int> pathInExchange;                            //用于dfs时记录路径
    mutable std::vector<int> minPathInExchange;                         //用于记录dfs时的最小换乘路径
    mutable std::map<std::string, double> velocity;                     //velocity用于储存地铁线路及其对应的时间
    mutable std::map<std::string, int> subwayNum;                       //subwayNum用于储存地铁站点及其对应的编号
    mutable std::vector<Node> node;                                     //node用于建立无向图，其下标即为对应的站点编号

    mutable int countOfSubway;
    void init(std::string start,std::string end,double distance,std::string line);
                                                                        //用于建图
    std::string QSToS(QString name) const;                              //将QString转为string
    std::string findNameOfNode(int num) const;                          //根据编号来找地铁站
    bool isInNode(std::string nameOfNode) const;                        //用于判断是否站点是否在线路中
    void destroy(std::string start, std::string end);                   //删除路线
    void addNewLine(std::string start, std::string end,double distance,std::string line);//添加路线
    void minTime(std::string start, std::string end) const;             //判断最小时间
    void minDistance(std::string start, std::string end) const;         //判断最短距离
    void minExchange(std::string start, std::string end) const;         //判断最少换乘数
    void dfs(int next,std::string line, int exchange) const;            //dfs 用于寻找最少换乘路径
    void finalAnswer(std::vector<int> path) const;                      //在得出该方案的路径后，寻找综合信息
    void paintEvent(QPaintEvent *);
private:
    Ui::ResultShow *ui;
private slots:
    void on_searchWay_currentIndexChanged(const QString &arg1);
    void receiveStartPoint(QString data);
    void receiveEndPoint(QString data);
    void receiveAdministrator();
    void receiveInfoForAdd(QString point1, QString point2, QString distance,QString line);
    void receiveInfoForDel(QString point1, QString point2);
signals:
    void openAdministrator();
};

#endif // RESULTSHOW_H
