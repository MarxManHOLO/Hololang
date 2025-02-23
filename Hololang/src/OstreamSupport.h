#ifndef OSTREAMSUPPORT_H
#define OSTREAMSUPPORT_H

#include "Hololang/SimpleString.h"
#include "Hololang/List.h"

#include <iostream>
using std::ostream;

ostream & operator<< (ostream &out, const Hololang::String &rhs);
ostream & operator<< (ostream &out, const Hololang::List<int> &rhs);
ostream & operator<< (ostream &out, const Hololang::List<Hololang::String> &rhs);


#endif  
