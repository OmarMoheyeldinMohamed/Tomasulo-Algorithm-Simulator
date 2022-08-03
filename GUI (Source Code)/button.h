#ifndef BUTTON_H
#define BUTTON_H

#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QObject>
class Button : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
public:
    Button(QString t,int width=300, int height=100, QGraphicsItem* parent = NULL);
    void mousePressEvent (QGraphicsSceneMouseEvent * event);
    void hoverEnterEvent (QGraphicsSceneHoverEvent * event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);
signals:
    void clicked();

private:
    QGraphicsTextItem *text;
};


#endif // BUTTON_H
