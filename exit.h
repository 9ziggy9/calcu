#ifndef EXIT_H_
#define EXIT_H_

#define PANIC(ERR)                                   \
  do {                                               \
    fprintf(stderr, "PANIC --> %s() :: ", __func__); \
    exit(ERR);                                       \
  } while(0);                                        \


#define EXIT_WIN_RANGE(EXIT_CODE) EXIT_CODE >= 6000 && EXIT_CODE <= 6999
typedef enum {
  EXIT_WIN_ALLOC_FAIL = 6000 ,
  EXIT_OUT_OVERFLOW          ,
} exit_win_t;

#define EXIT_EVAL_RANGE(EXIT_CODE) EXIT_CODE >= 7000 && EXIT_CODE <= 7999
typedef enum {
  EXIT_DIVIDE_BY_ZERO = 7000 ,
} exit_eval_t;

#define EXIT_PARSE_RANGE(EXIT_CODE) EXIT_CODE >= 8000 && EXIT_CODE <= 8999
typedef enum {
  EXIT_PARSE_OVERFLOW = 8000   ,
  EXIT_PARSE_UNDERFLOW         ,
  EXIT_PARSE_INVALID_CHAR      ,
  EXIT_PARSE_NODE_ALLOC_FAIL   ,
  EXIT_PARSE_STACK_ALLOC_FAIL  ,
  EXIT_PARSE_UNBALANCED_PARENS ,
} exit_parse_t;

#define EXIT_STREAM_RANGE(EXIT_CODE) EXIT_CODE >= 9000 && EXIT_CODE <= 9999
typedef enum {
  EXIT_STREAM_OVERFLOW = 9000       ,
  EXIT_STREAM_ALLOC_FAIL            ,
  EXIT_STREAM_INVALID_CHAR          ,
  EXIT_STREAM_UNEXPECTED_TERMINATOR ,
} exit_lex_t;

#define LOG_BREAK_ERR(S) case S: fprintf(stderr, ""#S"\n"); break;

#ifdef EXIT_IMPL
static void lex_exit_log(exit_lex_t e) {
  if (EXIT_STREAM_RANGE(e)) {
    switch (e) {
    LOG_BREAK_ERR(EXIT_STREAM_OVERFLOW);
    LOG_BREAK_ERR(EXIT_STREAM_ALLOC_FAIL);
    LOG_BREAK_ERR(EXIT_STREAM_INVALID_CHAR);
    LOG_BREAK_ERR(EXIT_STREAM_UNEXPECTED_TERMINATOR);
    }
  }
}

static void parse_exit_log(exit_parse_t e) {
  if (EXIT_PARSE_RANGE(e)) {
    switch (e) {
    LOG_BREAK_ERR(EXIT_PARSE_OVERFLOW);
    LOG_BREAK_ERR(EXIT_PARSE_UNDERFLOW);
    LOG_BREAK_ERR(EXIT_PARSE_INVALID_CHAR);
    LOG_BREAK_ERR(EXIT_PARSE_NODE_ALLOC_FAIL);
    LOG_BREAK_ERR(EXIT_PARSE_STACK_ALLOC_FAIL);
    LOG_BREAK_ERR(EXIT_PARSE_UNBALANCED_PARENS);
    }
  }
}

static void eval_exit_log(exit_eval_t e) {
  if (EXIT_EVAL_RANGE(e)) {
    switch (e) {
    LOG_BREAK_ERR(EXIT_DIVIDE_BY_ZERO);
    }
  }
}

static void win_exit_log(exit_win_t e) {
  if (EXIT_WIN_RANGE(e)) {
    switch(e) {
    LOG_BREAK_ERR(EXIT_WIN_ALLOC_FAIL);
    LOG_BREAK_ERR(EXIT_OUT_OVERFLOW);
    }
  }
}

void exit_clean_handler(int exit_code, void *args) {
  WINDOW **ws = (WINDOW **) args;
  delwin(ws[0]);
  delwin(ws[1]);
  endwin();
  win_exit_log((exit_win_t) exit_code);
  eval_exit_log((exit_eval_t) exit_code);
  parse_exit_log((exit_parse_t) exit_code);
  lex_exit_log((exit_lex_t) exit_code);
}

#endif // EXIT_IMPL
#endif // EXIT_H_
