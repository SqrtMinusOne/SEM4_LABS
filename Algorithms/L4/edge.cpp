#include "edge.h"
#include "node.h"

#include <qmath.h>
#include <QPainter>
#include <QDebug>

//! [0]
Edge::Edge(Node *sourceNode, Node *destNode, List* lst)
    : arrowSize(10)
{
    list = lst;
    graph = sourceNode->graph;
    setAcceptedMouseButtons(0);
    source = sourceNode;
    dest = destNode;
    source->addEdge(this);
    dest->addEdge(this);
    adjust();
}

void Edge::clear()
{
    sourceNode()->edgeList.removeOne(this);
    destNode()->edgeList.removeOne(this);
}
//! [0]

//! [1]
Node *Edge::sourceNode() const
{
    return source;
}

Node *Edge::destNode() const
{
    return dest;
}
//! [1]

//! [2]
void Edge::adjust()
{
    if (!source || !dest)
        return;

    QLineF line(mapFromItem(source, 0, 0), mapFromItem(dest, 0, 0));
    qreal length = line.length();

    prepareGeometryChange();

    if (length > qreal(20.)) {
        QPointF edgeOffset((line.dx() * 15) / length, (line.dy() * 15) / length);
        sourcePoint = line.p1() + edgeOffset;
        destPoint = line.p2() - edgeOffset;
    } else {
        sourcePoint = destPoint = line.p1();
    }
}

void Edge::AddAnimation(int pos)
{
    QColor col = Qt::black;
    if (list->mark!=0){
        double hue = (list->mark - 1);
        hue = hue*360;
        hue = hue/(graph->gr->max_iter);
        col.setHsv(hue, 255, 255);
    }
    QGraphicsEllipseItem* a = this->scene()->addEllipse(0, 0, 10, 10);
    a->setParentItem(this);
    a->setPos(sourcePoint);
    a->setPen(QPen(col, 0));
    a->setBrush(QBrush(col));
    a->update();
    MovingElliplse str;
    str.Ellipse = a; str.position = pos;
    Anim.push_back(str);
}
//! [2]

//! [3]
QRectF Edge::boundingRect() const
{
    if (!source || !dest)
        return QRectF();

    qreal penWidth = 1;
    if (list->node->node->graph->gr->weights)
        penWidth = (log10(abs(this->list->weight)) + 1)*32;
    qreal extra = (penWidth + arrowSize) / 2.0;

    return QRectF(sourcePoint, QSizeF(destPoint.x() - sourcePoint.x(),
                                      destPoint.y() - sourcePoint.y()))
        .normalized()
        .adjusted(-extra, -extra, extra, extra);
}
//! [3]

//! [4]
void Edge::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if (!source || !dest)
        return;

    QLineF line(sourcePoint, destPoint);
    if (qFuzzyCompare(line.length(), qreal(0.)))
        return;
//! [4]

//! [5]
    // Draw the line itself
    QColor col;
    int w = 1;
    if (list->mark!=0){
        double hue = (list->mark - 1);
        hue = hue*360;
        hue = hue/(graph->gr->max_iter);
        col.setHsv(hue, 255, 255);
        w = 2;
    }
    else
        col = Qt::black;
    painter->setPen(QPen(col, w, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->drawLine(line);
    weight_temp = 1;
    if (graph->gr->weights){
        //Draw text
        QString temp = QString::number(list->flow) + "/" + QString::number(list->weight - list->flow);
        weight_temp = list->weight - list->flow;
        if (weight_temp == 0)
            weight_temp++;
        QFont font = painter->font();
        font.setPointSize(14);
        painter->setFont(font);
        painter->setPen(QPen(Qt::black, 0));
        QPointF centerPoint = (sourcePoint + destPoint)/2;
        painter->drawText(centerPoint, temp);
    }
    double angle = std::atan2(-line.dy(), line.dx());

    QPointF destArrowP1 = destPoint + QPointF(sin(angle - M_PI / 3) * arrowSize,
                                              cos(angle - M_PI / 3) * arrowSize);
    QPointF destArrowP2 = destPoint + QPointF(sin(angle - M_PI + M_PI / 3) * arrowSize,
                                              cos(angle - M_PI + M_PI / 3) * arrowSize);
    painter->setBrush(col);
    painter->drawPolygon(QPolygonF() << line.p2() << destArrowP1 << destArrowP2);
   // ProcessAnimation();
}

void Edge::ProcessAnimation()
{
    for (int e = 0; e < Anim.size(); e++){
        if (Anim[e].position == 100)
            Anim[e].position = 0;
        Anim[e].position++;
        qreal coef = 1 - (qreal)(Anim[e].position)/100;
        int xs = sourcePoint.x() - 5;
        int ys = sourcePoint.y() - 5;
        int xd = destPoint.x() - 5;
        int yd = destPoint.y() - 5;
        qreal newx = xs*coef + xd * (1-coef);
        qreal newy = ys*coef + yd * (1-coef);
        Anim[e].Ellipse->setX(newx);
        Anim[e].Ellipse->setY(newy);
        Anim[e].Ellipse->update();
    }
}

void Edge::RemoveAnimation()
{
    for (int e = 0; e < Anim.size(); e++){
        delete Anim[e].Ellipse;
    }
    Anim.clear();
}

//! [6]