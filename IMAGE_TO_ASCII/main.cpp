#include <QApplication>
#include <QMainWindow>
#include <QTimer>
#include <QElapsedTimer>
#include <QPoint>
#include <QPainter>
#include <QObject>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QShortcut>
#include <QKeyEvent>
#include <QFileDialog>
#include <cmath>


class Window : public QMainWindow
{
    Q_OBJECT
public:
    Window(int w, int h) : WIDTH(w), HEIGHT(h)
    {
        resize(w, h);
        m_pathToImg = "://Lenna64x64.png";
        setWindowTitle("ImageToASCII [" + m_pathToImg + "]");

        m_openAction = new QAction("Open image...", this);
        saveAction = new QAction("Save image...", this);

        menuBar()->setVisible(true);
        m_menu = this->menuBar()->addMenu("File");
        m_menu->addAction(m_openAction);
        m_menu->addAction(saveAction);
        connect(m_openAction, &QAction::triggered, [this](){
            QString newImagePath = QFileDialog::getOpenFileName(this,"Open image... less than 100px", QDir::currentPath(), "Image files (*.jpg *.jpeg *.png *.bmp *.gif)");
            if(!newImagePath.isEmpty())
            {
                m_pathToImg = newImagePath;
                update();
            }
        });

        connect(saveAction, &QAction::triggered, [this](){
            QString newImagePath = QFileDialog::getSaveFileName(this,"Save image...", QDir::currentPath(), "Image files (*.jpg *.jpeg *.png *.bmp *.gif)");
            if(!newImagePath.isEmpty())
            {
                QImage bitmap(this->size(), QImage::Format_ARGB32);
                bitmap.fill(Qt::transparent);
                QPainter painter(&bitmap);
                this->render(&painter, QPoint(), QRegion(), QWidget::DrawChildren);
                bitmap.save(newImagePath);
            }
        });

        update();
    }
    virtual ~Window(){}
private:
    void paintEvent(QPaintEvent *event)
    {
        Q_UNUSED(event);
        QPainter qp(this);
        qp.setBrush(QBrush(Qt::black));
        qp.drawRect(0,0,size().width(), size().height());
        drawASCI(qp);
    }
    void drawASCI(QPainter& p){
        p.save();
        QFont textFont("Arial");
        QImage image(m_pathToImg);

        int w = WIDTH/image.width();
        int h = HEIGHT/image.height();
        for(int x = 0; x < image.width(); ++x)
        {
            for(int y = 0; y < image.height(); ++y)
            {
                int grayscale = qGray(image.pixelColor(x, y).rgb());
                QColor textColor(grayscale, grayscale, grayscale);
                textFont.setPixelSize(w*1.2f);
                p.setFont(textFont);
                p.setBrush(QBrush(textColor));
                p.setPen(QPen(textColor));

                int avg = grayscale;
                int len = m_density.length();

                int charIndex = static_cast<int>(floor( avg  * len / 255));
                p.drawText(x * w + w * 0.5, y * h + h * 0.5,w,h,Qt::AlignCenter , m_density[charIndex] );
            }
        }
        p.restore();
    }

    const int WIDTH;
    const int HEIGHT;
    QString m_pathToImg;
    QString m_density = "N@#W$9876543210?!abc;:+=-,._ ";
    QMenu* m_menu;
    QAction* m_openAction, *saveAction;
    // QWidget interface
protected:
    void keyReleaseEvent(QKeyEvent *event){
        if(event->key() == Qt::Key_O)
        {
            QString newImagePath = QFileDialog::getOpenFileName(this,"Open image... less than 100px", QDir::currentPath(), "Image files (*.jpg *.jpeg *.png *.bmp *.gif)");
            if(!newImagePath.isEmpty())
            {
                m_pathToImg = newImagePath;
                update();
            }
        }
        else if(event->key() == Qt::Key_1)
        {
            m_density = "N@#W$9876543210?!abc;:+=-,._ ";
            update();
        }
        else if(event->key() == Qt::Key_2)
        {
            m_density = "       .:-i|=+%O#@";
            update();
        }
        QMainWindow::keyReleaseEvent(event);
    }
};

#include "main.moc"

int main(int argc, char** argv)
{
    QApplication a(argc, argv);
    Window *w = new Window(512, 512);
    w->show();
    a.exec();
}
