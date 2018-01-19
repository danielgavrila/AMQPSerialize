#ifndef DESCPROD_H
#define DESCPROD_H
#include <string>
#include <vector>
//From library rainbow, file RbPdSortType.cpp
struct DescriptionProduct{
    std::string sensorId;
    std::string pdType;
    std::string dataType;
    std::string pdfName;
};
using VctDescProduct=std::vector<DescriptionProduct>;
inline DescriptionProduct LastElement()
{
    return DescriptionProduct{"last","last","last","last"};
}

bool operator==(const DescriptionProduct&a,const DescriptionProduct&b)
{
    return a.sensorId==b.sensorId&&
           a.pdType==b.pdType&&
           a.dataType==b.dataType&&
           a.pdfName==b.pdfName;
}
#endif // DESCPROD_H
