;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Copyright 2017 Claus Gittinger
;;
;; You may use this, but not claim to have written or own it!
;; Use at your own risk.
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define (testFunc1) (display "hello world"))

(define nil ())
(define null ())
(define true #t)
(define false #f)
(define #void (display ""))

(define (not aBool) (if aBool #f #t))
(define (> a b) (< b a))
(define (<= a b) (not (< b a)))
(define (>= a b) (not (< a b)))

;
; a recursive structural equal
;
(define (equal? a b)
    (if (eq? a b)
	#t
    ; else
	(if (cons? a)
	    (if (cons? b)
		(if (equal? (car a) (car b))
		    (equal? (cdr a) (cdr b))
		; else
		    #f
		)
	    ; else
		#f
	    )
	; else
	    (if (string? a)
		(if (string? b)
		    (string-equal? a b)
		; else
		    #f
		)
	    ; else
		(if (number? a)
		    (if (number? b)
			(= a b)
		    ; else
			#f
		    )
		; else
		    #f
		)
	    )
	)
    )
)

(define (member? o l)
    (if (equal? l '())
	#f
    (if (equal? (car l) o)
	#t
    (member? o (cdr l)))))

(define (memq? o l)
    (if (eq? l '())
	#f
    (if (eq? (car l) o)
	#t
    (memq? o (cdr l)))))

(define (length l)
    (if (eq? l '())
	0
    (+ 1 (length (cdr l)))))

;
; (append '(1 2 3) '(4 5 6)) -> '(1 2 3 4 5 6)
;
(define (append a b)
    (if (eq? a '())
	b
	(cons (car a) (append (cdr a) b))))

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
