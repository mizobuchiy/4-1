/* GC用グローバル変数宣言 */
/* スタックポインタの値を参照するための処理 */

void* env_init;
void* env_cur;
void** GET_STACK_PTR(void* x) {
  void** p = (void**)&x;
  return p;
}

Ptr gc_pointer; /* アトム「gc」を保存する変数. */

void** sp_init; /* スタックの初期値 */
void** sp_cur;  /* スタックの現在値 */
Ptr free_list;  /* フリーリストの先頭要素を指すポインタ */
Ptr heap_area;  /* セルとして使用する領域全体(配列) */
int max_cell_no = 2000; /*int max_cell_no=200000; */
/* GC用下請関数 */
int in_heap_area(void* p) { /* ヒープ領域に存在しうるポインタ値かどうか確認 */
  void* s = heap_area;
  void* e = heap_area + max_cell_no;
  if (s <= p && p < e)
    return 1;
  else
    return 0;
}

Ptr new_ptr() /* 一つのセルのための領域を確保.    */
{
  Ptr p; /* フリーリストに空きがあれば, それを返す.  */
  extern int gc(); /* 無ければ, gcを実行. */
  extern Ptr free_list;
  if (free_list != NULL && free_list != nil_pointer) {
    p = free_list;
    free_list = free_list->atr.cell.cdr;
    return p;
  }
  if (gc() > 0) return new_ptr();
  fprintf(stderr, "no more memory\n");
  exit(1);
}
void init_heap() /* ヒープ領域の確保と初期化 */
{
  int i;
  heap_area = (Ptr)malloc(sizeof(struct ptr) * max_cell_no);
  for (i = 0; i < max_cell_no - 1; i++) {
    heap_area[i].tag = FREE;
    heap_area[i].atr.cell.car = heap_area + i + 1;
    heap_area[i].atr.cell.cdr = heap_area + i + 1;
  }
  heap_area[max_cell_no - 1].tag = FREE;
  heap_area[max_cell_no - 1].atr.cell.car = NULL;
  heap_area[max_cell_no - 1].atr.cell.cdr = NULL;
  free_list = heap_area;
}

int gc_init() { /* GCのための初期化. 起動時のスタックを保存 */
  sp_init = GET_STACK_PTR(env_init);
  return 0;
}

int gc() { /* GCのプログラム入り口 */
  extern int gc_sys();
  sp_cur = GET_STACK_PTR(env_init);
  return gc_sys();
}

void dfs_mark(Ptr p) { /* 深さ優先でマーク */
  if (p->tag & MARK_BIT) return;
  while (p->tag == LIST && (p->tag & MARK_BIT) != MARK_BIT) {
    p->tag = p->tag | MARK_BIT;
    dfs_mark(p->atr.cell.car);
    p = p->atr.cell.cdr;
  }
  p->tag = p->tag | MARK_BIT;
}

int gc_sys() { /* GCの本体  */
  extern void dfs_mar(Ptr);
  int cnt;
  /* clear mark */
  {
    int i;
    for (i = 0; i < max_cell_no; i++) {
      heap_area[i].tag = heap_area[i].tag & (~MARK_BIT);
    }
  }
  /* mark from stack : スタックから辿れるものをマーク */
  {
    int n;
    void** tmp;
    n = sp_init - sp_cur;
    tmp = sp_cur;
    while (n-- >= 0) {
      if (in_heap_area(*tmp)) dfs_mark(*tmp);
      tmp++;
    }
  }
  /* mark from env : 環境から辿れるものをマーク */
  dfs_mark(oblist);
  dfs_mark(lisp_env);
  /* collect cells : マークされていないものを集めてフリーリストに追加 */
  {
    int i;
    free_list = NULL;
    cnt = 0;
    for (i = max_cell_no - 1; i >= 0; i--) {
      if (!(heap_area[i].tag & MARK_BIT)) {
        cnt++;
        heap_area[i].tag = -1;
        heap_area[i].atr.cell.car = nil_pointer;
        heap_area[i].atr.cell.cdr = free_list;
        free_list = heap_area + i;
      }
    }
    fprintf(stderr, " collected cells: %d in %d\n", cnt, max_cell_no);
  }
  /* clear mark : マークを消去 */
  {
    int i;
    for (i = 0; i < max_cell_no; i++) heap_area[i].tag &= ~MARK_BIT;
  }
  return cnt;
}
