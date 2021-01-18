#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// token kinds
typedef enum {
  TK_RESERVED, // symbols
  TK_NUM, // numbers
  TK_EOF, // end od input
} TokenKind;

typedef struct Token Token;

// data structure of the token type
struct Token {
  TokenKind kind; // kind of the token
  Token *next; // next token
  int val; // value of the token when TokenKind is TK_NUM
  char *str; // string representation
}; 

// current focussed token
Token *token;

char *user_input;

//reports error duration
void error_at(char *loc, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  int pos = loc - user_input;
  fprintf(stderr,"%s\n", user_input);
  fprintf(stderr, "%*s", pos, " ");
  fprintf(stderr, "^");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

// error handling function
// takes same argument as printf()
void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
} 

// when next token matched expected symbol consume 1 token then return true
// or return false
bool consume(char op) {
  if (token->kind != TK_RESERVED || token->str[0] != op)
    return false;
  token = token->next;
  return true;
}


// when next token matched expected symbol consume 1 token
// or throw error
void expect(char op) {
  if (token->kind != TK_RESERVED || token->str[0] != op)
    error_at(token->str, "not an expected symbol:'%c'", op);
  token = token->next;
}


// when next token is a number consume 1 token and return it
// or return error
int expect_number() {
  if (token->kind != TK_NUM)
    error_at(token->str, "not a number");
  int val = token->val;
  token = token->next;
  return val;
}

bool at_eof() {
  return token->kind == TK_EOF;
}

//  generates new token after current token
Token *new_token(TokenKind kind, Token *cur, char *str) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  cur->next = tok;
  return tok;
}

// tokenize input then return it
Token *tokenize(char *p) {
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while (*p) {
    // skip spaces
    if (isspace(*p)) {
      p++;
      continue;
    }

    if (*p == '+' || *p == '-') {
      cur = new_token(TK_RESERVED, cur, p++);
      continue;
    }

    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p);
      cur->val = strtol(p, &p, 10);
      continue;
    }

    error("cannot tokenize input");

  }

  new_token(TK_EOF, cur, p);
  return head.next;
}

int main(int argc, char **argv) {
  if (argc =! 2) {
    "invalid quantity of arguments";
    return 1;
  }

  // tokenize
  token = tokenize(argv[1]);
  *user_input = &token; 

  // preparation
  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("main:\n");

  // first token must a number so this is a special case
  printf("  mov rax, %d\n", expect_number());

  // output tokens
  while (!at_eof()) {

    if (consume('+')) {
      printf("  add rax, %d\n", expect_number());
      continue;
    }

    expect('-');
    printf("  sub rax, %d\n", expect_number());
  }

  printf("  ret\n");
  return 0;
}



