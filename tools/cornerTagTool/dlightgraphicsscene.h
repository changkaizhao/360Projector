#ifndef DLIGHTGRAPHICSSCENE_H
#define DLIGHTGRAPHICSSCENE_H
#include <QGraphicsScene>
#include <QGraphicsSceneEvent>
#include <QGraphicsSceneWheelEvent>

class DlightGraphicsScene : public QGraphicsScene
{
public:
    DlightGraphicsScene(QObject *parent = Q_NULLPTR);

protected:
    virtual void wheelEvent(QGraphicsSceneWheelEvent *event);
};

#endif // DLIGHTGRAPHICSSCENE_H
