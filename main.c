#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define EXIT_IMPL
#include "exit.h"

#define LEXER_IMPL
#include "lexer.h"

#define PARSE_IMPL
#include "parse.h"

#define EVAL_IMPL
#include "eval.h"

void clear_input(WINDOW *);
void clear_output(WINDOW *);

void ui_init(void) {
  initscr();
  noecho();
  curs_set(1);
  set_escdelay(0); // immediate escape key response
  start_color();
  init_pair(1, COLOR_YELLOW, COLOR_BLACK);
  init_pair(2, COLOR_GREEN,  COLOR_BLACK);
  init_pair(3, COLOR_CYAN,   COLOR_BLACK);
  init_pair(4, COLOR_RED,    COLOR_BLUE);
}

#define LEN_INPUT_BUFFER 280

#ifndef PARSE_TEST
int main(void) {
  WINDOW *w_out = NULL, *w_in = NULL;
  ui_init();
  on_exit(exit_clean_handler, (WINDOW *[]){w_out, w_in});

  int screen_height = getmaxy(stdscr);
  int screen_width  = getmaxx(stdscr);

  w_out = newwin(screen_height - 4, screen_width - 2, 1, 1);
  if (w_out == NULL) exit(EXIT_WIN_ALLOC_FAIL);
  scrollok(w_out, TRUE);
  box(w_out, 0, 0);

  w_in = newwin(3, screen_width - 2, screen_height - 3, 1);
  if (w_in == NULL) exit(EXIT_WIN_ALLOC_FAIL);
  keypad(w_in, TRUE);
  box(w_in, 0, 0);
  mvwprintw(w_in, 1, 1, "> ");

  refresh();
  wrefresh(w_out);
  wrefresh(w_in);

  #define MAX_OUT_LINES 20
  int ch;
  char input_buffer[LEN_INPUT_BUFFER] = {0};
  char output_buffer[MAX_OUT_LINES][LEN_INPUT_BUFFER] = {0};
  size_t cursor_p = 0;
  size_t line_p = 0;

  #define KEY_ESC 27
  #define MAX_NUM_DIGITS 32
  while((ch = wgetch(w_in)) != KEY_ESC) {
    if (ch == '\n') {
      if (line_p > MAX_OUT_LINES) exit(EXIT_OUT_OVERFLOW);
      clear_output(w_out);
      memcpy(output_buffer[line_p++], input_buffer, LEN_INPUT_BUFFER);

      char result_str[MAX_NUM_DIGITS];
      TokenStream ts = lex_expr(input_buffer);
      ASTBinaryNode *ast_root = parse_stream(&ts);
      Token result = eval_ast(ast_root);

      read_from_eval(result_str, result);

      memcpy(output_buffer[line_p++], result_str, MAX_NUM_DIGITS);

      for (size_t y = 0; y < line_p; y++) {
        if (!(y % 2)) wattron(w_out, COLOR_PAIR(1));
        else wattron(w_out, COLOR_PAIR(2));
        mvwprintw(w_out, (int) (1 + y), 1, "%s", output_buffer[y]);
        wattroff(w_out, COLOR_PAIR(1));
        wattroff(w_out, COLOR_PAIR(2));
      }

      memset(input_buffer, 0, LEN_INPUT_BUFFER);
      cursor_p = 0;
      clear_input(w_in);
      wrefresh(w_in);
      wrefresh(w_out);
    } else if (cursor_p < LEN_INPUT_BUFFER) {
      if (ch == KEY_BACKSPACE) {
        cursor_p = cursor_p == 0 ? cursor_p : cursor_p - 1;
        input_buffer[cursor_p] = '\0';
      } else {
        input_buffer[cursor_p++] = (char) ch;
      }
    }
    clear_input(w_in);
    mvwprintw(w_in, 1, 3, "%s", input_buffer);
    wrefresh(w_in);
  }
  return EXIT_SUCCESS;
}

#else

void handle_stream_error(int exit_code, void *args) {
  (void) args;
  token_print_error((error_stream_t) exit_code);
}

int main(void) {
  on_exit(handle_stream_error, NULL);

  char *expr1 = "2 * 3 + (3 * 4 * (10.33 / 2))";

  printf("\nRAW EXPRESSION: %s\n", expr1);
  TokenStream stream = lex_expr(expr1);

  printf("\nTOKEN STREAM:\n");
  token_stream_trace(&stream);

  printf("\nABSTRACT SYNTAX TREE:\n");
  ASTBinaryNode *ast_root = parse_stream(&stream);
  ast_parse_trace(ast_root, 0);

  Token result = eval_ast(ast_root);
  printf("\nRESULT: fval: %g, ival: %ld\n",
         result.value.fval, result.value.ival);

  return EXIT_SUCCESS;
}

#endif // PARSE_TEST

void clear_input(WINDOW *w_in) {
  wmove(w_in, 1, 3);
  wclrtoeol(w_in);
  box(w_in, 0, 0);
}

void clear_output(WINDOW *w_out) {
  wmove(w_out, 1, 1);
  wclrtobot(w_out);
  box(w_out, 0, 0);
}
