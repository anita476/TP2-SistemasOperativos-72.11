;taken ad verbatim from class 

GLOBAL acquire
GLOBAL release  

; reference void acquire(uint8_t * lock) param is rdi
acquire:
        mov al, 0 ; uint8 -> try set sem to 0 so no one is in critical zone
.retry:
        xchg [rdi], al
        test al, al
        jz   .retry
        ret 

; reference void release(uint8_t * lock) param is rdi
release:
        mov byte [rdi], 1 
        ret 
