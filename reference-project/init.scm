; this is a comment line

(define (testFunc1) (display "hello world"))

(define nil ())
(define null ())
(define true #t)
(define false #f)
(define #void (display ""))

(define (> a b) (< b a))
(define (not aBool) (if aBool #f #t))

(define (fac n)
    (if (< n 2)
	1
    ; else
	(* n (fac (- n 1)))))

(define (fac-iterative n)
    (define (fac-helper n productIn)
	(if (< n 2)
	    productIn
	; else
	    (fac-helper (- n 1) (* n productIn))))

    (fac-helper n 1))

;
; if executed with a recursive interpreter,
; this will overflow the stack when called with a big n
; as in (count-down 10000000)
;
(define (count-down n)
    (if (= n 0)
	0
	(count-down (- n 1))))


(define (REPL)
    (display ">> ")
    (print (eval (read)))
    (display "\n"))
