#include "log.h"

const char *levels[] = {"critical", "error", "warning", "fine"};

struct log_t init_log(const char *log_name, int log_level) {
  struct log_t new_log = {log_name, log_level};
  return new_log;
}

void logger(struct log_t log, const char *file_name, int line_number,
    const char *function_name, const char *msg) {
  FILE *log_file;
  if(strcmp(log.log_name, "stderr") == 0)
    log_file = stderr;
  else if(strcmp(log.log_name, "stdout") == 0)
    log_file = stdout;
  else
    log_file = fopen(log.log_name, "a");

  if(!log_file) return;

  fprintf(log_file, "Function %s at %s:%d: %s\n",
      function_name, file_name, line_number, msg);

  if(log_file != stdout && log_file != stderr)
    fclose(log_file);
}




