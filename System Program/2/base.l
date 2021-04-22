(defun p_evcon (c a)
; (print (list 'p_evcon c a))
  (cond
    ((p_eval (caar c) a) (p_eval (cadar c) a))
    (t (p_evcon (cdr c) a))
  )
)

(defun p_evlis(m a)
; (print (list 'p_evlis m a))
  (cond
    ((null m) nil)
    (t (cons (p_eval (car m) a) (p_evlis (cdr m) a)))
  )
)


(defun p_assoc ( x a)
  (cond
    ((null a) nil)
    ((equal x (caar a)) (car a))
    (t (p_assoc x (cdr a)))
  )
)

(defun p_pairlis (v e a)
  (cond
    ((null v) a)
    (t (cons (cons (car v) (car e)) (p_pairlis (cdr v) cdr a)))
  )
)
