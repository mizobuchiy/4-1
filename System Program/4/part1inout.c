#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { LIST = 0, SYMBOL = 1 };
int dbg = 0;

typedef struct ptr {
  int tag;
  union {
    struct { /* リストのための構造体 */
      struct ptr* car;
      struct ptr* cdr;
    } cell;
    struct { /* シンボル(アトム)のための構造体 */
      char* pname;
      struct ptr* plist;
    } symbol;
  } atr;
} * Ptr;

Ptr oblist = NULL; /* 全てのアトムからなるリスト */
Ptr true_pointer;  /* シンボル T を指す */
Ptr nil_pointer;   /* nil を指す        */

/* 下請関数 */
char* pname(Ptr x) { return x->atr.symbol.pname; }

Ptr symbolp(Ptr x) { return (x->tag == SYMBOL) ? true_pointer : nil_pointer; }

Ptr car(Ptr x) { return x->atr.cell.car; }
Ptr cdr(Ptr x) { return x->atr.cell.cdr; }

/*** プリント関数 print_expr ****/
Ptr print_expr(Ptr x) {
  if (symbolp(x) == true_pointer) {
    printf("%s", pname(x));
  } else {
    printf("(");
    print_expr(car(x));
    for (x = cdr(x); symbolp(x) == nil_pointer; x = cdr(x)) {
      printf(" ");
      print_expr(car(x));
    }
    if (x == nil_pointer)
      printf(")");
    else
      printf(" . %s)", pname(x));
  }
  return nil_pointer;
}

/* 読み込み用下請 */
Ptr new_ptr() {
  Ptr p = malloc(sizeof(struct ptr));
  if (p == NULL) exit(1); /* メモリが尽きたら終了 */
  return p;
}

Ptr new_symbol(char* pname) { /* アトムのための領域確保 */
  Ptr x = new_ptr();
  x->tag = SYMBOL;
  x->atr.symbol.pname = strdup(pname);
  x->atr.symbol.plist = nil_pointer;
  return x;
}

Ptr cons(Ptr x, Ptr y) { /* リストのためのセル領域確保 */
  Ptr z = new_ptr();
  z->tag = LIST;
  z->atr.cell.car = x;
  z->atr.cell.cdr = y;
  return z;
}

/* 全てのアトムはシステム内でユニーク！ */
Ptr intern(char* s) { /* 全てのアトムをリストにして登録 */
  Ptr p, id;
  if (dbg) printf("intern: %s\n", s);
  for (p = oblist; p != NULL && p != nil_pointer; p = cdr(p)) {
    if (strcmp(s, pname(car(p))) == 0) return car(p);
  }
  oblist = cons(id = new_symbol(s), oblist);
  return id;
}

/*** シンボル読み込み ***/
#define MaxLength 50
static char buf[MaxLength + 2];
enum { AtomName = 0x101 };

int isLispLetter(int ch) { /* 識別子に用いることのできる文字を指定 */
  return isgraph(ch) && ch != '(' && ch != ')' && ch != '.';
}

int nextchar;
int flag = 0;

int mygetchar() {
  int ch;
  if (flag) {
    flag = 0;
    return nextchar;
  } else {
    ch = getchar();
    return ch;
  }
}

int peekchar() {
  if (flag) {
    return nextchar;
  } else {
    nextchar = mygetchar();
    flag = 1;
    return nextchar;
  }
}

int next_token() { /* 1文字読む. アトムのときは名前を読む. */
  int ch, i = 0;
  while (isspace(ch = mygetchar()))
    ; /* スペースは読み飛ばす */
  if (ch == EOF) {
    printf("exit\n");
    exit(0);
  }
  if (isLispLetter(ch)) {
    for (buf[i++] = ch; isLispLetter(ch = peekchar()) && i < MaxLength;
         buf[i++] = mygetchar())
      ;
    buf[i] = 0;
    return AtomName;
  }
  return ch;
}

/*** 読み込み read_expr_list ***/
Ptr read_expr_list(int n) {
  Ptr read_expr_body(int), read_expr();
  int tk = next_token();
  if (dbg) printf("read_expr_list: tk=%c\n", tk);
  if (tk == ')') {
    return nil_pointer;
  }
  if (tk == '.') {
    if (n == 0) {
      fprintf(stderr, "Format error(1)\n");
      exit(1);
    }
    Ptr p = read_expr();
    if (next_token() != ')') {
      fprintf(stderr, "Format error(2)\n");
      exit(1);
    }
    return p;
  }
  Ptr p = read_expr_body(tk);
  return cons(p, read_expr_list(n + 1));
}

/*** 読み込み read_expr ***/
Ptr read_expr_body(int tk) {
  if (tk == AtomName)
    return intern(buf);
  else if (tk == '(')
    return read_expr_list(0);
  else {
    fprintf(stderr, "Format error(3): tk=%d\n", tk);
    exit(1);
  }
  return nil_pointer;
}

Ptr read_expr() { /* 読み込み関数エントリ */
  return read_expr_body(next_token());
}

/* メイン関数 */
int main() {
  Ptr x, y, z, w;
  nil_pointer = intern("nil");
  nil_pointer->atr.symbol.plist = nil_pointer;
  oblist->atr.cell.cdr = nil_pointer;
  true_pointer = intern("t");
  printf("%p %p\n", nil_pointer, true_pointer);

  x = intern("x");
  y = intern("y");
  z = cons(y, nil_pointer);
  w = cons(x, y);
  z = cons(w, z);
  z = cons(x, z);
  print_expr(z);
  printf("\n\n");

  x = read_expr();
  print_expr(x);
  printf("\n");
  return 0;
}
