#include "resultshow.h"
#include "ui_resultshow.h"
#include <administrator.h>
#include <workwindow.h>

//建路函数，基于邻接表
void ResultShow::init(std::string start, std::string end, double distance, std::string line)
{
    Node* startNode = &node[subwayNum[start]];
    Node* endNode = &node[subwayNum[end]];
    Edge *temp = new Edge;
    temp->line = line;
    temp->nextNode = endNode->id;
    temp->distance = distance;
    temp->nextEdge = startNode->firstEdge;
    temp->time = (distance / velocity[line]) * 60.0;                    //计算时间 单位 min
    startNode->firstEdge = temp;
}
ResultShow::ResultShow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ResultShow)
{
    ui->setupUi(this);                                                  //初始化窗口信息
    setFixedSize(600, 500);
    setWindowTitle("查询结果");
    setWindowIcon(QIcon(":/source/icon.png"));
    QFont ft;
    ft.setPointSize(20);
    ft.setFamily("微软雅黑");
    ui->endPoint->setFont(ft);
    ui->startPoint->setFont(ft);
    ui->time->setFont(ft);
    ui->distance->setFont(ft);
    ui->exchange->setFont(ft);
    ui->numOfSubway->setFont(ft);
    ui->textBrowser->setStyleSheet("background-color:rgba(244,244,244,120)");
    ui->textBrowser_2->setStyleSheet("background-color:rgba(244,244,244,120)");
    ui->textBrowser_3->setStyleSheet("background-color:rgba(244,244,244,120)");

    Administrator *administrator = new Administrator;
    administrator->hide();
    connect(this,&ResultShow::openAdministrator,administrator,[=](){     //接收主窗口的信息，用于打开管理系统
        administrator->show();
    });
    //用于接收管理系统传来的信息
    connect(administrator, SIGNAL(addInfo(QString, QString, QString, QString)), this, SLOT(receiveInfoForAdd(QString, QString, QString, QString)));
    connect(administrator, SIGNAL(delInfo(QString, QString)), this ,SLOT(receiveInfoForDel(QString, QString)));
    searchType = 1;
    subwayNum.clear();
    countOfSubway = 0;
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
    std::ifstream FILE;                                                  //读入地铁全部信息
    std::ifstream NODEFILE;
    std::ifstream LINEFILE;
    FILE.open("./source/subway.txt");                         //文件位置，以exe文件为出发点确认地址
    NODEFILE.open("./source/node.txt");
    LINEFILE.open("./source/velocity.txt");
    if(!FILE || !NODEFILE || !LINEFILE)
        QMessageBox::critical(this, tr("打开文件"), tr("打开文件错误"));
    int numOfSubway = 0;
    std::string nameOfNode;
    //通过STL的map容器，将地铁站点名与编号相连接
    while(NODEFILE >> nameOfNode)
    {
        subwayNum.insert(std::pair<std::string,int>(nameOfNode,countOfSubway++));
        Node temp;
        temp.id = subwayNum[nameOfNode];
        node.push_back(temp);
    }
    FILE >> numOfSubway;
    for(int i = 1; i <= numOfSubway; i ++)                              //读入地铁速度信息
    {
        std::string line;
        double speed;
        LINEFILE >> line;
        LINEFILE >> speed;
        velocity.insert(std::pair<std::string,double>(line,speed/2.0)); //平均速度取最高速度的一半
    }
    for(int i = 1; i <= numOfSubway; i ++)                              //读取地铁线路信息
    {
        std::string line;
        FILE >> line;
        int numOfNode = 0;
        FILE >> numOfNode;
        std::string startNode;
        double distance;
        FILE >> startNode >> distance;
        for(int j = 1;j < numOfNode; j++)
        {
            std::string endNode;
            FILE >> endNode;
            init(startNode, endNode, distance, line);
            init(endNode, startNode, distance, line);                   //正反重复建表来建立无向图
            FILE >> distance;
            startNode = endNode;
        }
    }
}

