(defun diff(e x)
  (cond ((eq e x) 1)
        ((atom e) 0)
        ((eq (car e) '+)
          (list '+ (diff (cadr e) x) (diff (caddr e) x)))
        ((eq (car e) '-)
          (list '- (diff (cadr e) x) (diff (caddr e) x)))
        ((eq (car e) '*)
          (list '+
            (list '* (diff (cadr e) x) (caddr e))
            (list '* (cadr e) (diff (caddr e) x))
        ))
        ((eq (car e) '/)
          (list '/
            (list '-
              (diff (cadr e) x)
              (list '/
                (list '* (cadr e) (diff (caddr e) x))
                (caddr e)
              )
            )
            (caddr e)
        ))
  )
)

(defun test nil
  (print (setq res (diff '(/ x (+ (* x x) 1)) 'x)))
  (print (subst '2 'x res))
  (print (eval (subst '2 'x res)))
)
