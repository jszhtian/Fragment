#include <QCoreApplication>
#include <QFile>
#include <QString>
#include <QHash>
#include <QDataStream>
#include <QByteArray>
#include <Windows.h>
#include <QTextStream>
#include <iostream>
using namespace std;
struct block{
    wchar_t Ori;
    wchar_t Map;
};
wchar_t* ctow(const char *str)
{
    wchar_t* buffer;
    if(str)
    {
        size_t nu = strlen(str);
        size_t n =(size_t)MultiByteToWideChar(932,0,(const char *)str,int(nu),NULL,0);
        buffer=0;
        buffer = new wchar_t[n+1];
        memset(buffer,0,(n+1)*sizeof(wchar_t));
        MultiByteToWideChar(932,0,(const char *)str,int(nu),buffer,int(n));
    }
    return buffer;

}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QFile qf_TBL(argv[1]);
    qf_TBL.open(QIODevice::ReadOnly|QIODevice::Text);
    QTextStream in_str(&qf_TBL);
    in_str.setCodec("UTF-16LE");
    QByteArray out;
    while(!in_str.atEnd())
    {
        auto fetch_str=in_str.readLine();
        auto spiltpos=fetch_str.indexOf("=")+1;
        auto index=fetch_str.mid(0,spiltpos-1).toInt(nullptr,16);
        auto PureText=fetch_str.mid(spiltpos);
        //cout<<hex<<index<<endl;
        //index=(index>>8)|((index<<8)&0xff00);
        unsigned short ucsnum=index;
        ucsnum=(ucsnum>>8)|((ucsnum<<8)&0xff00);
        wchar_t tarchar[5];
        PureText.toWCharArray(tarchar);
        char tmpchar[4];
        memset(tmpchar,0,4);
        memcpy(tmpchar,&ucsnum,2);
        wchar_t* wcharbuf=ctow(tmpchar);
        out.append((char*)wcharbuf,2);
        out.append((char*)&tarchar[0],2);
    }
    QDataStream datastm(&out,QIODevice::WriteOnly);
    QFile outFile("map.dat");
    if(!outFile.open(QIODevice::WriteOnly))
    {
        return -1;
    }
    outFile.write(out);
    outFile.close();
    qf_TBL.close();
    return a.exec();
}
