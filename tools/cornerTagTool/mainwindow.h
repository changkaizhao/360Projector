#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QList>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include "dlightgraphicsview.h"
#include "dlightgraphicsscene.h"
#include "dlightgraphicsellipseitem.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QImage *image;
protected:
    void resizeEvent(QResizeEvent *newsize);
    virtual void focusOutEvent(QFocusEvent *event);

private:
    const double EPSINON;
    Ui::MainWindow *ui;
    DlightGraphicsView *graphicsView;
    QJsonArray *marksJson;
    int jsonItemCount;
    int rowAmount;
    int colAmount;
    DlightGraphicsScene *picScene;
    QGraphicsPixmapItem *graphics_0;
    DlightGraphicsEllipseItem *graphics_1;
    QList<DlightGraphicsEllipseItem *> graphics_marks;
    QString pictureName;
    QString jsonName;

    void connectSignalSlot();
    void createGraphicsView();
    void openPicture(const QString &fileName);
    bool openJson(const QString &fileName, QJsonArray* &Jsons);
    void loadMarks(QJsonArray *Jsons);
    QPointF markPosFromJson(QJsonArray *JsonArray, int order);
    QPoint markLocFromJson(QJsonArray *JsonArray, int order);
    void messageInfo(const QString &message);
    void addNewItem(QPoint location, QPointF position, int jsonIndex);

    void itemCountIncrease(int value);

private slots:
    void openFile();
    void changeSetting();
    void resizeView();
    void getFocus();
    void saveJson();

    void setMarkItemInfo(QPoint location, QPointF position, int jsonIndex);

    void markXChange(double value);
    void markYChange(double value);
    void markRChange(int value);
    void markCChange(int value);

    void addPointItem();
    void deletePointItem();

    void setJsonIndex(int index);

signals:
    void sendMarkR(int R);
    void sendMarkC(int C);
    void sendMarkX(double X);
    void sendMarkY(double Y);
    void sendMarkPosition(QPointF position);
};

#endif // MAINWINDOW_H
