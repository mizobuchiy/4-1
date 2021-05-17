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

void init_lisp() { /* ポインタの初期化 */
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
