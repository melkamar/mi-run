;
; bytecode compiler
;

(define constantTable '())
(define theArgList '())
(define collectedCode '())

(define ADD     10)
(define SUB     11)
(define MUL     12)
(define DIV     13)

(define RET_TOP 20)
(define DROP    21)

(define PUSH_CONST    30)
(define PUSH_ARG      31)
(define PUSH_LOCAL    32)

;
; if already known, return its index
; if not, add and return the (new) index
;
(define (encode-symbolic-op op)
    (eval op))

(define (emit-bytcode aByte)
    (set! collectedCode
     (append collectedCode (list1 aByte))))

(define (emit0 symbolicBytecode)
    (display symbolicBytecode) (display "\n")
    (emit-bytcode (encode-symbolic-op symbolicBytecode)))

(define (emit1 symbolicBytecode arg)
    (display symbolicBytecode) (display " ") (display arg) (display "\n")
    (emit-bytcode (encode-symbolic-op symbolicBytecode))
    (emit-bytcode arg))

(define (not-in-list v l)
  (not (member? v l)))

(define (list1 v)
    (cons v nil))

(define (add-to-globals v)
  (set! constantTable (append constantTable (list1 v)))
  (- (length constantTable) 1))

(define (index-in-list v l)
  (define (index-in-list-helper v l idxIn)
  (if (eq? l '())
      -1
      (if (eq? (car l) v)
	  idxIn
	  (index-in-list-helper v (cdr l) (+ idxIn 1)))))
  (index-in-list-helper v l 0))

(define (add-constant const)
    (if (not-in-list const constantTable)
	(add-to-globals const)
	(index-in-list const constantTable)))

(define (compile-expression expr)
    (if (symbol? expr)
	(compile-symbol expr)
    (if (cons? expr)
	(compile-call expr)
    (compile-constant expr))))

(define (compile-constant const)
    (emit1 'PUSH_CONST (add-constant const)))

(define (isLocal? var) #f)
(define (isArg? var) #t)
(define (index-of-local var) -1)
(define (index-of-arg var) 0)

(define (compile-symbol const)
    (if (isLocal? const)
	(emit1 'PUSH_LOCAL (index-of-local const))

    (if (isArg? const)
	(emit1 'PUSH_ARG (index-of-arg const))
    (emit1 'PUSH_GLOBAL const))))

(define (compile-call expr)
    (compile-call-function (car expr) (cdr expr)))

(define (compile-call-function fn argList)
    (compile-arg-list argList)
    (if (eq? fn '+)
	(emit0 'ADD)
    (if (eq? fn '*)
	(emit0 'MUL)
    (emit0 'CALL))))

(define (isNull? e) (eq? e '()))

(define (compile-arg-list list)
    (if (isNull? list)
	'()
    (compile-next-arg-in-list (car list) (cdr list))))

(define (compile-next-arg-in-list thisArg moreArgs)
    (compile-expression thisArg)
    (compile-arg-list moreArgs))

; (compile-expression '(+ 10 20))
;(compile-expression '(+ (* 10 20) (* 30 40)))
;(compile-expression 'foo)
; (compile-expression '(+ (* foo 20) (* 30 foo)))
; fine (isNull? e) (eq? e '()))

(define (compile-arg-list list)
    (if (isNull? list)
	'()
    (compile-next-arg-in-list (car list) (cdr list))))

(define (compile-next-arg-in-list thisArg moreArgs)
    (compile-expression thisArg)
    (compile-arg-list moreArgs))

(define (compile-expression-and-continue expr restBodyList)
    (compile-expression expr)
    (emit0 'DROP)
    (compile-body restBodyList))

(define (compile-body bodyList)
    (if (eq? (cdr bodyList) '())
	(compile-expression (car bodyList))
    ; else
	(compile-expression-and-continue (car bodyList) (cdr bodyList))))

(define (compile-function argList bodyList)
    (set! theArgList argList)
    (set! collectedCode '())
    (compile-body bodyList)
    (emit0 'RET_TOP))

(define (c-fn fn)
    (compile-function (get-function-args fn) (get-function-body fn))
    (set-bytecode fn collectedCode constantTable))
