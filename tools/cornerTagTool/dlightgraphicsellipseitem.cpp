#include "dlightgraphicsellipseitem.h"

DlightGraphicsEllipseItem::DlightGraphicsEllipseItem()
{

}

QPointF DlightGraphicsEllipseItem::itemRawPos() const
{
    return itemRawPos_v;
}

void DlightGraphicsEllipseItem::setItemRawPos(const QPointF &pointPos)
{
    itemRawPos_v = pointPos;
}

QPoint DlightGraphicsEllipseItem::itemLocation() const
{
    return itemLocation_v;
}

void DlightGraphicsEllipseItem::setItemLocation(const QPoint &pointLocation)
{
    itemLocation_v = pointLocation;
}

int DlightGraphicsEllipseItem::itemJsonIndex() const
{
    return jsonIndex_v;
}
void DlightGraphicsEllipseItem::setItemJsonIndex(int newIndex)
{
    jsonIndex_v = newIndex;
}
