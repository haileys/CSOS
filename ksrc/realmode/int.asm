use16
org 0x9000

mov al, [0x200C]
mov [interrupt + 1], al

mov ax, [0x2000]
mov bx, [0x2002]
mov cx, [0x2004]
mov dx, [0x2006]

mov si, [0x2008]
mov di, [0x200A]

interrupt: int 255

mov [0x2000], ax
mov [0x2002], bx
mov [0x2004], cx
mov [0x2006], dx

mov [0x2008], si
mov [0x200A], di

ret