#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "common.h"
#include <QPen>
#include <QFileDialog>
#include <QString>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QMessageBox>
#include <QInputDialog>

MainWindow::MainWindow(QWidget *parent) :QMainWindow(parent),
    EPSINON(0.01),
    ui(new Ui::MainWindow),
    graphicsView(NULL),
    marksJson(NULL),
    jsonItemCount(0),
    rowAmount(ROW_AMOUNT),
    colAmount(COL_AMOUNT)
{
    ui->setupUi(this);
    ui->toolBar->addAction(ui->actionAddPoint);
    ui->toolBar->addAction(ui->actionDelPoint);

    setWindowTitle("DLight-Calibrate");
    setWindowIcon(QIcon(":/Resources/resources/dlight.png"));
    resize(800,600);

    //create GraphicsScene
    picScene = new DlightGraphicsScene(this);
    //create GraphicsView
    createGraphicsView();

    //initial Json and Picture
    pictureName = "IMG_0.png";
    jsonName = "IMG_0.json";
    openJson(jsonName, marksJson);
    openPicture(pictureName);

    //connect signals and slots
    connectSignalSlot();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::connectSignalSlot()
{
    connect(ui->actionOpen,SIGNAL(triggered(bool)),this,SLOT(openFile()));
    connect(ui->actionSave,SIGNAL(triggered(bool)),this,SLOT(saveJson()));
    connect(ui->actionSet,SIGNAL(triggered(bool)),this,SLOT(changeSetting()));

    connect(graphicsView,SIGNAL(changeMarkItem(QPoint,QPointF,int)),
            this,SLOT(setMarkItemInfo(QPoint,QPointF,int)));
    connect(this,SIGNAL(sendMarkR(int)),ui->markLocationR,SLOT(setValue(int)));
    connect(this,SIGNAL(sendMarkC(int)),ui->markLocationC,SLOT(setValue(int)));
    connect(this,SIGNAL(sendMarkX(double)),ui->markPositionX,SLOT(setValue(double)));
    connect(this,SIGNAL(sendMarkY(double)),ui->markPositionY,SLOT(setValue(double)));

    connect(this,SIGNAL(sendMarkPosition(QPointF)),graphicsView,SLOT(setMarkItemPos(QPointF)));
    connect(ui->markPositionX,SIGNAL(valueChanged(double)),this,SLOT(markXChange(double)));
    connect(ui->markPositionY,SIGNAL(valueChanged(double)),this,SLOT(markYChange(double)));
    connect(ui->markLocationR,SIGNAL(valueChanged(int)),this,SLOT(markRChange(int)));
    connect(ui->markLocationC,SIGNAL(valueChanged(int)),this,SLOT(markCChange(int)));

    connect(graphicsView,SIGNAL(saveMarks()),this,SLOT(saveJson()));

    connect(ui->actionAddPoint,SIGNAL(triggered(bool)),this,SLOT(addPointItem()));
    connect(ui->actionDelPoint,SIGNAL(triggered(bool)),this,SLOT(deletePointItem()));

    connect(ui->jsonFileIndex,SIGNAL(valueChanged(int)),this,SLOT(setJsonIndex(int)));
}
void MainWindow::createGraphicsView()
{
    graphicsView = new DlightGraphicsView(picScene,ui->centralWidget);
    graphicsView->setObjectName(QStringLiteral("graphicsView"));
    graphicsView->setGeometry(QRect(200, 10, 351, 291));
    graphicsView->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    QBrush brush(QColor(0, 0, 0, 255));
    brush.setStyle(Qt::NoBrush);
    graphicsView->setBackgroundBrush(brush);
    graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
    graphicsView->setResizeAnchor(QGraphicsView::AnchorUnderMouse);
}

void MainWindow::resizeEvent(QResizeEvent *newsize)
{
    Q_UNUSED(newsize);

    resizeView();
}

void MainWindow::focusOutEvent(QFocusEvent *event)
{
    Q_UNUSED(event);

    getFocus();
}

/*
 * resize graphics window
 */
void MainWindow::resizeView()
{
    graphicsView->setGeometry(QRect(180,10,ui->centralWidget->width()-181,ui->centralWidget->height()-10));
    ui->companyInfo->setGeometry(QRect(0,this->height()-160,180,100));
}

void MainWindow::getFocus()
{
    qDebug("get a focus");
}

void MainWindow::messageInfo(const QString &message)
{
    QMessageBox errorMessage(QMessageBox::Information,
                             tr("Message"),
                             message,
                             0,
                             this);
    errorMessage.addButton(tr("OK"), QMessageBox::AcceptRole);
    errorMessage.exec();
}

void MainWindow::openFile()
{
    QString oldPictureName;
    QString oldJsonName;
    QFileDialog::Options options;
    options |= QFileDialog::DontUseNativeDialog;

    QString selectedFilter;
    QString fileName = QFileDialog::getOpenFileName(this,
                                                     tr("OpenPicture"),
                                                     "/home/",
                                                     tr("Images (*.jpg *.jpeg *.png *.bmp);;All File (*)"),
                                                     &selectedFilter,
                                                     options);
    if(fileName.length() > 0){
        oldPictureName = pictureName;
        oldJsonName = jsonName;

        pictureName = fileName;
        fileName.truncate(pictureName.indexOf('.'));
        jsonName = fileName+".json";

        if(false == openJson(jsonName, marksJson)){
            jsonName = oldJsonName;
            pictureName = oldPictureName;
            messageInfo("打开JSON文件失败！");

            return;
        }
        openPicture(pictureName);
    }
}

void MainWindow::changeSetting()
{
    bool ret;
    int value_row;
    int value_col;
    value_row = QInputDialog::getInt(this, tr("GetRowAmount"),tr("行数量"),
                                     ROW_AMOUNT,0,1000,
                                     1,&ret);
    if(ret == true){
        value_col = QInputDialog::getInt(this, tr("GetColAmount"),tr("列数量"),
                                         COL_AMOUNT,0,1000,
                                         1,&ret);
        if(ret == true){
            saveJson();

            colAmount = value_col;
            rowAmount = value_row;
            openJson(jsonName, marksJson);
            openPicture(pictureName);
        }
    }
}

void MainWindow::openPicture(const QString &fileName)
{
    picScene->clear();

    graphics_0 = new QGraphicsPixmapItem(0);
    graphics_0->setPixmap(QPixmap(fileName));
    graphics_0->setScale(1.0);
    graphics_0->setCursor(Qt::CrossCursor);
    picScene->addItem(graphics_0);

    loadMarks(marksJson);

    picScene->setSceneRect(0,
              0,
              graphics_0->pixmap().rect().width(),
              graphics_0->pixmap().rect().height());
    graphics_0->setTransformOriginPoint(0,0);

    graphicsView->initialVariant();
}

void MainWindow::saveJson()
{
    QJsonArray markJsonArray;
    QJsonObject fileJson;
    QJsonDocument fileJsonDoc;
    int markJson_index;

    for(int i=0; i < graphics_marks.size(); i++){
        QJsonArray markValue;
        QJsonObject markObject;

        //search index Or create new one
        for(int j=0; j <= markJsonArray.count(); j++){
            if(j == markJsonArray.count()){
                QJsonObject marksObject_v;
                QJsonArray marksArray_v;
                marksObject_v.insert("index", graphics_marks.at(i)->itemJsonIndex());
                marksObject_v.insert("pts", marksArray_v);
                markJsonArray.append(marksObject_v);

                markJson_index = j;
                break;
            }
            if(markJsonArray.at(j).toObject().value("index").toInt() \
                    == graphics_marks.at(i)->itemJsonIndex()){
                markJson_index = j;
                break;
            }
        }

        QJsonObject marksObject_x;
        QJsonArray marksValue_x;

        marksObject_x = markJsonArray.takeAt(markJson_index).toObject();
        marksValue_x = marksObject_x.take("pts").toArray();

        markValue.append(graphics_marks.at(i)->itemRawPos().x());
        markValue.append(graphics_marks.at(i)->itemRawPos().y());
        markObject.insert("order",graphics_marks.at(i)->itemLocation().y() * colAmount +
                          graphics_marks.at(i)->itemLocation().x());
        markObject.insert("pos",markValue);

        marksValue_x.append(markObject);
        marksObject_x.insert("pts",marksValue_x);

        markJsonArray.append(marksObject_x);
    }

    fileJson.insert("grouplist",markJsonArray);
    fileJsonDoc.setObject(fileJson);

    //qDebug() << markJsonFile.toJson();

    QFile file;
    file.setFileName(jsonName);
    file.open(QIODevice::WriteOnly);
    file.write(fileJsonDoc.toJson());
    file.close();

    messageInfo("保存JSON文件成功");
}

bool MainWindow::openJson(const QString &fileName, QJsonArray* &Jsons)
{
    QFile fileJson;
    QJsonDocument jsonDocument;
    QJsonParseError jsonError;
    QByteArray fileRead;
    QByteArray fileReadOne;
    QJsonObject jsonTemp;
    QJsonArray jsonArray;

    if(Jsons != NULL){
        delete Jsons;
    }
    Jsons = new QJsonArray();

    fileJson.setFileName(fileName);
    if(!fileJson.exists()){
        //create File Or Not
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, tr("Message"),
                           "是否新建Json文件？",
                            QMessageBox::Yes | QMessageBox::No);
        if(reply == QMessageBox::Yes){
            bool ret;
            int newIndex = QInputDialog::getInt(this, tr("GetIndex"),tr("Index"),
                                                300,300,399,
                                                1,&ret);

            fileJson.open(QIODevice::ReadWrite);
        }else{
            return false;
        }
    }
    fileJson.close();
    if(false == fileJson.open(QIODevice::ReadOnly)){
        qDebug() << "json file open fail!";
        return false;
    }

    fileReadOne = fileJson.read(8192);
    fileRead.append( fileReadOne );
    for(;fileReadOne.length() >= 8192;){
        fileReadOne.clear();
        fileReadOne = fileJson.read(8192);
        fileRead.append( fileReadOne );
    }
    fileJson.close();
    //************************************************************//
    jsonDocument = QJsonDocument::fromJson(fileRead, &jsonError);
    if(jsonError.error == QJsonParseError::NoError){
        jsonTemp = jsonDocument.object();
        //
        jsonArray = jsonTemp.value("grouplist").toArray();
        //jsonTemp = jsonArray.first().toObject();
        //
        //jsonArray = jsonTemp.value("pts").toArray();
        //jsonTemp = jsonArray.first().toObject();
        //
        *Jsons = jsonArray;
    }else{
        qDebug() << "json Error!!!";
    }

    return true;
}

