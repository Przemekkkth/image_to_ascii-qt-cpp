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
    Window(int w, int h) : WIDTH(w), HEIGHT(h), m_startIndex(0)
    {
        resize(w, h);
        m_pathToImg = "://3.png";
        setWindowTitle("ImageToASCII [" + m_pathToImg + "]");

        m_openAction = new QAction("Open image...", this);
        menuBar()->setVisible(true);
        m_menu = this->menuBar()->addMenu("File");
        m_menu->addAction(m_openAction);

        connect(m_openAction, &QAction::triggered, [this](){
            QString newImagePath = QFileDialog::getOpenFileName(this,"Open image... less than 100px", QDir::currentPath(), "Image files (*.jpg *.jpeg *.png *.bmp *.gif)");
            if(!newImagePath.isEmpty())
            {
                m_pathToImg = newImagePath;
                update();
            }
        });
        QFile file(":/poem.txt");
        file.open(QIODevice::ReadOnly);
        QTextStream stream(&file);
        m_contentFile = stream.readAll();
        m_contentData = m_contentFile.split(" ");
        update();

        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, [this](){
           this->update();
        });
        timer->start(250);
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
        int charIndex = m_startIndex;
        QFont textFont("Arial");
        QImage image(m_pathToImg);

        int w = WIDTH/image.width();
        int h = HEIGHT/image.height();
        for(int i = 0; i < image.height(); ++i)
        {
            for(int j = 0; j < image.width(); ++j)
            {
                int grayscale = qGray(image.pixelColor(j, i).rgb());
                QColor textColor(grayscale, grayscale, grayscale);
                textFont.setPixelSize(w*1.2f);
                p.setFont(textFont);
                p.setBrush(QBrush(textColor));
                p.setPen(QPen(textColor));

                p.drawText(j * h + h * 0.5, i * w + w * 0.5 ,w,h,Qt::AlignCenter , m_contentFile.at(charIndex % m_contentFile.length()));
                charIndex++;
            }
        }
        ++m_startIndex;
        p.restore();
    }

    const int WIDTH;
    const int HEIGHT;
    QString m_pathToImg;
    QMenu* m_menu;
    QAction* m_openAction;
    QString m_contentFile;
    QStringList m_contentData;
    int m_startIndex;
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
        QMainWindow::keyReleaseEvent(event);
    }
};

#include "main.moc"

int main(int argc, char** argv)
{
    QApplication a(argc, argv);
    Window *w = new Window(800, 800);
    w->show();
    a.exec();
}
