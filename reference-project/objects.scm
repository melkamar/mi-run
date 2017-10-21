; this is a comment line

(define (make-rectangle x y w h)

    (define (get-x)
	x)

    (define (get-y)
	y)

    (define (get-width)
	w)

    (define (get-height)
	h)

    (define (area)
	(* w h))

    (lambda (operation)
	(if (eq? operation 'x) get-x
	(if (eq? operation 'y) get-y
	(if (eq? operation 'width) get-width
	(if (eq? operation 'height) get-height
	(if (eq? operation 'area) area
	    (error "no such virtual function"))))))))

(define (make-circle x y r)

    (define (get-x)
	x)

    (define (get-y)
	y)

    (define (area)
	(* r 2 3))

    (lambda (operation)
	(if (eq? operation 'x) get-x
	(if (eq? operation 'y) get-y
	(if (eq? operation 'area) area
	    (error "no such virtual function"))))))

;(define (get-x obj)
;    ( (obj 'x) ))
;
;(define (get-y obj)
;    ( (obj 'y) ))
;
;(define (get-width obj)
;    ( (obj 'width) ))
;
;(define (get-height obj)
;    ( (obj 'height) ))
;
;(define (area obj)
;    ( (obj 'area) ))
;
(define r1 (make-rectangle 100 200 50 150))
(define r2 (make-rectangle 0 0 50 150))
(define c1 (make-circle 0 0 120))
;
;(get-x r1)
;(get-x c1)