void MainWindow::loadMarks(QJsonArray *Jsons)
{
    QPointF markPos;
    QPoint markLoc;

    graphics_marks.clear();
    jsonItemCount = 0;
    itemCountIncrease(0);

    QJsonArray Json;
    int jsonIndex;
    for(int j=0; j < Jsons->count(); j++){

        jsonIndex = Jsons->at(j).toObject().value("index").toInt();
        Json = Jsons->at(j).toObject().value("pts").toArray();

        for(int i=0; i < Json.count(); i++){
            markPos = markPosFromJson(&Json, i);
            markLoc = markLocFromJson(&Json, i);
            addNewItem(markLoc,markPos,jsonIndex);
        }
    }
}

#if 0
QPointF MainWindow::markPosFromJson(QJsonObject *Json, int row, int col)
{
    QPointF retValue;
    QJsonArray jsonArray;

    QString keyR, keyC;
    keyR = QString("row")+QString("%1").arg(row);
    keyC = QString("col")+QString("%1").arg(col);

    jsonArray = Json->value(keyR).toObject().value(keyC).toArray();
    retValue.setX(jsonArray.at(0).toDouble());
    retValue.setY(jsonArray.at(1).toDouble());

    return retValue;
}
#else
QPointF MainWindow::markPosFromJson(QJsonArray *JsonArray, int order)
{
    QPointF retValue;
    QJsonArray jsonItem;

    jsonItem = JsonArray->at(order).toObject().value("pos").toArray();
    retValue.setX(jsonItem.at(0).toDouble());
    retValue.setY(jsonItem.at(1).toDouble());

    return retValue;
}
#endif

