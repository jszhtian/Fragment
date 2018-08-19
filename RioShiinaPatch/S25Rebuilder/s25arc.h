#ifndef S25ARC_H
#define S25ARC_H
#include "s25class.h"
#include <QMap>

class S25Arc
{
public:
    S25Arc(QString File);
    void save(QString OutputName);
    void replace(int index,QString File);
    void dump(int index,QString File);
    int FileNum();
private:
    QList<s25Class*> s25List;
    QMap<int,int> ListMap;
    S25_header_t head;
    unsigned long* entries_offset=nullptr;
};

#endif // S25ARC_H
