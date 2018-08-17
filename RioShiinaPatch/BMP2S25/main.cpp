#include <QApplication>
#include <QFile>
#include <QString>
#include <iostream>
#include <QFile>
#include <QPixmap>
#include <QTextStream>
#include <QList>
#include <QPainter>
using namespace std;
typedef struct {
    char magic[4];		// "S25"
    signed long index_entries;
} S25_header_t;

typedef struct {
    signed long width;
    signed long height;
    signed long orig_x;
    signed long orig_y;
    signed long reserved;		// 0
} S25_info_t;

int main(int argc, char *argv[])
{
    QGuiApplication a(argc, argv);
    if(argc<2)
    {
        cout<<"Usage:"<<argv[0]<<" BMP_File"<<endl;
        return -1;
    }
    //load image
    QImage img;
    auto res=img.load(argv[1]);
    if(!res)cout<<"Load Fail"<<endl;
    //build payload
    auto img_width=img.width();
    auto img_height=img.height();
    auto linebufsize=2+img_width*6;
    unsigned long* rowtablebuf=new unsigned long[img_height];
    char* payloadbuf=new char[linebufsize*img_height];
    //fake compress method
    for(int y=0;y<img_height;y++)
    {
        *(unsigned short*)(payloadbuf+y*linebufsize)=linebufsize-2;
        rowtablebuf[y]=y*linebufsize;
        for(int x=0;x<img_width;x++)
        {
            *(unsigned short*)(payloadbuf+y*linebufsize+2+x*6)=0xA001;
            QRgb col=img.pixel(x,y);
            char a=qAlpha(col);
            char b=qBlue(col);
            char r=qRed(col);
            char g=qGreen(col);
            *(payloadbuf+y*linebufsize+2+x*6+2)=a;
            *(payloadbuf+y*linebufsize+2+x*6+3)=b;
            *(payloadbuf+y*linebufsize+2+x*6+4)=g;
            *(payloadbuf+y*linebufsize+2+x*6+5)=r;
        }
    }
    char* filebuf=new char[sizeof (long)*img_height+sizeof(char)*linebufsize*img_height+sizeof (S25_header_t)+sizeof(long)+sizeof(S25_info_t)];
    S25_header_t head;
    head.index_entries=1;
    char magic[4]={0x53,0x32,0x35,0x00};
    memcpy(head.magic,magic,sizeof(magic));
    memcpy(filebuf,&head,sizeof(head));
    *(long*)(filebuf+sizeof(S25_header_t))=sizeof(head)+sizeof(long);
    S25_info_t info;
    info.height=img_height;
    info.width=img_width;
    info.orig_x=0;
    info.orig_y=0;
    info.reserved=0;
    memcpy(filebuf+sizeof (S25_header_t)+sizeof(long),&info,sizeof(info));
    for(int i=0;i<img_height;i++)
    {
        rowtablebuf[i]+=(sizeof (S25_header_t)+sizeof(long)+sizeof(info)+sizeof (long)*img_height);
    }
    memcpy(filebuf+sizeof (S25_header_t)+sizeof(long)+sizeof(info),rowtablebuf,sizeof (long)*img_height);
    memcpy(filebuf+sizeof (S25_header_t)+sizeof(long)+sizeof(info)+sizeof (long)*img_height,payloadbuf,sizeof(char)*linebufsize*img_height);
    QFile fout("rebuild.s25");
    fout.open(QIODevice::WriteOnly);
    fout.write(filebuf,sizeof (long)*img_height+sizeof(char)*linebufsize*img_height+sizeof (S25_header_t)+sizeof(long)+sizeof(S25_info_t));
    fout.close();
    cout<<"finish"<<endl;
    return a.exec();
}