ResultShow::~ResultShow()                                               //析构函数，用于删除邻接表
{
    for(int i = 0; i < countOfSubway; i++)
    {
        Edge *p = node[i].firstEdge;
        while(p)
        {
            Edge *q = p->nextEdge;
            delete p;
            p = q;
        }
    }
    delete ui;
}
bool ResultShow::isInNode(std::string nameOfNode) const                 //通过在map中寻找站点名来判断站点是否存在
{
    if(nameOfNode == "") return false;
    if(subwayNum.find(nameOfNode) == subwayNum.end())
        return false;
    return true;
}
std::string ResultShow::QSToS(QString name) const                       //将QString转为String
{
    std::string nameOfNode = name.toStdString();
    QTextCodec *code = QTextCodec::codecForName("gb18030");
    if(code)
        nameOfNode= code->fromUnicode(name).data();
    return nameOfNode;
}
void ResultShow::minDistance(std::string start, std::string end) const  //通过dijkstra计算最小路径
{
    int startPoint = subwayNum[start];
    int endPoint = subwayNum[end];
    double dis[1000]={};                                                //dis用于储存start点到该点的最小路径
    int vis[1000]={}, path[1000];
    memset(path, 0x7fffffff, sizeof(path));
    for(int i = 0;i < 1000;i ++)                                        //将距离信息设为极大值
        dis[i] = 1000000000.0;
    std::priority_queue<trainDis, std::vector<trainDis>,std::greater<trainDis>> subwayQueue;
                                                                        //通过优先队列优化dijkstra算法
    dis[startPoint] = 0;
    subwayQueue.push({0,startPoint});
    while (!subwayQueue.empty())
    {
        int x = subwayQueue.top().x;
        subwayQueue.pop();
        if(vis[x] == 1)
            continue;
        vis[x] = 1;
        Edge *edge = node[x].firstEdge;
        while(edge)
        {
            int y = edge->nextNode;
            double w = edge->distance;                                  //以距离作为权值
            if(dis[y] > dis[x] + w)
            {
                dis[y] = dis[x] + w;
                path[y] = x;                                            //记录路径
                subwayQueue.push({dis[y], y});
            }
            edge = edge->nextEdge;
        }
    }
    std::stack<int> reverse;
    //由于path中的路径储存为逆序，所以利用栈的先进先出特性将其转为正序
    int p = endPoint;
    while (path[p] != -1)
    {
        reverse.push(p);
        p = path[p];
    }
    reverse.push(p);
    std::vector<int> minPath;                                           //逆转路径
    while(!reverse.empty())
    {
        minPath.push_back(reverse.top());
        reverse.pop();
    }
    finalAnswer(minPath);                                               //根据路径输出最终结果
}
void ResultShow::minTime(std::string start, std::string end) const      //通过dijkstra计算最小时间
{
    int startPoint = subwayNum[start];
    int endPoint = subwayNum[end];
    int time[1000]={};                                                  //time用于储存start点到该点的最小时间
    int vis[1000]={}, path[1000];
    memset(path, 0x7fffffff, sizeof(path));
    for(int i = 0;i < 1000;i ++)
        time[i] = 1000000000;
    std::priority_queue<trainTime, std::vector<trainTime>,std::greater<trainTime>> subwayQueue;
                                                                        //通过优先队列优化dijkstra算法
    time[startPoint] = 0;
    subwayQueue.push({0,startPoint});
    while (!subwayQueue.empty())
    {
        int x = subwayQueue.top().x;
        subwayQueue.pop();
        if(vis[x] == 1)
            continue;
        vis[x] = 1;
        Edge *edge = node[x].firstEdge;
        while(edge)
        {
            int y = edge->nextNode;
            int w = edge->time;                                         //以时间作为权值
            if(time[y] > time[x] + w)
            {
                time[y] = time[x] + w;
                path[y] = x;                                            //记录路径
                subwayQueue.push({time[y], y});
            }
            edge = edge->nextEdge;
        }
    }
    std::stack<int> reverse;
    //由于path中的路径储存为逆序，所以利用栈的先进先出特性将其转为正序
    int p = endPoint;
    while (path[p] != -1)
    {
        reverse.push(p);
        p = path[p];
    }
    reverse.push(p);
    std::vector<int> minPath;                                           //逆转路径
    while(!reverse.empty())
    {
        minPath.push_back(reverse.top());
        reverse.pop();
    }
    finalAnswer(minPath);                                               //根据路径输出最终结果
}
void ResultShow::dfs(int next, std::string line, int exchange) const    //dfs 用于寻找最少换乘路径
{
    if (exchange > minOfExchange || visInExchange[next] == 1)           //剪枝，防止dfs过深
        return;
    if (next == endInExchange)                                          //到达终点时
    {
        if(exchange < minOfExchange)                                    //根据exchange与min的比较判断是否更新
        {
            minPathInExchange.clear();
            minPathInExchange = pathInExchange;                         //记录最小换乘的路径
            minOfExchange = std::min(exchange,minOfExchange);
        }

        return;
    }
    visInExchange[next] = 1;                                            //将该点标为已达到
    Edge *p = node[next].firstEdge;
    pathInExchange.push_back(next);                                     //加入此次dfs的路径
    while(p)                                                            //遍历每条边
    {
        if(p->line != line)                                             //当前后两条边line不同时，exchange + 1
            dfs(p->nextNode,p->line,exchange + 1);
        else
            dfs(p->nextNode,p->line,exchange);
        p = p->nextEdge;
    }
    visInExchange[next] = 0;                                            //回溯
    pathInExchange.pop_back();
}
void ResultShow::minExchange(std::string start, std::string end) const  //计算最小换乘数
{
    minOfExchange = 10;
    memset(visInExchange,0,sizeof(visInExchange));
    int startInExchange = subwayNum[start];
    endInExchange = subwayNum[end];
    Edge *p = node[startInExchange].firstEdge;
    pathInExchange.push_back(startInExchange);
    while(p)                                                            //遍历出发点的每一条边
    {
        dfs(p->nextNode, p->line, 0);
        p = p->nextEdge;
    }
    minPathInExchange.push_back(endInExchange);
    finalAnswer(minPathInExchange);
}
void ResultShow::on_searchWay_currentIndexChanged(const QString &arg1)  //用于选择不同的方案
{
    QString str = ui->searchWay->currentText();
    std::string startNode = QSToS(ui->startPoint->text());
    std::string endNode = QSToS(ui->endPoint->text());
    if(str == "最快时间")
        minTime(startNode, endNode);
    else if(str == "最短距离")
        minDistance(startNode, endNode);
    else
        minExchange(startNode, endNode);
    qDebug()<<"Text:"<< str;
}
void ResultShow::finalAnswer(std::vector<int> path) const                   //根据路径，寻找综合信息
{
    int time = 0;                                                           //计算时间
    int exchange = 0;                                                       //计算换乘数
    double distance = 0.0;                                                  //计算距离
    std::vector<int> exchangePath;                                          //记录换乘点的位置
    std::vector<std::string> linePath;                                      //记录换乘点的线路
    std::string line;                                                       //记录线路
    for(auto it : path)
    {
        qDebug() << QString::fromLocal8Bit(findNameOfNode(it).c_str());
    }
    exchangePath.push_back(path[0]);
    for(int i = 0;i < path.size() - 1;i ++)                                 //遍历路径上每个点
    {
        Edge* edge = node[path[i]].firstEdge;
        while(edge)                                                         //遍历每个点的每条边
        {
            if(edge->nextNode == path[i + 1])                               //当找到路径上的下个点时，更新数据
            {
                time += edge->time;
                distance += edge->distance;
                if(line != "" && line != edge->line)                        //如果两点不在一条线路上，标记换乘
                {
                    exchange ++;
                    exchangePath.push_back(path[i]);                        //记录换乘站
                    linePath.push_back(line);                               //记录换乘线路
                }
                line = edge->line;
                break;
            }
            edge = edge->nextEdge;
        }
    }
    ui->time->setNum(time);                                                 //在结果框显示信息
    ui->distance->setText(QString::number(distance,'f',3));
    ui->exchange->setNum(exchange);
    ui->numOfSubway->setNum(int(path.size()));
    exchangePath.push_back(path[path.size()-1]);
    linePath.push_back(line);
    QString finalPath;                                                      //围绕换乘点，计算最终线路
    for(int i = 0;i < exchangePath.size() - 1;i ++)
    {
        finalPath += "从";
        finalPath += QString::fromLocal8Bit(findNameOfNode(exchangePath[i]).c_str());
        finalPath += "搭";
        finalPath += QString::fromLocal8Bit(linePath[i].c_str());
        finalPath += "到";
        finalPath += QString::fromLocal8Bit(findNameOfNode(exchangePath[i + 1]).c_str());
        finalPath += "\n";
    }
    QFont ft;
    ft.setFamily("微软雅黑");                                                //设置信息格式
    ft.setPointSize(10);
    ui->Path->setFont(ft);
    ui->Path->setText(finalPath);
}
std::string ResultShow::findNameOfNode(int num) const                       //根据站点名寻找对应的编号
{
    for(auto it : subwayNum)
        if(it.second == num)
            return it.first;
}
void ResultShow::destroy(std::string start, std::string end)                //删除路径
{
    int startNode = subwayNum[start];
    int endNode = subwayNum[end];
    if(!isInNode(start) || !isInNode(end))                                  //先判断路径是否存在
    {
        QMessageBox::critical(this, tr("删除线路"), tr("未找到该点,请重新确认信息！"));
    }
    bool success = false;
    Edge *p = node[startNode].firstEdge;
    while(p)                                                                //遍历站点的所有路线
    {
        if(p->nextNode == endNode && p->nextEdge!=nullptr)                  //找到指定路线
        {
            Edge *newNext = p->nextEdge->nextEdge;                          //更新邻接表信息
            delete p->nextEdge;                                             //删除该点
            p->nextEdge = newNext;
            success = true;
            break;
        }
        p = p->nextEdge;
    }
    Edge *q = node[endNode].firstEdge;
    while(q)                                                                //正反交换重复一次
    {
        if(q->nextNode == startNode && q->nextEdge != nullptr)
        {
            Edge *newNext = q->nextEdge->nextEdge;
            delete q->nextEdge;
            q->nextEdge = newNext;
            success = true;
            break;
        }
        q = q->nextEdge;
    }
    if(success == true)                                                     //显示成功弹窗
        QMessageBox::information(this,
            tr("删除线路"),
            tr("成功删除"),
            QMessageBox::Ok | QMessageBox::Cancel,
            QMessageBox::Ok);
    else
        QMessageBox::critical(this, tr("删除线路"), tr("删除失败,请重新确认信息！"));
}
void ResultShow::addNewLine(std::string start, std::string end, double distance, std::string line)
                                                                            //添加新的路线
{
    if(!isInNode(start))                                                    //若站点不存在则添加新的站点
    {
        subwayNum.insert(std::pair<std::string,int>(start,countOfSubway++));
        Node temp;
        temp.id = subwayNum[start];
        node.push_back(temp);
        QMessageBox::information(this,
            tr("新增站点"),
            tr("成功新增站点"),
            QMessageBox::Ok | QMessageBox::Cancel,
            QMessageBox::Ok);
    }
    if(!isInNode(end))
    {
        subwayNum.insert(std::pair<std::string,int>(end,countOfSubway++));
        Node temp;
        temp.id = subwayNum[end];
        node.push_back(temp);
        QMessageBox::information(this,
            tr("新增站点"),
            tr("成功新增站点"),
            QMessageBox::Ok | QMessageBox::Cancel,
            QMessageBox::Ok);
    }
    init(start, end, distance, line);                                       //建图
    init(end, start, distance, line);
    QMessageBox::information(this,
        tr("新增站点"),
        tr("成功新增线路"),
        QMessageBox::Ok | QMessageBox::Cancel,
        QMessageBox::Ok);
}
void ResultShow::receiveAdministrator()                                     //用于接受打开管理系统的信息
{
    emit openAdministrator();
}
void ResultShow::receiveStartPoint(QString data)                            //接收主窗口传来的起始点信息
{
    QFont ft;
    ft.setPointSize(20);
    ui->startPoint->setFont(ft);
    ui->startPoint->setText(data);
}
void ResultShow::receiveEndPoint(QString data)
{
    QFont ft;
    ft.setPointSize(20);
    ui->endPoint->setFont(ft);
    ui->endPoint->setText(data);
}
void ResultShow::receiveInfoForAdd(QString point1, QString point2, QString distance, QString line)
                                                                            //接收管理系统传来的信息
{
    std::string start = QSToS(point1);
    std::string end = QSToS(point2);
    std::string disInfo = QSToS(distance);
    double dis = std::atoi(disInfo.c_str());
    std::string lineInfo = QSToS(line);
    addNewLine(start,end,dis,lineInfo);

}
void ResultShow::receiveInfoForDel(QString point1, QString point2)
{
    std::string start = QSToS(point1);
    std::string end = QSToS(point2);
    destroy(start, end);
}
void ResultShow::paintEvent(QPaintEvent *)                                  //绘制背景图
{
    QPainter painter(this);
    QPixmap pix;
    pix.load(":/source/BgForResult.jpg");
    painter.drawPixmap(0,0,pix.width(),pix.height(),pix);
}
