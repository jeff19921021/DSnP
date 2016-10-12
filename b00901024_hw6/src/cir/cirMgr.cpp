/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-2012 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;

enum CirParseError {
   EXTRA_SPACE,
   MISSING_SPACE,
   ILLEGAL_WSPACE,
   ILLEGAL_NUM,
   ILLEGAL_IDENTIFIER,
   ILLEGAL_SYMBOL_TYPE,
   ILLEGAL_SYMBOL_NAME,
   MISSING_NUM,
   MISSING_IDENTIFIER,
   MISSING_NEWLINE,
   MISSING_DEF,
   CANNOT_INVERTED,
   MAX_LIT_ID,
   REDEF_GATE,
   REDEF_SYMBOLIC_NAME,
   REDEF_CONST,
   NUM_TOO_SMALL,
   NUM_TOO_BIG,

   DUMMY_END
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static unsigned lineNo = 0;  // in printint, lineNo needs to ++
static unsigned colNo  = 0;  // in printing, colNo needs to ++
static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;

static bool
parseError(CirParseError err)
{
   switch (err) {
      case EXTRA_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Extra space character is detected!!" << endl;
         break;
      case MISSING_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing space character!!" << endl;
         break;
      case ILLEGAL_WSPACE: // for non-space white space character
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal white space char(" << errInt
              << ") is detected!!" << endl;
         break;
      case ILLEGAL_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
              << errMsg << "!!" << endl;
         break;
      case ILLEGAL_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
              << errMsg << "\"!!" << endl;
         break;
      case ILLEGAL_SYMBOL_TYPE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal symbol type (" << errMsg << ")!!" << endl;
         break;
      case ILLEGAL_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Symbolic name contains un-printable char(" << errInt
              << ")!!" << endl;
         break;
      case MISSING_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing " << errMsg << "!!" << endl;
         break;
      case MISSING_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
              << errMsg << "\"!!" << endl;
         break;
      case MISSING_NEWLINE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": A new line is expected here!!" << endl;
         break;
      case MISSING_DEF:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
              << " definition!!" << endl;
         break;
      case CANNOT_INVERTED:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": " << errMsg << " " << errInt << "(" << errInt/2
              << ") cannot be inverted!!" << endl;
         break;
      case MAX_LIT_ID:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
              << endl;
         break;
      case REDEF_GATE:
         cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
              << "\" is redefined, previously defined as "
              << errGate->getTypeStr() << " in line " << errGate->getLineNo()
              << "!!" << endl;
         break;
      case REDEF_SYMBOLIC_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
              << errMsg << errInt << "\" is redefined!!" << endl;
         break;
      case REDEF_CONST:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Cannot redefine const (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_SMALL:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too small (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_BIG:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too big (" << errInt << ")!!" << endl;
         break;
      default: break;
   }
   return false;
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
bool
CirMgr::readCircuit(const string& fileName)
{
   ifstream infile(fileName.c_str());
   if( !infile.is_open() )  
   {
      cout<<"Cannot open design \""<<fileName<<"\"!!"<<endl;
      return false;
   }
   lineNo=0;  colNo=0;
   infile.getline(buf,1024,' ');
   if(buf[0]=='\0')  {return parseError(EXTRA_SPACE);}
   if(buf[0]!='a')  {errMsg=buf;  return parseError(ILLEGAL_IDENTIFIER);}
   if(buf[1]!='a')  {errMsg=buf;  return parseError(ILLEGAL_IDENTIFIER);}
   if(buf[2]!='g')  {errMsg=buf;  return parseError(ILLEGAL_IDENTIFIER);}
   if(isdigit(buf[3]))  {colNo=3;  return parseError(MISSING_SPACE);}
   if(buf[3]!='\0')  {errMsg=buf;  return parseError(ILLEGAL_IDENTIFIER);}
   colNo=4;

   infile.getline(buf,1024,' ');
   if(buf[0]=='\0')  {return parseError(EXTRA_SPACE);}   
   colNo++;  for(int i=0;buf[i]!='\0';i++)  {colNo++;}
   myStr2Int(buf,M);

   infile.getline(buf,1024,' ');   
   if(buf[0]=='\0')  {return parseError(EXTRA_SPACE);}
   colNo++;  for(int i=0;buf[i]!='\0';i++)  {colNo++;}
   myStr2Int(buf,I);

   infile.getline(buf,1024,' ');
   if(buf[0]=='\0')  {return parseError(EXTRA_SPACE);}
   colNo++;  for(int i=0;buf[i]!='\0';i++)  {colNo++;}
   myStr2Int(buf,L);

   infile.getline(buf,1024,' ');
   if(buf[0]=='\0')  {return parseError(EXTRA_SPACE);}
   colNo++;  for(int i=0;buf[i]!='\0';i++)  {colNo++;}
   myStr2Int(buf,O);

   infile.getline(buf,1024);
   if(buf[0]==' ')  {return parseError(EXTRA_SPACE);}
   colNo=0;lineNo++;
   myStr2Int(buf,A);

   for(int i=0;i<M+O+1;i++)  {_totallist.push_back(0);}
   _totallist[0]=new CirGate(0,0,CONST_GATE);
   int lid;
   for(int i=0;i<I;i++)  
   {
      infile.getline(buf,1024);  
      lineNo++; 
      myStr2Int(buf,lid);
      _pilist.push_back(new CirGate(lid/2,i+2,PI_GATE));
      _totallist[lid/2]=_pilist[i];
   }
   for(int i=0;i<O;i++)  
   {
      infile.getline(buf,1024);   
      lineNo++;
      _polist.push_back(new CirGate(M+i+1,i+I+2,PO_GATE));
      _totallist[M+i+1]=_polist[i];
   }
   for(int i=0;i<A;i++)  
   {
      infile.getline(buf,1024,' ');  
      myStr2Int(buf,lid); 
      _aiglist.push_back(new CirGate(lid/2,i+I+O+2,AIG_GATE));
      _totallist[lid/2]=_aiglist[i];
      infile.getline(buf,1024); 
      lineNo++;
   }
   while(!infile.eof())
   {
      infile.getline(buf,1024,' ');
      if(buf[0]=='i')
      {
         myStr2Int(&(buf[1]),lid);
         if(lid>I)  {errMsg="PI index";  errInt=lid;  return parseError(NUM_TOO_BIG);}
         infile.getline(buf,1024);  
         lineNo++;
         _pilist[lid]->setsymbolic(buf);
      }
      else if(buf[0]=='o')
      {
         myStr2Int(&(buf[1]),lid);
         if(lid>O)  {errMsg="PO index";  errInt=lid;  return parseError(NUM_TOO_BIG);}
         infile.getline(buf,1024);  
         lineNo++;
         _polist[lid]->setsymbolic(buf);
      }
      else if(buf[0]=='c')  {break;}
   }
   infile.clear();
   infile.seekg(0,ios::beg);
   for(int i=0;i<I+1;i++)
   {infile.getline(buf,1024);  lineNo++;}
   for(int i=0;i<O;i++)
   {
      infile.getline(buf,1024);
      lineNo++;
      myStr2Int(buf,lid); 
      if(!_totallist[lid/2])  {_totallist[lid/2]=new CirGate(lid/2,0,UNDEF_GATE);}
      _totallist[lid/2]->setfanout(_polist[i],lid%2);
      _polist[i]->setfanin1(_totallist[lid/2],lid%2);
   }
   for(int i=0;i<A;i++)
   {
      infile.getline(buf,1024,' ');

      infile.getline(buf,1024,' ');
      myStr2Int(buf,lid); 
      if(!_totallist[lid/2])  {_totallist[lid/2]=new CirGate(lid/2,0,UNDEF_GATE);}
      _totallist[lid/2]->setfanout(_aiglist[i],lid%2);
      _aiglist[i]->setfanin1(_totallist[lid/2],lid%2);

      infile.getline(buf,1024);
      lineNo++;
      myStr2Int(buf,lid); 
      if(!_totallist[lid/2])  {_totallist[lid/2]=new CirGate(lid/2,0,UNDEF_GATE);}
      _totallist[lid/2]->setfanout(_aiglist[i],lid%2);
      _aiglist[i]->setfanin2(_totallist[lid/2],lid%2);
   }
   infile.close();  
   buildfloatingandunused();
   builddfslist();
   return true;
}

/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************
Circuit Statistics
==================
  PI          20
  PO          12
  AIG        130
------------------
  Total      167
*********************/
void
CirMgr::printSummary() const
{
   cout<<endl;
   cout<<"Circuit Statistics"<<endl;
   cout<<"=================="<<endl;
   cout<<"  PI"<<setw(12)<<I<<endl;
   cout<<"  PO"<<setw(12)<<O<<endl;
   cout<<"  AIG"<<setw(11)<<A<<endl;
   cout<<"------------------"<<endl;
   cout<<"  Total"<<setw(9)<<I+O+A<<endl;
}

void
CirMgr::printNetlist() const
{
   size_t s=_dfslist.size();
   cout<<endl;
   for(size_t i=0;i<s;i++)
   {
      cout<<"["<<i<<"] ";
      switch(_dfslist[i]->gettype())
      {
         case AIG_GATE:
            cout<<_dfslist[i]->getTypeStr()<<" "<<_dfslist[i]->getid()<<" ";
            if(_dfslist[i]->getfanin1()->gate()->gettype()==UNDEF_GATE)  {cout<<"*";}
            if(_dfslist[i]->getfanin1()->isinv())  {cout<<"!";}
            cout<<_dfslist[i]->getfanin1()->gate()->getid()<<" ";
            if(_dfslist[i]->getfanin2()->gate()->gettype()==UNDEF_GATE)  {cout<<"*";}
            if(_dfslist[i]->getfanin2()->isinv())  {cout<<"!";}
            cout<<_dfslist[i]->getfanin2()->gate()->getid();
            break;
         case PI_GATE:
            cout<<_dfslist[i]->getTypeStr()<<"  "<<_dfslist[i]->getid()<<" ";
            if((_dfslist[i]->getsymbolic())[0]!='\0')  {cout<<"("<<_dfslist[i]->getsymbolic()<<")";}
            break;
         case PO_GATE:
            cout<<_dfslist[i]->getTypeStr()<<"  "<<_dfslist[i]->getid()<<" ";
            if(_dfslist[i]->getfanin1()->gate()->gettype()==UNDEF_GATE)  {cout<<"*";}
            if(_dfslist[i]->getfanin1()->isinv())  {cout<<"!";}
            cout<<_dfslist[i]->getfanin1()->gate()->getid()<<" ";
            if((_dfslist[i]->getsymbolic())[0]!='\0')  {cout<<"("<<_dfslist[i]->getsymbolic()<<")";}
            break;
         default:
            cout<<"CONST0";
            break;
      }    
      cout<<endl;
   }
}

void
CirMgr::printPIs() const
{
   cout<<"PIs of the circuit:";
   for(int i=0;i<I;i++)
   {cout<<" "<<_pilist[i]->getid();}
   cout<<endl;
}

void
CirMgr::printPOs() const
{
   cout<<"POs of the circuit:";
   for(int i=0;i<O;i++)
   {cout<<" "<<_polist[i]->getid();}
   cout<<endl;
}

void
CirMgr::printFloatGates() const
{
   if(!_floatinglist.empty())
   {
      size_t s=_floatinglist.size();
      cout<<"Gates with floating fanin(s):";
      for(size_t i=0;i<s;i++)
      {cout<<" "<<_floatinglist[i]->getid();}
      cout<<endl;
   }
   if(!_unusedlist.empty())
   {
      size_t s=_unusedlist.size();
      cout<<"Gates defined but not used  :";
      for(size_t i=0;i<s;i++)
      {cout<<" "<<_unusedlist[i]->getid();}
      cout<<endl;
   }
}

void
CirMgr::writeAag(ostream& outfile) const
{  
   size_t s=_dfslist.size();
   int aignum=0;
   for(size_t i=0;i<s;i++)
   {
      if( _dfslist[i]->gettype()==AIG_GATE )  {aignum++;}
   } 
   outfile<<"aag "<<M<<" "<<I<<" "<<L<<" "<<O<<" ";
   outfile<<aignum<<endl;
   s=_pilist.size();
   for(size_t i=0;i<s;i++)
   {outfile<<(_pilist[i]->getid())*2<<endl;}
   s=_polist.size();
   for(size_t i=0;i<s;i++)
   {outfile<<(_polist[i]->getfanin1()->gate()->getid())*2+(_polist[i]->getfanin1()->isinv())<<endl;}
   s=_dfslist.size();
   for(size_t i=0;i<s;i++)
   {
      if( _dfslist[i]->gettype()==AIG_GATE )
      {
         outfile<<(_dfslist[i]->getid())*2<<" ";
         outfile<<(_dfslist[i]->getfanin1()->gate()->getid())*2+(_dfslist[i]->getfanin1()->isinv())<<" ";
         outfile<<(_dfslist[i]->getfanin2()->gate()->getid())*2+(_dfslist[i]->getfanin2()->isinv());
         outfile<<endl;
      }   
   }    
   s=_pilist.size();
   for(size_t i=0;i<s;i++)
   {
      if( (_pilist[i]->getsymbolic())[0]!='\0' )
      {outfile<<"i"<<i<<" "<<_pilist[i]->getsymbolic()<<endl;}
   }
   s=_polist.size();
   for(size_t i=0;i<s;i++)
   {
      if( (_polist[i]->getsymbolic())[0]!='\0' )
      {outfile<<"o"<<i<<" "<<_polist[i]->getsymbolic()<<endl;}
   }
   outfile<<"c"<<endl;
   outfile<<"AAG output by Jeff Chang"<<endl<<"I love DSnp!!!"<<endl;
}
