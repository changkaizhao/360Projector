#ifndef DLIGHTGRAPHICSVIEW_H
#define DLIGHTGRAPHICSVIEW_H
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QList>
#include <QPointF>
#include <QPoint>
#include "dlightgraphicsellipseitem.h"

class DlightGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    DlightGraphicsView(QGraphicsScene *scene, QWidget *parent = Q_NULLPTR);

    void initialVariant();
    void setMarkItem(DlightGraphicsEllipseItem *markItem);
    DlightGraphicsEllipseItem *markItem() const;
    void followCursor();

protected:
    virtual void wheelEvent(QWheelEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);

private:
    bool keyShiftPressing;
    bool keyCtrlPressing;
    bool markpointMoving;

    DlightGraphicsEllipseItem *markItem_processing;
    QGraphicsPixmapItem *picture;


signals:
    void changeMarkItem(QPoint location, QPointF position, int jsonIndex);
    void saveMarks();

private slots:
    void setMarkItemPos(QPointF position);

};

#endif // DLIGHTGRAPHICSVIEW_H
