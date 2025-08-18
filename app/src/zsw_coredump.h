
#pragma once

#define ZSW_COREDUMP_MAX_FILENAME_LEN   32
#define ZSW_COREDUMP_DATETIME_LEN       32
#define ZSW_COREDUMP_MAX_STORED         1

typedef struct zsw_coredump_summary_t {
    char datetime[ZSW_COREDUMP_DATETIME_LEN + 1];
    char file[ZSW_COREDUMP_MAX_FILENAME_LEN + 1];
    int line;
} zsw_coredump_summary_t;

int zsw_coredump_init(void);

/*
* @brief: Dumps the coredump using the logging backend.
*/
int zsw_coredump_to_log(void);

void zsw_coredump_erase(int index);

int zsw_coredump_get_summary(zsw_coredump_summary_t *summary, int max_dumps, int *num_dumps);
