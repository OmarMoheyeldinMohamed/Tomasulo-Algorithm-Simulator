#include "button.h"
#include <QBrush>
#include <QFont>
Button::Button(QString t,int width, int height, QGraphicsItem *parent):QGraphicsRectItem(parent)
{
    setRect(0, 0, width, height);
    setBrush(QBrush(Qt::darkCyan));
    QFont font("comic sans", 16);
    text = new QGraphicsTextItem(t, this);
    text->setFont(font);
    text->setPos(boundingRect().width()/2-text->boundingRect().width()/2, boundingRect().height()/2-text->boundingRect().height()/2);
    setAcceptHoverEvents(true);

}

void Button::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    emit(clicked());
}

void Button::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    setBrush(QBrush(Qt::cyan));
}

void Button::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    setBrush(QBrush(Qt::darkCyan));
}

