#include "subwaymap.h"
#include <QGraphicsPixmapItem>
#include <QPixmap>
SubwayMap::SubwayMap(QWidget *parent) :
    QGraphicsView(parent)
{
    isTranslate = false;
    scene = new QGraphicsScene();
    imageItem = new QGraphicsPixmapItem();
    imagePath = ":/source/luwangtu.png";

    scene->addItem(imageItem);                                          //初始化图片框信息
    setScene(scene);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setRenderHint(QPainter::Antialiasing);

    setSceneRect(INT_MIN/2,INT_MIN/2,INT_MAX,INT_MAX);
    setTransformationAnchor(QGraphicsView::AnchorViewCenter);
    centerOn(0, 0);

    QImage image;
    image.load(imagePath);                                              //加载图片
    imageItem->setPixmap(QPixmap::fromImage(image));                    //显示图片
    QPoint newCenter(image.width()/2,image.height()/2);
    centerOn(newCenter);
    show();
}
SubwayMap::~SubwayMap()
{
    scene->deleteLater();
    delete  imageItem;
}
void SubwayMap::wheelEvent(QWheelEvent *event)                          //用于检测鼠标滚轮信息，完成放缩操作
{
    QPoint scrollAmount = event->angleDelta();
    scrollAmount.y() > 0 ? zoomIn(): zoomOut();
}
void SubwayMap::mouseMoveEvent(QMouseEvent *event)                      //用于检测鼠标移动信息
{
    if(isTranslate)
    {
        QPointF mouseDelta = event->pos()-oldPos;
        translate(mouseDelta);
    }
    oldPos = event->pos();
    QGraphicsView::mouseMoveEvent(event);
}
void SubwayMap::mousePressEvent(QMouseEvent *event)                     //检测鼠标按压信息，用于拖动
{
    if(event->button() == Qt::LeftButton)
    {
        isTranslate = true;
        oldPos = event->pos();
    }
    QGraphicsView::mousePressEvent(event);
}
void SubwayMap::mouseReleaseEvent(QMouseEvent *event)                   //检测鼠标松开信息
{
    if(event->button() == Qt::LeftButton)
        isTranslate = false;
    QGraphicsView::mouseReleaseEvent(event);
}
void SubwayMap::zoomIn()                                                //放大
{
    zoom(1.1);
}
void SubwayMap::zoomOut()                                               //缩小
{
    zoom(0.9);
}
void SubwayMap::zoom(float scaleFactor)                                 //放缩
{
    scale(scaleFactor,scaleFactor);
}
void SubwayMap::translate(QPointF delta)                                //计算鼠标移动信息，用于图片平移
{
    int width = viewport()->rect().width();
    int height = viewport()->rect().height();
    QPoint newCenter(width/2 - delta.x() + 0.5, height/2 - delta.y() + 0.5);
    centerOn(mapToScene(newCenter));
}