QPoint MainWindow::markLocFromJson(QJsonArray *JsonArray, int order)
{
    QPoint retValue;
    int jsonIndex;

    jsonIndex = JsonArray->at(order).toObject().value("order").toInt();
    retValue.setX(jsonIndex%colAmount);
    retValue.setY(jsonIndex/colAmount);

    return retValue;
}

void MainWindow::setMarkItemInfo(QPoint location, QPointF position, int jsonIndex)
{
    emit sendMarkC(location.x()+1);
    emit sendMarkR(location.y()+1);
    emit sendMarkX(position.x());
    emit sendMarkY(position.y());

    ui->jsonFileIndex->setValue(jsonIndex);
}

//
//Edit mark Items
//
void MainWindow::markRChange(int value)
{
    //NO select Mark Item
    if(graphicsView->markItem() == 0)
        return;

    if(value-1 != graphicsView->markItem()->itemLocation().y()){
        QPoint location;
        location.setX(graphicsView->markItem()->itemLocation().x());
        location.setY(value-1);

        graphicsView->markItem()->setItemLocation(location);
    }
}
void MainWindow::markCChange(int value)
{
    //NO select Mark Item
    if(graphicsView->markItem() == 0)
        return;

    if(value-1 != graphicsView->markItem()->itemLocation().x()){
        QPoint location;
        location.setX(value-1);
        location.setY(graphicsView->markItem()->itemLocation().y());

        graphicsView->markItem()->setItemLocation(location);
    }
}

