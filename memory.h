//****************************************************************************
// CS 502 Operating System Project.
// Che Sun at Worcester Polytechnic Institute, Fall 2014.
//****************************************************************************

#ifndef MEMORY_H_
#define MEMORY_H_

#define ALLOC(Type) calloc(1, sizeof(Type))
#define DEALLOC(Object) free((void*)Object); Object = NULL

#endif
