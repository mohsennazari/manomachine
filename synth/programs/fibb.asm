//////////////////////////////////////////////////////////////////////////////
// Mano machine project
// (c) Greg Toombs 2007
// 
// fibb.asm
// 
// The Fibonacci series sample program.

org 000
ira,	bun lop

org 001
		bun ira i
		
lop,	add n00
		sze
		bun fin
		sta tmp
		lda n01
		sta n00
		lda tmp
		sta n01
		bun lop
		
fin,	hlt

n00,	hex 0
n01,	hex 1
tmp,	hex 0
		
		end
