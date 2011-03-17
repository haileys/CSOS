use16
org 0x9000

; LBA_LO = 0x2000
; LBA_HI = 0x2004
; DISK = 0x2006
; AH = 0x2007 (0x42 for read, 0x43 for write)

; lba extensions

mov si, 0x2000
mov dl, [0x2010]
mov ah, [0x2011]

int 0x13

jc error

mov al, 0
mov [0x2000], al
ret

error:
	mov al, 1
	mov [0x2000], al
	ret