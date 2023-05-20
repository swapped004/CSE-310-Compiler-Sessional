.MODEL SMALL
.STACK 100H
.DATA
	print_var dw ?
	ret_temp dw ?
	t0 dw ?
	x1_2 dw ?
	t1 dw ?
	t2 dw ?
	a1_3 dw ?
	b1_3 dw ?
.CODE
print PROC
	PUSH ax
	PUSH bx
	PUSH cx
	PUSH dx
	MOV cx, 0
	XOR dx,dx
check_if_negative:
	CMP print_var,32767
	JA negative
	JMP loop_begin
negative:
	MOV ax,65535
	SUB ax, print_var
	MOV print_var,ax
	INC print_var
	MOV dx,'-'
	MOV ah,2
	INT 21H
loop_begin:
	MOV ax,print_var
	MOV dx, 0
	MOV bx,10
	DIV bx
	MOV print_var,ax
	ADD dx,'0'
	PUSH dx
	INC cx
	CMP print_var,0
	JE loop_exit
	JMP loop_begin
loop_exit:
	POP dx
	MOV ah,2
	INT 21H
	DEC cx
	CMP cx,0
	JE func_end
	JMP loop_exit
func_end:
	MOV dl,10
	MOV ah,02h
	INT 21H
	MOV dl,13
	MOV ah,02h
	INT 21H
	POP dx
	POP cx
	POP bx
	POP ax
	RET
ENDP print
f PROC
	PUSH ax
	PUSH bx
	PUSH cx
	PUSH dx
	PUSH bp
	MOV bp,sp
;return
;2*a;

	MOV ax,2
	MOV dx,[bp+12]
	MUL dx
	MOV t0,ax
	MOV ax,t0
	MOV ret_temp,ax
;a=9;

	MOV ax,9
	MOV [bp+12],ax
	POP bp
	POP dx
	POP cx
	POP bx
	POP ax
	RET 2
f endp
g PROC
	PUSH ax
	PUSH bx
	PUSH cx
	PUSH dx
	PUSH bp
	MOV bp,sp
;int
;x;

;x=f(a)+a+b;

	PUSH [bp+14]
	CALL f
	MOV ax,ret_temp
	MOV t0,ax
	MOV ax,t0
	ADD ax,[bp+14]
	MOV t1,ax
	MOV ax,t1
	ADD ax,[bp+12]
	MOV t2,ax
	MOV ax,t2
	MOV x1_2,ax
;return
;x;

	MOV ax,x1_2
	MOV ret_temp,ax
	POP bp
	POP dx
	POP cx
	POP bx
	POP ax
	RET 2
g endp
main proc
	mov ax,@data
	mov ds,ax


;int
;a,b;

;a=1;

	MOV ax,1
	MOV a1_3,ax
;b=2;

	MOV ax,2
	MOV b1_3,ax
;a=g(a,b);

	PUSH a1_3
	PUSH b1_3
	CALL g
	MOV ax,ret_temp
	MOV t0,ax
	MOV ax,t0
	MOV a1_3,ax
;printf(a);

	MOV ax,a1_3
	MOV print_var,ax
	CALL print
;return
;0;

	MOV ax,0
	MOV ret_temp,ax
	MOV AH,4CH
	INT 21H
ENDP main
END main
