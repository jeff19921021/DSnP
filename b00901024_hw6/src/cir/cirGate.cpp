/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-2012 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

extern CirMgr *cirMgr;

/**************************************/
/*   class CirGate member functions   */
/**************************************/



void
CirGate::reportGate() const
{
   cout<<"=================================================="<<endl;
   int b=39-lengthofblank();
   cout<<"= "<<getTypeStr()<<"("<<_id<<")";
   if(_symbolic[0]!='\0')
   {cout<<"\""<<_symbolic<<"\"";}
   cout<<", line "<<_line<<setw(b)<<"="<<endl;
   cout<<"=================================================="<<endl;
}

void
CirGate::reportFanin(int level) const
{
   assert (level >= 0);
   this->printGate();
   cout<<endl;
   if(level)
   {
      cirMgr->resetallflags();
      this->printfanin(level);
   }
}

void
CirGate::reportFanout(int level) const
{
   assert (level >= 0);
   this->printGate();
   cout<<endl;
   if(level)
   {
      cirMgr->resetallflags();
      this->printfanout(level);
   }
}



