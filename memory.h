#ifndef MEMORY_H_
#define MEMORY_H_

#define ALLOC(Type) calloc(1, sizeof(Type))
#define DEALLOC(Object) free(Object); Object = NULL

#endif