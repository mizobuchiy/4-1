dummy: test1 test2
	echo end

rule2: rule2.y
	bison rule2.y
	gcc rule2.tab.c -o rule2

test1: rule2
	echo '1+2*3=' | ./rule2

sexpr: sexpr.y
	bison sexpr.y
	gcc sexpr.tab.c -o sexpr

test2: sexpr
	echo '(a)' | ./sexpr
	echo '(a b)' | ./sexpr
	echo '(a b a b  b b)' | ./sexpr
	echo '(a b (a (a b b) b (b b a) b . a) a .  ())' | ./sexpr
