#include "datatracer.h"
#include <QFontMetrics>
#include <QRect>
#include <QDebug>

DataTracer::DataTracer(QCustomPlot *plot):
    plot(plot),
visible(false)
{
    if(plot){
        this->tracer = new QCPItemTracer(plot);
        this->tracer->setStyle(QCPItemTracer::tsCircle);
        this->tracer->setPen(QPen(Qt::yellow));
        this->tracer->setBrush(QPen(Qt::yellow).color());
        this->tracer->setSize(10);
        //tracer按照坐标值设置位置
        this->tracer->position->setType(QCPItemPosition::ptPlotCoords);

        this->label = new QCPItemText(plot);
        this->label->setLayer("overlay");
        this-> label->setClipToAxisRect(false);
        this->label->setPadding(QMargins(2,2,2,2));
        //设置label的父锚点
        this->label->position->setParentAnchor(tracer->position);
        //设置父锚点后，该函数设置的数值类型会变为ptAbsolute
        //按像素点的方式设置label与父锚点的相对位置
        this->label->position->setCoords(0,0);
        //设置label与跟踪点的相对位置
        this->label->setPositionAlignment(Qt::AlignLeft|Qt::AlignVCenter);
        this->label->setFont(this->plot->xAxis->labelFont());
        this->label->setBrush(QBrush(QColor(255, 255, 0, 60)));
        this->label->setPen(QPen(Qt::yellow,1));
        this->label->setColor(this->plot->xAxis->basePen().color());
        //设置label文字的对齐方式
        this->label->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);

        this->arrow = new QCPItemLine(plot);
        this->arrow->setLayer("overlay");
        this->arrow->setPen(QPen(Qt::yellow,2));
        this->arrow->setClipToAxisRect(false);
        //设置arrow起始端点样式
        this->arrow->setHead(QCPLineEnding::esSpikeArrow);
        //设置arrow终点位置为tracer点位置
        this->arrow->end->setParentAnchor(tracer->position);
        //设置arrow起始点位置为label的下端点（初始状态）
        this->arrow->start->setParentAnchor(label->bottom);
        //设置arrow的起始的相对位置（初始状态）
        this->arrow->start->setCoords(0, 0);
        //设置初始不显示
        this->setVisible(visible);
    }
}

DataTracer::~DataTracer()
{
    if(tracer){
        plot->removeItem(tracer);
    }
    if(label){
        plot->removeItem(label);
    }
    if(arrow){
        plot->removeItem(arrow);
    }
}

void DataTracer::setPen(const QPen &pen)
{
    this->tracer->setPen(pen);
    this->arrow->setPen(pen);
}

void DataTracer::setBrush(const QBrush &brush)
{
    this->tracer->setBrush(brush);
}

void DataTracer::setText(const QString &x_text, const QString &y_text)
{
    this->label_key = x_text;
    this->label_value = y_text;
    this->label->setText(this->label_value+'\n'+ this->label_key);
}

void DataTracer::setLabelStyle(const QFont &font, const QPen &pen, const QColor &color)
{
    this->label->setFont(font);
    this->label->setPen(pen);
    this->label->setColor(color);
}

void DataTracer::updatePosition(QCPGraph *graph, double xValue, double yValue)
{
    this->graph = graph;
    this->key = xValue;
    this->value = yValue;
    //设置tracer的位置
    this->tracer->position->setCoords(xValue, yValue);
    //坐标(xValue,yValue)对应像素点的位置
    QPointF pf = graph->coordsToPixels(xValue, yValue);
    //当前坐标区域对应像素点区域
    QRectF axis_rect(this->plot->axisRect(0)->topLeft(), this->plot->axisRect(0)->bottomRight());

    //获取显示字符对应的像素区域大小
    QFontMetrics fm(this->label->font());
    QRect r1=fm.boundingRect(this->label_key);
    QRect r2=fm.boundingRect(this->label_value);

    //依据父锚点位置，计算相对父锚点位置为（0,0）时label对应的绝对位置的像素区域
    QRectF label_rect;
    if(r1.width() >= r2.width()){
        label_rect.setTopLeft(QPointF(pf.x(), pf.y()-0.5*r1.height()-2));
        label_rect.setSize(QSizeF(r1.width(), 2*r1.height()+4));
    }else{
        label_rect.setTopLeft(QPointF(pf.x(), pf.y()-0.5*r2.height()-2));
        label_rect.setSize(QSizeF(r1.width(), 2*r2.height()+4));
    }

    //计算label显示的像素区域位置
    QRectF show_rect = label_rect;
    //假如label显示在tracer的正上方
    show_rect.translate(-0.5*label_rect.width()-2, -1.0*label_rect.height()-2);
    if(axis_rect.contains(show_rect)){
        //up
        this->arrow->start->setParentAnchor(label->bottom);
        this->label->position->setCoords(-0.5*label_rect.width()-2, -1.0*label_rect.height()-2);
        //        qDebug()<<"up";
    }else{
        show_rect = label_rect;
        //假如label显示在tracer的正下方
        show_rect.translate(-0.5*label_rect.width()-2, 1.0*label_rect.height()+2);
        if(axis_rect.contains(show_rect)){
            //bot
            this->arrow->start->setParentAnchor(label->top);
            this->label->position->setCoords(-0.5*label_rect.width()-2, 1.0*label_rect.height()+2);
            //            qDebug()<<"bot";
        }else{
            show_rect = label_rect;
            //假如label显示在tracer的右侧
            show_rect.translate(30, 0);
            if(axis_rect.contains(show_rect)){
                //right
                this->arrow->start->setParentAnchor(label->left);
                this->label->position->setCoords(30, 0);
                //                qDebug()<<"right";
            }else{
                show_rect = label_rect;
                //假如label显示在tracer的左侧
                show_rect.translate(-1.0*label_rect.width()-30, 0);
                if(axis_rect.contains(show_rect)){
                    //left
                    this->arrow->start->setParentAnchor(label->right);
                    this->label->position->setCoords(-1.0*label_rect.width()-30, 0);
                    //                    qDebug()<<"left";
                }
            }
        }
    }
}

bool DataTracer::isVisible()
{
    return visible;
}

void DataTracer::setVisible(bool visible)
{
    this->visible = visible;
    this->tracer->setVisible(visible);
    this->label->setVisible(visible);
    this->arrow->setVisible(visible);
}

void DataTracer::updateShow()
{
    if(!visible)    return;

    QPointF pf = this->graph->coordsToPixels(this->key, this->value);
    QRectF axis_rect(this->plot->axisRect(0)->topLeft(), this->plot->axisRect(0)->bottomRight());

    if(axis_rect.contains(pf)){
        updatePosition(this->graph, this->key, this->value);
    }else{
        this->setVisible(false);
    }
}
