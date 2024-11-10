; Taken ad verbatim from class 

GLOBAL acquire
GLOBAL release  

acquire:
        mov al, 0 ; uint8 -> Setting sem to 0 so no one is in critical zone
.retry:
        xchg [rdi], al
        test al, al
        jz   .retry
        ret 

release:
        mov byte [rdi], 1 
        ret 
