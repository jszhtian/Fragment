#include "s25arc.h"

S25Arc::S25Arc(QString File)
{
    int totread=0;
    QFile Fopen(File);
    Fopen.open(QIODevice::ReadOnly);
    QByteArray buf=Fopen.readAll();
    char* procBuf=(char*)buf.data();
    Fopen.close();
    memcpy(&this->head,procBuf+totread,sizeof(head));
    totread+=sizeof(head);
    entries_offset=new unsigned long[this->head.index_entries];
    memset(entries_offset,0,this->head.index_entries*sizeof(unsigned long));
    memcpy(entries_offset,procBuf+totread,this->head.index_entries*sizeof(unsigned long));
    totread+=this->head.index_entries*sizeof(unsigned long);
    unsigned long targetEntry=sizeof(this->head)+sizeof(unsigned long)*this->head.index_entries;
    int realEntries=0;
    bool firsthit=false;
    QList<int> FileOffsetList;
    for(int i=0;i<this->head.index_entries;i++)
    {
        if(firsthit)
        {
            if(entries_offset[i]==0)continue;
            ListMap.insert(realEntries,i);
            FileOffsetList.append(entries_offset[i]);
            realEntries++;
        }
        else
        {
            if(targetEntry==entries_offset[i])
            {
                ListMap.insert(realEntries,i);
                FileOffsetList.append(entries_offset[i]);
                firsthit=true;
                realEntries++;
            }
        }
    }
    for(auto offset:FileOffsetList)
    {
        unsigned int transptr=offset;
        S25_info_t info;
        memcpy(&info,procBuf+transptr,sizeof(S25_info_t));
        transptr+=sizeof(S25_info_t);
        unsigned long* row_table=new unsigned long[info.height];
        memcpy(row_table,procBuf+transptr,sizeof(unsigned long)*info.height);
        unsigned long compszie=0;
        for(unsigned int i=0;i<info.height;i++)
        {
            unsigned int rowlength=*(unsigned short*)(procBuf+row_table[i]);
            compszie+=(rowlength+2);
        }
        char* compdata=new char[compszie];
        unsigned int dataptr=0;
        for(unsigned int i=0;i<info.height;i++)
        {
            unsigned int rowlength=*(unsigned short*)(procBuf+row_table[i]);
            memcpy(compdata+dataptr,procBuf+row_table[i],rowlength+2);
            row_table[i]=dataptr;
            dataptr+=(rowlength+2);
        }
        auto s25img=new s25Class(compdata,row_table,info,compszie);
        this->s25List.append(s25img);
    }

}

int Align4(int num)
{
    if(num%4==0)
    {
        return num;
    }
    else
    {
        return num+(4-num%4);
    }

}

void S25Arc::save(QString File)
{
    unsigned long FileSize=0;
    unsigned long tottrans=0;
    FileSize+=sizeof(this->head);
    FileSize+=sizeof(unsigned long)*this->head.index_entries;
    for(auto var:this->s25List)
    {
        FileSize+=Align4(var->getPakedSize());
    }
    char* outBuf=new char[FileSize];
    memset(outBuf,0,FileSize);
    memcpy(outBuf+tottrans,&this->head,sizeof(this->head));
    tottrans+=sizeof(this->head);
    unsigned long* entries=(unsigned long*)(outBuf+tottrans);
    memcpy(outBuf+tottrans,this->entries_offset,this->head.index_entries*sizeof(unsigned long));
    tottrans+=sizeof(unsigned long)*(this->head.index_entries);
    for(int i=0;i<this->s25List.size();i++)
    {
        unsigned int headsize=s25List.at(i)->getHeadSize();
        unsigned int paksize=s25List.at(i)->getPakedSize();
        s25List.at(i)->setStartOffset(tottrans+headsize);
        char* PakedImg=s25List.at(i)->getPakedImage();
        memcpy(outBuf+tottrans,PakedImg,paksize);
        delete [] PakedImg;
        entries[ListMap[i]]=tottrans;
        tottrans+=Align4(paksize);
    }
    QFile fout(File);
    fout.open(QIODevice::WriteOnly);
    fout.write(outBuf,FileSize);
    fout.close();
}

void S25Arc::replace(int index, QString File)
{
    s25Class* newimg=new s25Class(File);
    auto oldS25=s25List.at(ListMap.key(index));
    auto oldInfo=oldS25->getImageInfo();
    newimg->setImageInfo(oldInfo);
    s25List.replace(ListMap.key(index),newimg);
}

void S25Arc::dump(int index,QString File)
{
    auto s25=s25List.at(ListMap.key(index));
    int fileSize=0;
    char* filebuf=s25->dumpS25(&fileSize);
    if(fileSize==0||filebuf==nullptr)return;
    QFile fout(File);
    fout.open(QIODevice::WriteOnly);
    fout.write(filebuf,fileSize);
    fout.close();
}

int S25Arc::FileNum()
{
    return this->head.index_entries;
}
