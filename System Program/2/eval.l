(defun p_eval(e a)
; (print (list 'p_eval e a))
  (cond
    ((atom e) (cdr (p_assoc e a)))
    ((eq (car e) 'quote) (cadr e))
    ((eq (car e) 'cond) (p_evcon (cdr e) a))
    (t (p_apply (car e) (p_evlis (cdr e) a) a))
  )
)
