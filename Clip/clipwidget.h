#ifndef CLIPWIDGET_H
#define CLIPWIDGET_H

#include "graph.h"

#include <QWidget>
#include <QDebug>

#include <vector>


namespace Ui {
    class ClipWidget;
}

class ClipWidget : public QWidget {
    Q_OBJECT

public:
    explicit ClipWidget(QWidget *parent = nullptr);
    ~ClipWidget();

signals:
    void warning(QString msg);
    void status(QString msg);
    void inputOut();
    void shiftInOut(bool isOut);

public slots:
    void clip();
    void finish();
    void clear();
    void showResultsMode(int state);
    void switchInOut(bool isOut);

protected:
    void mousePressEvent(QMouseEvent *e) override;
    void keyPressEvent(QKeyEvent *e) override;
    void paintEvent(QPaintEvent *e) override;

private:
    Ui::ClipWidget *ui;

    std::vector<std::pair<double, double>> outPoints;
    std::vector<std::vector<std::pair<double, double>>> inPoints;
    std::vector<Polygon*> polygons;
    std::vector<Polygon*> res;

    unsigned int maxPolygons;
    Qt::GlobalColor color[10];
    bool isOut;
    bool clipped;
    bool resultOnly;
};

#endif // CLIPWIDGET_H
