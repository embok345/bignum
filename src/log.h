#ifndef _LOG_H
#define _LOG_H

#include <stdio.h>
#include <string.h>
//#include <stdlib.h>

#define LOG_LEVEL_CRIT 0
#define LOG_LEVEL_ERR 1
#define LOG_LEVEL_WARN 2
#define LOG_LEVEL_FINE 3
#define LOG_LEVEL_TRACING 4

struct log_t {
  const char *log_name;
  int log_level;
};

extern const char *levels[];

#define LOG(LOGGER, LEVEL, MSG) \
({ \
  if(LEVEL < LOGGER.log_level) {\
    char *msg = malloc(strlen(levels[LEVEL]) + strlen(MSG) + 3); \
    sprintf(msg, "%s: %s", levels[LEVEL], MSG); \
    logger(LOGGER, __FILE__, __LINE__, __func__, msg); \
    free(msg); \
  }\
})

#define LOG_C(LOGGER, MSG) LOG(LOGGER, LOG_LEVEL_CRIT, MSG)
#define LOG_E(LOGGER, MSG) LOG(LOGGER, LOG_LEVEL_ERR, MSG)
#define LOG_W(LOGGER, MSG) LOG(LOGGER, LOG_LEVEL_WARN, MSG)
#define LOG_T(LOGGER, MSG) LOG(LOGGER, LOG_LEVEL_TRACING, MSG)

struct log_t init_log(const char *, int);
void logger(struct log_t, const char *, int, const char *, const char *);

#endif
