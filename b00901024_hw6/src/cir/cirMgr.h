/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-2012 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

#include "cirDef.h"
#include "cirGate.h"

extern CirMgr *cirMgr;

// TODO: Define your own data members and member functions
class CirMgr
{
public:
   CirMgr(){}
   ~CirMgr() {releasememory();}

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(int gid) const 
   {
      if( gid>M+O || !_totallist[gid] )  {return 0;}
      else  {return _totallist[gid];} 
   }

   // Member functions about circuit construction
   bool readCircuit(const string&);
   void buildfloatingandunused()
   {
      for(int i=1;i<M+O+1;i++)
      {
         if( _totallist[i]  && _totallist[i]->isfloating() )
         {_floatinglist.push_back(_totallist[i]);}  
      }
      for(int i=1;i<M+1;i++)
      {
         if( _totallist[i] && _totallist[i]->getfanoutsize()==0 )
         {_unusedlist.push_back(_totallist[i]);}  
      } 
   }

   void dfstraversal(CirGate* g)
   {
      if(g->getfanin1()!=0)
      {
         if( !g->getfanin1()->gate()->getflag() )  
         {
            g->getfanin1()->gate()->setflag();
            dfstraversal(g->getfanin1()->gate());
         }
      }
      if(g->getfanin2()!=0)
      {
         if( !g->getfanin2()->gate()->getflag() )  
         {
            g->getfanin2()->gate()->setflag();
            dfstraversal(g->getfanin2()->gate());
         }
      }

      if(g->gettype()!=UNDEF_GATE)
      {_dfslist.push_back(g);}
   }
   void builddfslist()
   {
      resetallflags();
      for(int i=0;i<O;i++)
      {dfstraversal(_polist[i]);}
   }

   void resetallflags()
   {
      for(int i=0;i<M+O+1;i++)
      {   
         if(_totallist[i])  
         {_totallist[i]->resetflag();}
      }
   }

   
   void releasememory()
   {
      size_t s=_totallist.size();
      for(size_t i=0;i<s;i++)
      {if(_totallist[i])  delete _totallist[i];}
   }

   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void writeAag(ostream&) const;


private:
   int M,I,L,O,A;
   GateList _pilist,_polist,_aiglist,_totallist,_floatinglist,_unusedlist,_dfslist;
};

#endif // CIR_MGR_H
