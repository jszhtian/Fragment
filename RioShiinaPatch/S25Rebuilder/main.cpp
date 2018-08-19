#include <QApplication>
#include "s25arc.h"
#include <iostream>
using namespace std;
int main(int argc, char *argv[])
{
    QGuiApplication a(argc, argv);
    if(argc==1)
    {
        cout<<argv[0]<<" r s25Arc s25File index"<<endl;
        cout<<argv[0]<<" d s25Arc index"<<endl;
        cout<<argv[0]<<" t s25Arc"<<endl;
    }
    if(strcmp(argv[1],"r")==0)
    {
        cout<<"REPLACE"<<endl<<"idx:"<<argv[4]<<endl;
        S25Arc s25(argv[2]);
        int idx=QString(argv[4]).toInt();
        s25.replace(idx,argv[3]);
        cout<<"output:"<<"rebuild.s25"<<endl;
        s25.save("rebuild.s25");
    }
    if(strcmp(argv[1],"d")==0)
    {
        cout<<"DUMP"<<endl<<"idx:"<<argv[3]<<endl;
        S25Arc s25(argv[2]);
        int idx=QString(argv[3]).toInt();
        QString Fname=QString(argv[3]);
        Fname+=".s25";
        cout<<"output:"<<Fname.toStdString()<<endl;
        s25.dump(idx,Fname);
    }
    if(strcmp(argv[1],"t")==0)
    {
        cout<<"TEST PACK"<<endl;
        S25Arc s25(argv[2]);
        cout<<"output:"<<"test.s25"<<endl;
        s25.save("test.s25");
    }
    cout<<"Finish!"<<endl;
    QGuiApplication::exit(0);
    return a.exec();
}
