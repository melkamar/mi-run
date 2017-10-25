;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Copyright 2017 Claus Gittinger
;;
;; You may use this, but not claim to have written or own it!
;; Use at your own risk.
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;
; objects implemented using closures
;

(define (make-rectangle x y w h)

    (define (get-x)
	x)

    (define (get-y)
	y)

    (define (set-x newX)
	(set! x newX))

    (define (set-y newY)
	(set! y newY))

    (define (get-width)
	w)

    (define (get-height)
	h)

    (define (area)
	(* w h))

    (lambda (operation)
	(if (eq? operation 'x) get-x
	(if (eq? operation 'y) get-y
	(if (eq? operation 'setX) set-x
	(if (eq? operation 'setY) set-y
	(if (eq? operation 'width) get-width
	(if (eq? operation 'height) get-height
	(if (eq? operation 'area) area
	    (error "no such virtual function"))))))))))

(define (make-circle x y r)

    (define (get-x)
	x)

    (define (get-y)
	y)

    (define (set-x newX)
	(set! x newX))

    (define (set-y newY)
	(set! y newY))

    (define (area)
	(* r 2 3))  ; actually: r*2 * pi

    (lambda (operation)
	(if (eq? operation 'x) get-x
	(if (eq? operation 'y) get-y
	(if (eq? operation 'setX) set-x
	(if (eq? operation 'setY) set-y
	(if (eq? operation 'area) area
	    (error "no such virtual function"))))))))

;; notice:
;;      (obj 'x)
;; gets use the get-x functions (the closure).
;; and
;;      ((obj 'x) )
;; calls it (without argument).

(define (get-x obj)
    ((obj 'x) ))

(define (get-y obj)
    ((obj 'y) ))

(define (set-x! obj newX)
    ((obj 'setX) newX ))

(define (set-y! obj newX)
    ((obj 'setY) newY ))

(define (get-width obj)
    ((obj 'width) ))

(define (get-height obj)
    ((obj 'height) ))

(define (area obj)
    ((obj 'area) ))

;
; demo
;
(define r1 (make-rectangle 100 200 50 150))
(define r2 (make-rectangle 0 0 50 150))
(define c1 (make-circle 15 25 120))

(display "x of rectangle1 is: ") (display (get-x r1)) (display "\n")
(display "x of circle1 is: ") (display (get-x c1)) (display "\n")

(display "area of rectangle1 is: ") (display (area r1)) (display "\n")
(display "area of circle1 is: ") (display (area c1)) (display "\n")
