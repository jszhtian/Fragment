#ifndef S25CLASS_H
#define S25CLASS_H
#include <QFile>
#include <QString>
#include <QFile>
#include <QPixmap>
#include <QTextStream>
#include <QList>
#include <QPainter>
struct ARGB
{
    unsigned short A;
    unsigned short R;
    unsigned short G;
    unsigned short B;
};
struct S25_header_t {
    char magic[4];		// "S25"
    signed long index_entries;
};
struct S25_info_t {
    unsigned long width;
    unsigned long height;
    unsigned long orig_x;
    unsigned long orig_y;
    unsigned long addition;
};
class s25Class
{
public:
    s25Class(const ARGB** ARGBMatrix,const S25_info_t info);
    s25Class(const char* CompData,unsigned long* table,const S25_info_t info,unsigned int bufsize);
    s25Class(QString FileName);
    void setStartOffset(int offset);
    char* getPakedImage();
    char* getPurePayload();
    char* dumpS25(int* Size);
    S25_info_t getImageInfo();
    void setImageInfo(S25_info_t info);
    unsigned int getHeadSize();
    unsigned int getPakedSize();
#if(0)
    ARGB* getARGBMatrix(S25_info_t* info);
#endif
private:
#if(0)
    void deCompress();
#endif
    void FakeCompress();
    S25_header_t IMG_head;
    S25_info_t IMG_info;
    unsigned int offset=0;
    unsigned long* row_offset_table=nullptr;
    char* payload=nullptr;
    ARGB* ARGBMatrix=nullptr;
    unsigned int payloadsize;

};

#endif // S25CLASS_H
