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
    uint16_t width;
    uint16_t height;
    uint16_t bitCount;
    uint16_t reserved1;
    uint32_t reserved2;
    uint32_t reserved3;
} BGITextureHDR;


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    if(argc<3)
    {
            cout<<"Usage:"<<argv[0]<<"mode Picture File"<<endl;
            cout<<"p ->pic2bgiTexture"<<endl;
            cout<<"e ->bgiTexture2png"<<endl;
            return -1;
    }
    if(strcmp(argv[1],"p")==0)
    {
        QImage img;
        auto res=img.load(argv[2]);
        if(!res)
        {
            cout<<"LOAD PIC FAIL!"<<endl;
            return -1;
        }
        BGITextureHDR BHDR;
        memset(&BHDR,0,sizeof(BGITextureHDR));
        BHDR.width=img.width();
        BHDR.height=img.height();
        BHDR.bitCount=32;
        char* payloadbuf=new char[BHDR.width*BHDR.height*32/8];
        for (int y=0;y<BHDR.height;y++)
        {
            for (int x=0;x<BHDR.width;x++)
            {
                QRgb col=img.pixel(x,y);
                payloadbuf[y*BHDR.width*32/8+x*32/8+0]=qBlue(col);
                payloadbuf[y*BHDR.width*32/8+x*32/8+1]=qGreen(col);
                payloadbuf[y*BHDR.width*32/8+x*32/8+2]=qRed(col);
                payloadbuf[y*BHDR.width*32/8+x*32/8+3]=qAlpha(col);
            }
        }
        QString FileName=argv[2];
        FileName+=".out";
        QFile fout(FileName);
        fout.open(QIODevice::WriteOnly);
        char* filebuf=new char[sizeof (BGITextureHDR)+BHDR.width*BHDR.height*32/8];
        memcpy(filebuf,&BHDR,sizeof(BGITextureHDR));
        memcpy(filebuf+sizeof(BGITextureHDR),payloadbuf,BHDR.width*BHDR.height*32/8);
        fout.write(filebuf,sizeof (BGITextureHDR)+BHDR.width*BHDR.height*32/8);
        fout.close();
        delete [] filebuf;
        delete [] payloadbuf;
    }

    if(strcmp(argv[1],"e")==0)
    {
        QFile fread(argv[2]);
        char* filebuf=new char[fread.size()];
        fread.open(QIODevice::ReadOnly);
        fread.read(filebuf,fread.size());
		fread.close();
        BGITextureHDR BHDR;
        memset(&BHDR,0,sizeof(BGITextureHDR));
        memcpy(&BHDR,filebuf,sizeof(BGITextureHDR));
        auto H_bitmap=QImage(BHDR.width,BHDR.height,QImage::Format_RGBA8888);
		if (BHDR.bitCount / 8 == 4)
		{
			for (int y = 0; y < BHDR.height; y++)
			{
				for (int x = 0; x < BHDR.width; x++)
				{
					auto b = filebuf[sizeof(BGITextureHDR) + y * BHDR.width * 32 / 8 + x * 32 / 8 + 0];
					auto g = filebuf[sizeof(BGITextureHDR) + y * BHDR.width * 32 / 8 + x * 32 / 8 + 1];
					auto r = filebuf[sizeof(BGITextureHDR) + y * BHDR.width * 32 / 8 + x * 32 / 8 + 2];
					auto a = filebuf[sizeof(BGITextureHDR) + y * BHDR.width * 32 / 8 + x * 32 / 8 + 3];
					H_bitmap.setPixel(x, y, qRgba(r, g, b, a));
				}
			}
		}
		else if(BHDR.bitCount / 8 == 3)
		{
			for (int y = 0; y < BHDR.height; y++)
			{
				for (int x = 0; x < BHDR.width; x++)
				{
					auto b = filebuf[sizeof(BGITextureHDR) + y * BHDR.width * 24 / 8 + x * 24 / 8 + 0];
					auto g = filebuf[sizeof(BGITextureHDR) + y * BHDR.width * 24 / 8 + x * 24 / 8 + 1];
					auto r = filebuf[sizeof(BGITextureHDR) + y * BHDR.width * 24 / 8 + x * 24 / 8 + 2];
					H_bitmap.setPixel(x, y, qRgb(r, g, b));
				}
			}
		}
		else
		{
			cout << "Unexpected Texture, Need RGBA or RGB Texture" << endl;
		}
        
        QString FileName=argv[2];
        FileName+=".png";
        H_bitmap.save(FileName,"png");
        delete [] filebuf;
    }

    cout<<"Finish"<<endl;
    //return a.exec();
    return 0;
}
