#include "s25class.h"

s25Class::s25Class(const char *CompData,unsigned long* table, const S25_info_t info,unsigned int CompSize)
{
    payload=new char[CompSize];
    memset(payload,0,CompSize);
    memcpy(payload,CompData,CompSize);
    payloadsize=CompSize;
    IMG_info=info;
    row_offset_table=new unsigned long[IMG_info.height];
    memset(row_offset_table,0,sizeof(unsigned long)*IMG_info.height);
    memcpy(row_offset_table,table,IMG_info.height*sizeof(unsigned long));
}

s25Class::s25Class(QString File)
{
    QFile Fopen(File);
    Fopen.open(QIODevice::ReadOnly);
    QByteArray buf=Fopen.readAll();
    char* procBuf=(char*)buf.data();
    int bufsize=buf.size();
    Fopen.close();
    int totread=0;
    memcpy(&this->IMG_head,procBuf+totread,sizeof(IMG_head));
    totread+=sizeof(IMG_head);
    totread+=4;
    memcpy(&this->IMG_info,procBuf+totread,sizeof(IMG_info));
    totread+=sizeof(IMG_info);
    row_offset_table=new unsigned long[IMG_info.height];
    memcpy(row_offset_table,procBuf+totread,IMG_info.height*sizeof(unsigned long));
    totread+=IMG_info.height*sizeof(unsigned long);
    payloadsize=bufsize-totread;
    payload=new char[payloadsize];
    memcpy(payload,procBuf+totread,payloadsize);
    this->setStartOffset(-totread);
}

void s25Class::setStartOffset(int offset)
{
    this->offset=offset;
    for (unsigned int i=0;i<this->IMG_info.height;i++)
    {
        row_offset_table[i]+=offset;
    }
}

char *s25Class::getPakedImage()
{
    char* outBuf=new char[sizeof(IMG_info)+IMG_info.height*sizeof(unsigned long)+payloadsize];
    int tottrans=0;
    memcpy(outBuf+tottrans,&IMG_info,sizeof(IMG_info));
    tottrans+=sizeof(IMG_info);
    memcpy(outBuf+tottrans,row_offset_table,IMG_info.height*sizeof(unsigned long));
    tottrans+=IMG_info.height*sizeof(unsigned long);
    memcpy(outBuf+tottrans,payload,payloadsize);
    return outBuf;
}

char *s25Class::dumpS25(int *Size)
{
    if(payload==nullptr)
    {
        return nullptr;
    }
    IMG_head.index_entries=1;
    char magic[4]={0x53,0x32,0x35,0x00};
    memcpy(IMG_head.magic,magic,sizeof(magic));
    int totlength=payloadsize+IMG_info.height*sizeof(unsigned long)+sizeof(IMG_info)+sizeof(unsigned long)*IMG_head.index_entries+sizeof(IMG_head);
    int totTrans=0;
    char* outbuf=new char[totlength];
    *Size=totlength;
    memcpy(outbuf,&IMG_head,sizeof(IMG_head));
    totTrans+=sizeof(IMG_head);
    *(long*)(outbuf+totTrans)=sizeof(IMG_head)+sizeof(long);
    totTrans+=sizeof(unsigned long)*IMG_head.index_entries;
    memcpy(outbuf+totTrans,&this->IMG_info,sizeof(IMG_info));
    totTrans+=sizeof(IMG_info);
	int Changedoffset = totTrans + IMG_info.height * sizeof(unsigned long);
    setStartOffset(Changedoffset);
    memcpy(outbuf+totTrans,row_offset_table,IMG_info.height*sizeof(unsigned long));
    totTrans+=IMG_info.height*sizeof(unsigned long);
    memcpy(outbuf+totTrans,payload,payloadsize);
	setStartOffset(-Changedoffset);
    return outbuf;
}

S25_info_t s25Class::getImageInfo()
{
    return this->IMG_info;
}

void s25Class::setImageInfo(S25_info_t info)
{
    memcpy(&this->IMG_info,&info,sizeof(S25_info_t));
}

unsigned int s25Class::getHeadSize()
{
    return sizeof(IMG_info)+IMG_info.height*sizeof(unsigned long);
}

unsigned int s25Class::getPakedSize()
{
    return sizeof(IMG_info)+IMG_info.height*sizeof(unsigned long)+payloadsize;
}
