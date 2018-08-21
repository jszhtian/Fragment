#include <QApplication>
#include <QFont>
#include <QFile>
#include <QString>
#include <iostream>
#include <QFile>
#include <QPixmap>
#include <QTextStream>
#include <QList>
#include <QPainter>

#define nHeigh 30
#define nWeight 30
using namespace std;

int main(int argc, char *argv[])
{
    QGuiApplication a(argc, argv);
    if(argc<2)
    {
        cout<<"Usage:"<<argv[0]<<" TBL_file"<<endl;
        return -1;
    }
    QFile qf_TBL(argv[1]);

    if(!qf_TBL.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        cout<<"File can not open!"<<endl;
        return -1;
    }
    QTextStream in_str(&qf_TBL);
    in_str.setCodec("UTF-16LE");
    QList<QString> ProcChrList;
    while(!in_str.atEnd())
    {
        auto fetch_str=in_str.readLine();
        auto spiltpos=fetch_str.indexOf("=")+1;
        auto PureText=fetch_str.mid(spiltpos);
        ProcChrList.append(PureText);
    }
    cout<<"TBL read!"<<endl<<"Writing the Font file"<<endl;
    QByteArray outputBuf;
    for(auto i=0;i<ProcChrList.size();i++)
    {
        auto pix=new QPixmap(nHeigh,nWeight);
        auto p=new QPainter (pix);
        p->fillRect(QRect(QPoint(0,0),QPoint(nHeigh,nWeight)),QBrush(QColor(Qt::black).rgb()));
        p->setFont(QFont("微软雅黑", 18,75));
        p->setRenderHint(QPainter::Antialiasing);
        p->setPen(QColor(Qt::white));
        p->drawText(QRect(QPoint(0,0),QPoint(nHeigh,nWeight)),Qt::AlignCenter,ProcChrList[i]);
        p->end();
        //pix->save("bmp_"+QString::number(i)+".bmp");
        QImage img=pix->toImage();
        delete p;
        delete pix;
        auto grayImg=img.convertToFormat(QImage::Format_Grayscale8);
        //grayImg.save("bmp_"+QString::number(i)+".bmp");
        for(int y=0;y<nHeigh;y++)
        {
            for(int x=0;x<nHeigh;x+=2)
            {
                auto PixCol1=qGray(grayImg.pixel(x,y));
                auto PixCol2=qGray(grayImg.pixel(x+1,y));
                PixCol1=PixCol1*0xF/0xFF;
                PixCol2=(PixCol2*0xF/0xFF)<<4;
                char val=(char)PixCol2|PixCol1;
                outputBuf.append(val);
            }
        }
    }
    QFile outFile("output.fd0");
    if(!outFile.open(QIODevice::WriteOnly))
    {
        cout<<"File can not open!"<<endl;
        return -1;
    }

    outFile.write(outputBuf);
    outFile.close();
    cout<<"Font Make finish!"<<endl;
    return a.exec();
}
