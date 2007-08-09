#ifndef _TESTITEM_HPP_
#define _TESTITEM_HPP_

#include <string>
#include <stdlib.h>
#include <ostream>

class TestBed;

#define __SPOT__ __FILE__,__LINE__

class TestItem {
    static TestBed *theBed;
    char *name;
    bool success;
    bool isSet;
    char *fileName;
    int lineNumber;
    int compare(const TestItem &otherItem)const;
public:
    static void setBed(TestBed *aBed){theBed=aBed;}
    TestItem(const char *itemName);
    ~TestItem();

    void passItem(){if(!isSet) success=true;isSet=true;}

    void failItem(const char *_fileName, 
                  const int lineNum);

    void expectNonZero(const char *_fileName, 
                       const int lineNum, 
                       const int val);

    void expectFalse(const char *_fileName, 
		     const int lineNum, 
		     const bool val);

    void expectTrue(const char *_fileName, 
                    const int lineNum, 
                    const bool val);

    void expectZero(const char *_fileName, 
                    const int lineNum, 
                    const int val);

    void sameString(const char *fileName, const int lineNum, 
                    const char *string1,const char *string2);

    bool passed(){ return success; }

    friend std::ostream &operator<<(std::ostream &os, const TestItem &item);

    friend bool operator<(const TestItem &item1, const TestItem &item2);
    friend bool operator>(const TestItem &item1, const TestItem &item2);
    friend bool operator<=(const TestItem &item1, const TestItem &item2);
    friend bool operator>=(const TestItem &item1, const TestItem &item2);
    friend bool operator==(const TestItem &item1, const TestItem &item2);
    friend bool operator!=(const TestItem &item1, const TestItem &item2);
};
#endif
