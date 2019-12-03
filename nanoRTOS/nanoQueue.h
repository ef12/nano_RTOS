/*
 * nanoQueue.h
 *
 *  Created on: 25 Nov 2019
 *      Author: Ehud Frank
 */

#ifndef NANOQUEUE_H_
#define NANOQUEUE_H_
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Macros to simulate c++ like template for creating a private fifo
 * Interface:
 * void name##_init(name##_t* self, T* values, int capacity);
 * int name##_is_empty(name##_t* self);
 * int name##_is_full(name##_t* self);
 * int name##_in(name##_t* self, T* value);
 * int name##_out(name##_t* self, T* value);
 * int name##_capacity(name##_t* self);
 */
#include "string.h"

/**
 * @brief Error code
 */
#ifndef nOS_QUEUE_OK
#define nOS_QUEUE_OK                    0
#endif
#ifndef nOS_QUEUE_NULL_POINTER
#define nOS_QUEUE_NULL_POINTER          1
#endif
#ifndef nOS_QUEUE_ARRAY_NULL_POINTER
#define nOS_QUEUE_ARRAY_NULL_POINTER    2
#endif
#ifndef nOS_QUEUE_WITHOUT_CAPACITY
#define nOS_QUEUE_WITHOUT_CAPACITY      3
#endif
#ifndef nOS_QUEUE_OVERFLOWED
#define nOS_QUEUE_OVERFLOWED            4
#endif
#ifndef nOS_QUEUE_EMPTY
#define nOS_QUEUE_EMPTY                 5
#endif
#ifndef nOS_QUEUE_ERR_UNKNOWN
#define nOS_QUEUE_ERR_UNKNOWN           (-1)
#endif

#define nOS_CREATE_TYPED_QUEUE(name, T)\
typedef struct \
{\
    int count;\
    int index;\
    int outdex;\
    int capacity;\
    T* values;\
} name##_t;\
int name##_init(name##_t* self, void* values, int capacity);\
int name##_is_empty(name##_t* self);\
int name##_is_full(name##_t* self);\
int name##_in(name##_t* self, void* value);\
int name##_out(name##_t* self, void* value);\
int name##_capacity(name##_t* self);

#define nOS_INSTALL_QUEUE_APIs(name, T)\
int name##_init(name##_t* self, void* values, int capacity)\
{\
    if (NULL == self)/* Check if queue was initiates*/\
        {return nOS_QUEUE_NULL_POINTER;} /* Return error code */\
    if (NULL == values)/* Check if queue was initiates*/\
        {return nOS_QUEUE_ARRAY_NULL_POINTER;} /* Return error code */\
    if (0 == capacity)/* Check if capacity is not 0 */\
        {return nOS_QUEUE_WITHOUT_CAPACITY;} /* Return error code */\
    self->count = 0;\
    self->outdex = 0;\
    self->index = 0;\
    self->capacity = capacity;\
    memset( values, 0, capacity * sizeof(T) );\
    self->values = (T*)values;\
    return nOS_QUEUE_OK;\
}\
int name##_is_empty(name##_t* self)\
{\
    return self->count == 0;\
}\
int name##_is_full(name##_t* self)\
{\
    return self->count == self->capacity;\
}\
int name##_in(name##_t* self, void* value)\
{\
    int ret = nOS_QUEUE_OK;\
    \
    if (NULL == self)/* Check if queue was initiates*/\
        {return nOS_QUEUE_NULL_POINTER;} /* Return error code */\
    if (NULL == self->values)/* Check if queue was initiates*/\
        return nOS_QUEUE_ARRAY_NULL_POINTER; /* Return error code */\
    if (self->count >= self->capacity)\
        ret = nOS_QUEUE_OVERFLOWED;/* This is only warning */\
        /* !!! IT IS THE USER RESPONSIBILITY TO CHECK IF QUEUE IS FULL !!! */\
    self->count++;\
    self->values[self->index++] = *(T*)value;\
    if (self->index >= self->capacity)\
        self->index = 0;\
    return ret;\
}\
int name##_out(name##_t* self, void* value)\
{\
    if (NULL == self)/* Check if queue was initiates*/\
        {return nOS_QUEUE_NULL_POINTER;} /* Return error code */\
    if (NULL == self->values)/* Check if queue was initiates*/\
        {return nOS_QUEUE_ARRAY_NULL_POINTER;} /* Return error code */\
    if (self->count <= 0)\
        {return nOS_QUEUE_EMPTY;}\
    *((T*)value) = self->values[self->outdex++];\
    self->count--;\
    if (self->outdex >= self->capacity)\
        self->outdex = 0;\
    return nOS_QUEUE_OK;\
}\
int name##_capacity(name##_t* self)\
{\
    return self->capacity;\
}

#ifdef __cplusplus
}
#endif
#endif /* NANOQUEUE_H_ */
