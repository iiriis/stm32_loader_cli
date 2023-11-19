#ifndef LOADER_CORE_H
#define LOADER_CORE_H

typedef enum {
    COMM_SUCCESS = 0,
    COMM_ERROR = 1,
    COMM_INIT_ERROR = 2,
    COMM_READ_ERROR = 3,
    COMM_WRITE_ERROR = 4,
} comm_status_t;


/* Function Pointers to hold the custom communication protocol */
typedef struct {
    comm_status_t (*comm_Init)(void);
    comm_status_t (*comm_Read)(uint8_t* pData, uint8_t size);
    comm_status_t (*comm_Write)(uint8_t* pData, uint8_t size);
} comm_t;


#endif // LOADER_CORE_H
