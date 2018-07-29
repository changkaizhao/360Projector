#include "dlightgraphicsview.h"
#include <QWheelEvent>
#include <QKeyEvent>
#include <QScrollBar>
#include <QGraphicsPixmapItem>
#include <QDebug>

DlightGraphicsView::DlightGraphicsView(QGraphicsScene *scene, QWidget *parent):
    QGraphicsView(scene, parent),
    keyShiftPressing(false),
    keyCtrlPressing(false),
    markpointMoving(false),
    markItem_processing(0)
{
    //picture = qgraphicsitem_cast<QGraphicsPixmapItem *>(items().last());
}

void DlightGraphicsView::initialVariant()
{
    keyShiftPressing = false;
    keyCtrlPressing = false;
    markpointMoving = false;
    markItem_processing = 0;
    picture = qgraphicsitem_cast<QGraphicsPixmapItem *>(items().last());
}

void DlightGraphicsView::wheelEvent(QWheelEvent *event)
{
    int newValue;//new scroll value *Or* new scale value;
    qreal newScale;

    if(keyShiftPressing){
        newValue = horizontalScrollBar()->value() - event->angleDelta().y()/2;
        if(horizontalScrollBar()->maximum() < newValue){
            newValue = horizontalScrollBar()->maximum();
        }

        horizontalScrollBar()->setValue(newValue);
    }else if(keyCtrlPressing){
        QGraphicsPixmapItem *picItem;
        DlightGraphicsEllipseItem *markItem;
        QPointF cursorInScene;

        cursorInScene = mapToScene(event->pos());
        //qDebug() << cursorInScene;

        picItem = picture;

        if(event->angleDelta().y() < 0)
            newScale = picItem->scale() - 0.1;
        else
            newScale = picItem->scale() + 0.1;
        if(newScale < 0.5){
            newScale = 0.5;
        }else if(newScale > 3.0){
            newScale = 3.0;
        }

        scene()->setSceneRect(0,
                  0,
                  picItem->pixmap().rect().width()*newScale,
                  picItem->pixmap().rect().height()*newScale);
        //centerOn(picItem);

        picItem->setTransformOriginPoint(
                    picItem->pixmap().width()/2,
                    picItem->pixmap().height()/2
                    );
        picItem->setScale(newScale);
        picItem->setTransformOriginPoint(0,0);

        for(int i=0; i<items().size()-1; i++){
            markItem = qgraphicsitem_cast<DlightGraphicsEllipseItem *>(items().at(i));
            markItem->setRect(markItem->itemRawPos().x()*newScale-10,
                              markItem->itemRawPos().y()*newScale-10,
                              markItem->rect().width(),
                              markItem->rect().height());
        }
        //picScale = newScale;
    }
    else{
        //qDebug("v scoll %d,%d",verticalScrollBar()->maximum(),event->angleDelta().y());
        QGraphicsView::wheelEvent(event);
    }

}

void DlightGraphicsView::keyPressEvent(QKeyEvent *event)
{
    if(0 != markItem_processing){
        bool rePaintMark = false;
        QPointF pointPos;

        pointPos = markItem_processing->itemRawPos();

        if(event->key() == Qt::Key_Left){
            pointPos.setX(pointPos.x()-0.1);
            rePaintMark = true;
        }else if(event->key() == Qt::Key_Right){
            pointPos.setX(pointPos.x()+0.1);
            rePaintMark = true;
        }else if(event->key() == Qt::Key_Up){
            pointPos.setY(pointPos.y()-0.1);
            rePaintMark = true;
        }else if(event->key() == Qt::Key_Down){
            pointPos.setY(pointPos.y()+0.1);
            rePaintMark = true;
        }

        if(rePaintMark){
            setMarkItemPos(pointPos);
        }

        //send signals
        emit changeMarkItem(markItem_processing->itemLocation(),
                             markItem_processing->itemRawPos(),
                            markItem_processing->itemJsonIndex());
    }

    if(event->key() == Qt::Key_Shift){
        keyShiftPressing = true;
        qDebug("pressed shift");
    }else if(event->key() == Qt::Key_Control){
        keyCtrlPressing = true;
        qDebug("pressed ctrl");
    }else if(event->key() == Qt::Key_S){
        if(keyCtrlPressing){
            emit saveMarks();
        }
    }
}

void DlightGraphicsView::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Shift){
        keyShiftPressing = false;
        qDebug("release shift");
    }else if(event->key() == Qt::Key_Control){
        keyCtrlPressing = false;
        qDebug("release ctrl");
    }
}

void DlightGraphicsView::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        DlightGraphicsEllipseItem *markItem;
        markItem = qgraphicsitem_cast<DlightGraphicsEllipseItem *>(itemAt(event->x(),event->y()));
        if(0 != markItem){
            setMarkItem(markItem);
            //send signals
            emit changeMarkItem(markItem_processing->itemLocation(),
                                 markItem_processing->itemRawPos(),
                                markItem_processing->itemJsonIndex());

            markpointMoving = true;
        }else{
            if(0 != markItem_processing){
                markItem_processing->setBrush(QBrush(Qt::blue));
                markItem_processing = 0;
            }else{
                QGraphicsView::mousePressEvent(event);
            }
        }
    }
}

void DlightGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    if(markpointMoving == true){
        markpointMoving = false;
    }else{
        QGraphicsView::mouseReleaseEvent(event);
        picture->setCursor(Qt::CrossCursor);
    }
}


void DlightGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    QPointF mousePos;
    if(markpointMoving == true && markItem_processing != 0){
        picture->setCursor(Qt::CrossCursor);

        mousePos = mapToScene(event->pos());
        mousePos.setX(mousePos.x()/picture->scale());
        mousePos.setY(mousePos.y()/picture->scale());

        setMarkItemPos(mousePos);

        //send signals
        emit changeMarkItem(markItem_processing->itemLocation(),
                             markItem_processing->itemRawPos(),
                            markItem_processing->itemJsonIndex());
    }else{
        QGraphicsView::mouseMoveEvent(event);
    }
}

void DlightGraphicsView::setMarkItemPos(QPointF position)
{
    if(0 != markItem_processing){
        //check position range
        if(position.x() < 0 ||
           position.y() < 0 ||
           position.x() > picture->pixmap().size().width() ||
           position.y() > picture->pixmap().size().height()) return;

        markItem_processing->setItemRawPos(position);
        markItem_processing->setRect(markItem_processing->itemRawPos().x()*picture->scale()-10,
                                     markItem_processing->itemRawPos().y()*picture->scale()-10,
                                     markItem_processing->rect().width(),
                                     markItem_processing->rect().height());
    }
}

void DlightGraphicsView::setMarkItem(DlightGraphicsEllipseItem *markItem)
{   
    if(0 != markItem_processing){
        markItem_processing->setBrush(QBrush(Qt::blue));
        markItem_processing = 0;
    }

    markItem_processing = markItem;
    if(markItem_processing != 0){
        markItem_processing->setBrush(QBrush(Qt::cyan));
    }else{
        markpointMoving = false;
    }
}

void DlightGraphicsView::followCursor()
{
    markpointMoving = true;
}

DlightGraphicsEllipseItem *DlightGraphicsView::markItem() const
{
    return markItem_processing;
}
