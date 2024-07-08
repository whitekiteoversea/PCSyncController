#ifndef DATATRACER_H
#define DATATRACER_H

#include <QObject>
#include "qcustomplot.h"

    class DataTracer : public QObject
{
    Q_OBJECT
public:
    explicit  DataTracer(QCustomPlot *plot);
    ~DataTracer();

    void setPen(const QPen &pen);
    void setBrush(const QBrush &brush);
    void setText(const QString &x_text,const QString &y_text);
    void setLabelStyle(const QFont &font, const QPen &pen, const QColor&color);
    void updatePosition(QCPGraph *graph, double xValue, double yValue);
    bool isVisible();
    void setVisible(bool visible);
    void updateShow();

private:
    QCustomPlot *plot;
    QCPGraph *graph;
    QCPItemTracer *tracer;
    QCPItemText *label;
    QCPItemLine *arrow;
    bool visible;
    double key;
    double value;
    QString label_key;
    QString label_value;
};


#endif // DATATRACER_H
