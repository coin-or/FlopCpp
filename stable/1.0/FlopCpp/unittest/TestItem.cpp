#include "TestItem.hpp"
#include "TestBed.hpp"

#include <iostream>

TestBed *TestItem::theBed=NULL;

TestItem::TestItem(const char *itemName)
    :name(NULL),success(false),fileName(NULL)
{
    name = new char[strlen(itemName)+1];
    strcpy(name,itemName);
    isSet=false;
    if(theBed)
        theBed->insert(this);
    else
    {
        std::cerr<<"ERROR: TestItem Static TestBed Not initialized.."<<std::endl;
        std::cerr<<"       Call TestItem::setBed first"<<std::endl;
    }
}

TestItem::~TestItem() {
    delete [] name;
    if(fileName!=NULL)
	delete [] fileName;
    theBed->erase( theBed->find(this));
}

std::ostream &operator<<(std::ostream &os, const TestItem &item) {
    if(item.success)
        os<<"Passed!";
    else
        os<<"Failed!";
    os<<':'<<item.name;
    if(!item.success)
    {
        if(item.isSet)
            os<<" LINE:"<<item.lineNumber<<" in "<<item.fileName;
        else
            os<<"Item Not Tested!!!";
    }
    return os;
}

int TestItem::compare(const TestItem &otherItem)const
{ return strcmp(name,otherItem.name); }

bool operator<(const TestItem &item1, const TestItem &item2)
{ return (item1.compare(item2)<0); }

bool operator>(const TestItem &item1, const TestItem &item2)
{ return (item1.compare(item2)>0); }

bool operator<=(const TestItem &item1, const TestItem &item2)
{ return (item1.compare(item2)<=0); }

bool operator>=(const TestItem &item1, const TestItem &item2)
{ return (item1.compare(item2)>=0); }

bool operator==(const TestItem &item1, const TestItem &item2)
{ return (item1.compare(item2)==0); }

bool operator!=(const TestItem &item1, const TestItem &item2)
{ return (item1.compare(item2)!=0); }

void TestItem::failItem(const char *_fileName, 
			const int lineNum)
{
    if((!isSet)||(isSet&&success))
    {
        success=false;
        isSet=true;
        fileName = new char[strlen(_fileName)+1];
	strcpy(fileName,_fileName);
        lineNumber = lineNum;
    }
}

void TestItem::expectTrue(const char *_fileName, const int lineNum,
                          const bool val)
{
    if(val) 
        passItem(); 
    else 
    {
        std::cout<<"FAIL:"<<lineNum<<':'<<_fileName<<"Expected true, got false"<<std::endl;
        failItem(_fileName,lineNum);
    }
}

void TestItem::expectFalse(const char *_fileName, const int lineNum,
                           const bool val)
{
    if(!val) 
        passItem(); 
    else 
    {
        std::cout<<"FAIL:"<<lineNum<<':'<<_fileName<<"Expected true, got false"<<std::endl;
        failItem(_fileName,lineNum);
    }
}

void TestItem::expectNonZero(const char *_fileName, const int lineNum, 
			     const int val) 
{
    if(val) 
        passItem(); 
    else 
    {
        std::cout<<"FAIL:"<<lineNum<<':'<<_fileName<<" Expected nonZero, got ";
        std::cout<<val<<std::endl;
        failItem(_fileName,lineNum);
    }
}

void TestItem::expectZero(const char *_fileName, const int lineNum, 
			  const int val) 
{
    if(val==0) 
        passItem(); 
    else 
    {
        std::cout<<"FAIL:"<<lineNum<<':'<<_fileName<<" Expected Zero, got ";
        std::cout<<val<<std::endl;
        failItem(_fileName,lineNum);
    }
}

void TestItem::sameString(const char *_fileName, const int lineNum, 
			  const char *string1,const char *string2)
{
    if(strcmp(string1,string2))
    {
        std::cout<<"FAIL:"<<lineNum<<':'<<_fileName<<" Expected "<<string1<<", got ";
        std::cout<<string2<<std::endl;
        failItem(_fileName,lineNum);
    }
    else 
        passItem();
}
