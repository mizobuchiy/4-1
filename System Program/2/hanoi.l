(defun hanoi (x y z l)
  (cond ((atom (cdr l)) (list (list 'move x y (car l))))
    (t (append
      (hanoi x z y (cdr l))
      (cons (list 'move x y (car l)) (hanoi z y x (cdr l))))
    )
  )
)

(defun test()
  (hanoi 'a 'b 'c '(1 2 3))
)
