org 100h

MOV AX,5301H
XOR BX,BX
INT 15H
MOV AX,530EH
XOR BX,BX
MOV CX,0102H
INT 15H
MOV AX,5307H	;同上
MOV BX,0001H
MOV CX,0003H
INT 15H