void MainWindow::markXChange(double value)
{
    QPointF pos;

    //NO select Mark Item
    if(graphicsView->markItem() == 0)
        return;

    if(EPSINON <= abs(value - graphicsView->markItem()->itemRawPos().x())){
        pos.setX(value);
        pos.setY(ui->markPositionY->value());
        emit sendMarkPosition(pos);
    }
}
void MainWindow::markYChange(double value)
{
    QPointF pos;

    //NO select Mark Item
    if(graphicsView->markItem() == 0)
        return;

    if(EPSINON <= abs(value - graphicsView->markItem()->itemRawPos().y())){
        pos.setX(ui->markPositionX->value());
        pos.setY(value);

        emit sendMarkPosition(pos);
    }
}

void MainWindow::addNewItem(QPoint location, QPointF position, int jsonIndex)
{
    graphics_1 = new DlightGraphicsEllipseItem();
    graphics_1->setItemRawPos(position);
    graphics_1->setItemLocation(location);
    graphics_1->setItemJsonIndex(jsonIndex);

    picScene->addItem(graphics_1);
    graphics_1->setPen(QPen(Qt::blue));
    graphics_1->setBrush(QBrush(Qt::blue));
    graphics_1->setStartAngle(360*10);
    graphics_1->setSpanAngle(360*4);
    graphics_1->setRect(graphics_1->itemRawPos().x()-10,
                        graphics_1->itemRawPos().y()-10,
                        20,
                        20);
    graphics_1->setCursor(Qt::CrossCursor);

    graphics_marks.append(graphics_1);
    itemCountIncrease(1);
}

void MainWindow::addPointItem()
{
    QPoint centerPos;

    centerPos = graphicsView->mapToGlobal(QPoint(graphicsView->size().width()/2,
                                         graphicsView->size().height()/2));

    addNewItem(QPoint(0,0), QPointF(0,0), ui->jsonFileIndex->value());
    graphicsView->setMarkItem(graphics_marks.last());
    graphicsView->followCursor();

    QCursor::setPos(centerPos);
}

void MainWindow::deletePointItem()
{
    //NO select Mark Item
    if(graphicsView->markItem() == 0){
        messageInfo("请选择标点！");
        return;
    }

    picScene->removeItem(graphicsView->markItem());
    graphics_marks.removeOne(graphicsView->markItem());

    graphicsView->setMarkItem(0);
    delete graphicsView->markItem();

    itemCountIncrease(-1);
}

//
void MainWindow::setJsonIndex(int index)
{
    //NO select Mark Item
    if(graphicsView->markItem() == 0)
        return;

    graphicsView->markItem()->setItemJsonIndex(index);

    if(ui->jsonFileIndex->value() != index)
        ui->jsonFileIndex->setValue(index);
}

void MainWindow::itemCountIncrease(int value)
{
    jsonItemCount += value;
    ui->itemCount->setValue(jsonItemCount);
}
