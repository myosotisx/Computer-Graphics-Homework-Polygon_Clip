#include "clipwidget.h"
#include "ui_clipwidget.h"

#include <QDebug>
#include <QMouseEvent>
#include <QPen>
#include <QPainter>

ClipWidget::ClipWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClipWidget),
    maxPolygons(2),
    isOut(true),
    clipped(false),
    resultOnly(false)
{
    ui->setupUi(this);
    color[0] = Qt::white;
    color[1] = Qt::darkGray;
    color[2] = Qt::yellow;
    color[3] = Qt::red;
    color[4] = Qt::green;

    setFocusPolicy(Qt::StrongFocus);
}

ClipWidget::~ClipWidget() {
    delete ui;
}

void ClipWidget::mousePressEvent(QMouseEvent *e) {
    if(polygons.size() >= maxPolygons) return;
    if(e->button() == Qt::LeftButton) {
        if(isOut) {
            outPoints.push_back(std::pair<double, double>(e->x(), e->y()));
        }
        else {
            inPoints[inPoints.size()-1].push_back(std::pair<double, double>(e->x(), e->y()));
        }
    }
    else if(e->button() == Qt::RightButton) {
        if(outPoints.size() < 3) {
            emit warning("Number of edges is less than 3!");
            return;
        }
        int n = inPoints.size();
        for(int i=0; i<n-1; i++) {
            if(inPoints[i].size() < 3) {
                emit warning("Number of edges is less than 3!");
                return;
            }
        }
        if(n > 0 && inPoints[n-1].size() > 0 && inPoints[n-1].size() < 3) {
            emit warning("Number of edges is less than 3!");
            return;
        }
        Polygon *polygon = new Polygon(outPoints);
        for(int i=0; i<n; i++) {
            if(inPoints[i].size() > 0)polygon->rings.push_back(Polygon::genNodeList(inPoints[i]));
        }
        polygons.push_back(polygon);
        outPoints.clear();
        inPoints.clear();
        isOut = true;
        emit inputOut();
        if(polygons.size() == 1) emit status("Please input window polygon.");
    }
    update();
}

void ClipWidget::keyPressEvent(QKeyEvent *e) {
    if(polygons.size() >= maxPolygons) return;
    if(e->key() == Qt::Key_Space && !isOut && inPoints.size() > 0) {
        int n = inPoints[inPoints.size()-1].size();
        if(n >= 3) {
            emit status("Shift to next inner ring.");
            inPoints.push_back(std::vector<std::pair<double, double>>());
        }
        else if(n > 0) {
            emit warning("Number of edges is less than 3!");
        }
    }
    if(e->key() == Qt::Key_Shift) {
        emit shiftInOut(!isOut);
    }
}

void ClipWidget::paintEvent(QPaintEvent *e) {
    Q_UNUSED(e);
    QPen pen;
    QPainter painter(this);
    pen.setWidth(2);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setBrush(Qt::black);
    painter.drawRect(rect());
    painter.setBrush(Qt::NoBrush);

    if(!resultOnly) {
        // draw polygons
        if(polygons.size() >= 1) {
            pen.setColor(color[0]);
            painter.setPen(pen);
            Polygon *polygon = polygons[0];
            int n = polygon->rings.size();
            for(int i=0; i<n; i++) {
                Node *head = polygon->rings[i];
                Node *p = head;
                while(p->next) {
                    painter.drawLine(p->x, p->y, p->next->x, p->next->y);
                    p = p->next;
                }
                painter.drawLine(p->x, p->y, head->x, head->y);
            }
        }
        if(polygons.size() >= 2) {
            pen.setColor(color[1]);
            painter.setPen(pen);
            Polygon *polygon = polygons[1];
            int n = polygon->rings.size();
            for(int i=0; i<n; i++) {
                Node *head = polygon->rings[i];
                Node *p = head;
                while(p->next) {
                    painter.drawLine(p->x, p->y, p->next->x, p->next->y);
                    p = p->next;
                }
                painter.drawLine(p->x, p->y, head->x, head->y);
            }
        }

        // draw points
        pen.setColor(color[2]);
        painter.setPen(pen);
        int n = outPoints.size();
        for(int i=0; i<n; i++) {
            painter.drawPoint(outPoints[i].first, outPoints[i].second);
        }
        pen.setColor(color[3]);
        painter.setPen(pen);
        n = inPoints.size();
        for(int i=0; i<n; i++) {
            int m = inPoints[i].size();
            for(int j=0; j<m; j++) {
                painter.drawPoint(inPoints[i][j].first, inPoints[i][j].second);
            }
        }
    }

    // draw clip results
    pen.setColor(color[4]);
    pen.setWidth(3);
    painter.setPen(pen);
    int n = res.size();
    for(int i=0; i<n; i++) {
        Polygon *polygon = res[i];
        int m = polygon->rings.size();
        for(int j=0; j<m; j++) {
            Node *head = polygon->rings[j];
            Node *p = head;
            while(p->next) {
                painter.drawLine(p->x, p->y, p->next->x, p->next->y);
                p = p->next;
            }
            painter.drawLine(p->x, p->y, head->x, head->y);
        }
    }
}

void ClipWidget::clip() {
    if(clipped) return;
    if(polygons.size() == 0) {
        emit warning("Please input main polygon!");
        return;
    }
    if(polygons.size() == 1) {
        emit warning("Please input window polygon!");
        return;
    }

    res = clipPolygons(polygons[0], polygons[1]);
    clipped = true;
    update();
}

void ClipWidget::finish() {
    if(!clipped) {
        emit warning("You have not clipped!");
        return;
    }
    if(res.size() == 1) {
        outPoints.clear();
        inPoints.clear();
        polygons.clear();
        polygons.push_back(res[0]);
        res.clear();
        isOut = true;
        emit inputOut();
        emit status("Please input window polygon.");
        update();
    }
    else clear();
    clipped = false;
}

void ClipWidget::clear() {
    outPoints.clear();
    inPoints.clear();
    polygons.clear();
    res.clear();
    isOut = true;
    clipped = false;
    emit inputOut();
    update();
}

void ClipWidget::showResultsMode(int state) {
    if(state == Qt::Checked) {
        resultOnly = true;
        update();
    }
    if(state == Qt::Unchecked) {
        resultOnly = false;
        update();
    }
}

void ClipWidget::switchInOut(bool isOut) {
    this->isOut = isOut;
    if(!isOut && inPoints.size() == 0) {
        inPoints.push_back(std::vector<std::pair<double, double>>());
    }
}
