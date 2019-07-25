#ifndef _LOG_H
#define _LOG_H

#include <stdio.h>
#include <string.h>
//#include <stdlib.h>

#define LOG_LEVEL_CRIT 0
#define LOG_LEVEL_ERR 1
#define LOG_LEVEL_WARN 2
#define LOG_LEVEL_FINE 3

struct log_t {
  const char *log_name;
  int log_level;
};

extern const char *levels[];

#define LOG(LOG_T, LEVEL, MSG) \
({ \
  if(LEVEL < LOG_T.log_level) {\
    char *msg = malloc(strlen(levels[LEVEL]) + strlen(MSG) + 3); \
    sprintf(msg, "%s: %s", levels[LEVEL], MSG); \
    logger(LOG_T, __FILE__, __LINE__, __func__, msg); \
    free(msg); \
  }\
})

struct log_t init_log(const char *, int);
void logger(struct log_t, const char *, int, const char *, const char *);

#endif
