#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { LIST = 0, SYMBOL = 1 };
int dbg = 0;

typedef struct ptr {
  int tag;
  union {
    /* リストのための構造体 */
    struct {
      struct ptr* car;
      struct ptr* cdr;
    } cell;
    /* シンボル(アトム)のための構造体 */
    struct {
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
  extern Ptr quote_pointer;
  if (symbolp(x) == true_pointer) {
    printf("%s", pname(x));
  } else if (car(x) == quote_pointer) {
    printf("'");
    print_expr(car(cdr(x)));
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
  /* メモリが尽きたら終了 */
  if (p == NULL) exit(1);
  return p;
}

/* アトムのための領域確保 */
Ptr new_symbol(char* pname) {
  Ptr x = new_ptr();
  x->tag = SYMBOL;
  x->atr.symbol.pname = strdup(pname);
  x->atr.symbol.plist = nil_pointer;
  return x;
}

/* リストのためのセル領域確保 */
Ptr cons(Ptr x, Ptr y) {
  Ptr z = new_ptr();
  z->tag = LIST;
  z->atr.cell.car = x;
  z->atr.cell.cdr = y;
  return z;
}

/* 全てのアトムはシステム内でユニーク！ */
/* 全てのアトムをリストにして登録 */
Ptr intern(char* s) {
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

/* 識別子に用いることのできる文字を指定 */
int isLispLetter(int ch) {
  return isgraph(ch) && ch != '(' && ch != ')' && ch != '.' && ch != '\'';
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

/* 1文字読む. アトムのときは名前を読む. */
int next_token() {
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
  if (tk == AtomName) {
    return intern(buf);
  } else if (tk == '\'') {
    extern Ptr quote_pointer, read_expr();
    return cons(quote_pointer, cons(read_expr(), nil_pointer));
  } else if (tk == '(') {
    return read_expr_list(0);
  } else {
    fprintf(stderr, "Format error(3): tk=%d\n", tk);
    exit(1);
  }
  return nil_pointer;
}

/* 読み込み関数エントリ */
Ptr read_expr() { return read_expr_body(next_token()); }

/* 関数のプロトタイプ宣言と大域ポインタ類 */
Ptr p_eq(Ptr a1, Ptr a2);
Ptr p_apply(Ptr fn, Ptr args, Ptr a);
Ptr p_eval(Ptr e, Ptr a);
Ptr p_evcon(Ptr c, Ptr a);
Ptr p_evlis(Ptr m, Ptr a);
Ptr p_assoc(Ptr x, Ptr a);
Ptr p_pairlis(Ptr v, Ptr e, Ptr a);
/* int gc(); */
Ptr car_pointer;
Ptr cdr_pointer;
Ptr cons_pointer;
Ptr atom_pointer;
Ptr eq_pointer;
Ptr lambda_pointer;
Ptr label_pointer;
Ptr quote_pointer;
Ptr cond_pointer;
Ptr set_pointer;
Ptr gc_pointer;
Ptr lisp_env;

/* ポインタの初期化 */
void init_lisp() {
  car_pointer = intern("car");
  cdr_pointer = intern("cdr");
  cons_pointer = intern("cons");
  atom_pointer = intern("atom");
  eq_pointer = intern("eq");
  lambda_pointer = intern("lambda");
  label_pointer = intern("label");
  quote_pointer = intern("quote");
  cond_pointer = intern("cond");
  /* set_pointer=intern("set"); */
  /* gc_pointer=intern("gc"); */
}

/* eq */
Ptr p_eq(Ptr a1, Ptr a2) {
  if (a1 == a2) return true_pointer;
  return nil_pointer;
}

/* apply */
Ptr p_apply(Ptr fn, Ptr args, Ptr a) {
  if (symbolp(fn) == true_pointer) {
    if (fn == car_pointer) return car(car(args));
    if (fn == cdr_pointer) return cdr(car(args));
    if (fn == cons_pointer) return cons(car(args), car(cdr(args)));
    if (fn == atom_pointer) return symbolp(car(args));
    if (fn == eq_pointer) return p_eq(car(args), car(cdr(args)));
    /* if( fn==set_pointer) {
         lisp_env=cons(cons(car(args),car(cdr(args))),lisp_env);
         return nil_pointer; }
       if( fn==gc_pointer) { gc(); return nil_pointer; } */
    if ((fn = p_eval(fn, a)) != nil_pointer) return p_apply(fn, args, a);
    return fn;
  }
  if (car(fn) == lambda_pointer)
    return p_eval(car(cdr(cdr(fn))), p_pairlis(car(cdr(fn)), args, a));
  if (car(fn) == label_pointer)
    return p_apply(car(cdr(cdr(fn))), args,
                   cons(cons(car(cdr(fn)), car(cdr(cdr(fn)))), a));
  printf("unknown functions: ");
  print_expr(fn);
  printf("\n");
  return nil_pointer;
}

/* eval */
Ptr p_eval(Ptr e, Ptr a) {
  Ptr v;
  if (symbolp(e) == true_pointer) {
    if (e == true_pointer) return true_pointer;
    if (e == nil_pointer) return nil_pointer;
    v = p_assoc(e, a);
    if (v != nil_pointer) return cdr(v);
    printf("unbound variable: ");
    print_expr(e);
    printf("\n");
    return nil_pointer;
  }
  if (car(e) == quote_pointer) return car(cdr(e));
  if (car(e) == cond_pointer) return p_evcon(cdr(e), a);
  return p_apply(car(e), p_evlis(cdr(e), a), a);
}

/* evcon */
Ptr p_evcon(Ptr c, Ptr a) {
  if (c == nil_pointer) return nil_pointer;
  if (p_eval(car(car(c)), a) != nil_pointer) return p_eval(car(cdr(car(c))), a);
  return p_evcon(cdr(c), a);
}

/* evlis */
Ptr p_evlis(Ptr m, Ptr a) {
  Ptr p;
  if (m == nil_pointer) return nil_pointer;
  p = p_eval(car(m), a);
  return cons(p, p_evlis(cdr(m), a));
}

/* assoc */
Ptr p_assoc(Ptr x, Ptr a) {
  if (a == nil_pointer) return nil_pointer;
  if (x == car(car(a))) return car(a);
  return p_assoc(x, cdr(a));
}

/* pairlis */
Ptr p_pairlis(Ptr v, Ptr e, Ptr a) {
  Ptr p;
  if (v == nil_pointer) return a;
  p = cons(car(v), car(e));
  return cons(p, p_pairlis(cdr(v), cdr(e), a));
}

/* メイン関数 */
int main() {
  /* void init_heap(); */
  /* int gc_init(); */
  /* init_heap(); */
  /* gc_init(); */
  nil_pointer = intern("nil");
  nil_pointer->atr.symbol.plist = nil_pointer;
  oblist->atr.cell.cdr = nil_pointer;
  true_pointer = intern("t");
  printf("%p %p\n", nil_pointer, true_pointer);

  init_lisp();

  lisp_env = cons(cons(true_pointer, true_pointer),
                  cons(cons(nil_pointer, nil_pointer), nil_pointer));
  while (1) {
    Ptr x, y;
    printf("\nlisp> ");
    x = read_expr();
    y = p_eval(x, lisp_env);
    print_expr(y);
  }
}
