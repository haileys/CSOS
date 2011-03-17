#ifndef STDARG_H
#define STDARG_H

typedef char* va_list;

#define va_start(list,argbefore) list = (char*)&(argbefore)
#define va_arg(list,type) (*(type*)((list) += 4))
#define va_end(list) while(0){}

#endif