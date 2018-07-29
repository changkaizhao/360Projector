#include "dlightgraphicsscene.h"

DlightGraphicsScene::DlightGraphicsScene(QObject *parent):
    QGraphicsScene(parent)
{

}

void DlightGraphicsScene::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    Q_UNUSED(event)
}

