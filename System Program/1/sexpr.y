%{
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct cell {
  struct cell* car;
  struct cell* cdr;
  char* v;
} Cell;

Cell ptr_atom_a = { NULL, NULL, "A" };
Cell ptr_atom_b = { NULL, NULL, "B" };
Cell ptr_nil = { NULL, NULL, "NIL" };

Cell* cons(Cell* car, Cell* cdr){
  Cell* c = (Cell*)malloc(sizeof(Cell));
  c->car = car;
  c->cdr = cdr;
  c->v = NULL;
  return c;
}

void prin1(Cell* c){
  if(c->car == NULL){
    printf("%s", c->v);
  }else{
    printf("(");
    prin1(c->car);
    for(; c->cdr->car != NULL; c = c->cdr){
      printf(" ");
      prin1(c->cdr->car);
    }
    if(c->cdr != &ptr_nil){
      printf(" . ");
      prin1(c->cdr);
    }
    printf(")");
  }
}

Cell* replace_last_cdr(Cell* l, Cell* a){
  Cell* ll;
  for(ll = l; ll != NULL && ll->cdr != &ptr_nil; ll = ll->cdr);
  ll->cdr = a;
  return l;
}

int yylex();
int yyerror();

%}

%union{
    Cell* p;
}

%type <p> expr
%type <p> list
%type <p> atom

%%
statement: expr { prin1($1); };

expr  : atom { $$ = $1; }
      | '(' ')' { $$ = &ptr_nil; }
      | '(' list ')' { $$ = $2; }
      | '(' list '.' expr ')' { $$ = replace_last_cdr($2, $4); }
      ;

list  : expr { $$ = cons($1, &ptr_nil); }
      | list expr { $$ = replace_last_cdr($1, cons($2, &ptr_nil)); }
      ;

atom  : 'a' {$$ = &ptr_atom_a; }
      | 'b' {$$ = &ptr_atom_b; }
      ;

%%
int yylex(){
  int ch;
  do{
    if((ch = getchar()) == '\n' || ch == EOF){
      return 0;
    }
  }while(isspace(ch));
  return ch;
}

int yyerror(){
  fprintf(stderr, "error");
  return 0;
}

int main(int ac, char** av){
  yyparse();
  printf("\n");
  return 0;
}
