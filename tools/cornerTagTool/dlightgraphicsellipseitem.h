#ifndef DLIGHTGRAPHICSELLIPSEITEM_H
#define DLIGHTGRAPHICSELLIPSEITEM_H
#include <QGraphicsEllipseItem>
#include <QPointF>
#include <QPoint>


class DlightGraphicsEllipseItem : public QGraphicsEllipseItem
{
public:
    DlightGraphicsEllipseItem();

    QPointF itemRawPos() const;
    void setItemRawPos(const QPointF &pointPos);

    QPoint itemLocation() const;
    void setItemLocation(const QPoint &pointLocation);

    int itemJsonIndex() const;
    void setItemJsonIndex(int newIndex);

private:
    QPointF itemRawPos_v;
    QPoint itemLocation_v;
    int jsonIndex_v;
};

#endif // DLIGHTGRAPHICSELLIPSEITEM_H
