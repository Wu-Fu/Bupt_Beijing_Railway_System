#ifndef SUBWAYMAP_H
#define SUBWAYMAP_H

#include <QObject>
#include <QBoxLayout>
#include <QGraphicsView>
#include <qevent.h>

class SubwayMap : public QGraphicsView
{
    Q_OBJECT
public:
    SubwayMap(QWidget *parent = nullptr);
    ~SubwayMap();
protected:
    void wheelEvent(QWheelEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
public slots:
    void zoomIn();
    void zoomOut();
    void zoom(float scaleFactor);
    void translate(QPointF delta);

private:
    bool isTranslate;
    QPoint oldPos;
    QString imagePath;
    QGraphicsScene *scene;
    QGraphicsPixmapItem *imageItem;

signals:

};

#endif // SUBWAYMAP_H
