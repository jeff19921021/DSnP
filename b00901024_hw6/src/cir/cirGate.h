/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-2012 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <vector>
#include <iostream>
#include <climits>
#include "cirDef.h"

using namespace std;

class CirGate;

class CirGateV
{
public:
   CirGateV(CirGate* g,int p)
   :_gatev(int(g)+p) { }
   CirGate* gate() const  {return (CirGate*)(_gatev&(INT_MAX-3));}
   bool isinv() const  {return (_gatev&1);}  
private:
   int _gatev;
};

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
class CirGate
{
public:
   CirGate(int i,int l,GateType t)
   {_id=i;_line=l;_type=t;_fanin1=0;_fanin2=0;_symbolic="";_flag=0;}
   ~CirGate() {ReleaseMemory();}


   // Basic access methods
   string getTypeStr() const 
   { 
      switch(_type)
      {
         case AIG_GATE:
            return "AIG";
         case PI_GATE:
            return "PI";
         case PO_GATE:
            return "PO";
         case UNDEF_GATE:
            return "UNDEF";
         default:
            return "CONST";
      }         
   }

   int getLineNo() const  {return _line;}
   int getid() const  {return _id;}
   size_t getfanoutsize() const  {return _fanoutlist.size();}
   GateType gettype() const  {return _type;}
   bool getflag() const  {return _flag;}
   string getsymbolic() const  {return _symbolic;}
   CirGateV* getfanin1() const  {return _fanin1;}
   CirGateV* getfanin2() const  {return _fanin2;}


   // Printing functions
   void printGate() const  {cout<<getTypeStr()<<" "<<_id;}

   void reportGate() const;
   void reportFanin(int level) const;
   void reportFanout(int level) const;


   void setsymbolic(string sb)  {_symbolic=sb;}
   void setfanin1(CirGate* in1,int p1)
   {_fanin1=new CirGateV(in1,p1);}
   void setfanin2(CirGate* in2,int p2)
   {_fanin2=new CirGateV(in2,p2);}
   void setfanout(CirGate* out,int p)
   {_fanoutlist.push_back(new CirGateV(out,p));}
   void resetflag()  {_flag=0;}
   void setflag()  const {_flag=1;}


   // Useful functions  
   int lengthofblank() const
   {
      int b=_symbolic.length();
      if(b!=0)  {b+=2;;}
      int l=_line,i=_id;
      if(l==0)  {b++;}
      if(i==0)  {b++;}
      while(l!=0)
      {l/=10;  b++;}
      while(i!=0)
      {i/=10;  b++;}
      switch(_type)
      {
         case AIG_GATE:
            return (b+=5);
         case PI_GATE:
            return (b+=5);
         case PO_GATE:
            return (b+=5);
         case UNDEF_GATE:
            return (b+=5);
         default:
            return (b+=5);
      } 
   }

   bool isfloating() const  
   {
     if( !(_fanin1) )  {return false;}
     if( (_fanin1->gate())->gettype()==UNDEF_GATE )  {return true;}
     if( _fanin2 && (_fanin2->gate())->gettype()==UNDEF_GATE)  {return true;}
     else  {return false;}
   }

   void ReleaseMemory() 
   {
       if(_fanin1)  {delete _fanin1;}
       if(_fanin2)  {delete _fanin2;}
       size_t s=_fanoutlist.size();
       for(size_t i=0;i<s;i++)
       {delete _fanoutlist[i];}
   }

   void printfanin(int level,int i=0) const
   {
      bool t=0;
      if(this->_fanin1!=0)
      {
         t=1;
         i++;
         cout<<setfill(' ')<<setw(i*2)<<"";
         if( this->_fanin1->isinv() )  {cout<<"!";}   
         if( !this->_fanin1->gate()->getflag() )  
         {
            this->_fanin1->gate()->setflag();
            this->_fanin1->gate()->printGate();
            cout<<endl;
            if(i<level)  
            {this->_fanin1->gate()->printfanin(level,i);}
         }
         else if(i<level && this->_fanin1->gate()->gettype()==AIG_GATE)
         {
            this->_fanin1->gate()->printGate();
            cout<<" (*)"<<endl;
         }
         else 
         {
            this->_fanin1->gate()->printGate();
            cout<<endl;
         }
      }
      if(this->_fanin2!=0)
      {
         if(!t)  {i++;}
         cout<<setfill(' ')<<setw(i*2)<<"";
         if( this->_fanin2->isinv() )  {cout<<"!";}   
         if( !this->_fanin2->gate()->getflag() )  
         {
            this->_fanin2->gate()->setflag();
            this->_fanin2->gate()->printGate();
            cout<<endl;  
            if(i<level)  
            {this->_fanin2->gate()->printfanin(level,i);}
         }
         else if(i<level && this->_fanin2->gate()->gettype()==AIG_GATE)
         {
            this->_fanin2->gate()->printGate();
            cout<<" (*)"<<endl;
         }
         else 
         {
            this->_fanin2->gate()->printGate();
            cout<<endl;
         }
      }
   }

   void printfanout(int level,int i=0) const
   {
      bool t=0;
      for(size_t j=0;j<_fanoutlist.size();j++)
      {
         if(this->_fanoutlist[j]!=0)
         {
            if(!t)  {i++;}
            t=1;
            cout<<setfill(' ')<<setw(i*2)<<"";
            if( this->_fanoutlist[j]->isinv() )  {cout<<"!";}   
            if( !this->_fanoutlist[j]->gate()->getflag() )  
            {
               this->_fanoutlist[j]->gate()->setflag();
               this->_fanoutlist[j]->gate()->printGate();
               cout<<endl;
               if(i<level)  
               {this->_fanoutlist[j]->gate()->printfanout(level,i);}
            }
            else if(i<level && this->_fanoutlist[j]->gate()->gettype()==AIG_GATE)
            {
               this->_fanoutlist[j]->gate()->printGate();
               cout<<" (*)"<<endl;
            }
            else 
            {
               this->_fanoutlist[j]->gate()->printGate();
               cout<<endl;
            }
         }   
      }
   }

private:
   GateType _type;
   mutable bool _flag;
   CirGateV* _fanin1;
   CirGateV* _fanin2;
   vector<CirGateV*> _fanoutlist;
   int _id,_line;
   string _symbolic;
};

#endif // CIR_GATE_H
