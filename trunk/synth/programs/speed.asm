// Fibonacci sequence generator
// Tests direct and indirect memory, adding, and branching.

// Interrupt return address
org 000
ira,	hex lop

// Interrupt vector
org 001
		// Just return to normal program operation.
		bun ira i

// Program start
org 030
		// Add the two numbers and halt on carry
lop,	lda pn1 i
		add pn2 i
		sze
		bun fin

		// Store to a temporary variable
		sta tmp
		
		// Increment pointers
		cla
		inc
		add pn1
		sta pn1
		cla
		inc
		add pn2
		sta pn2

		// Store sum in second pointer and loop
		lda tmp
		sta pn2 i
		bun lop

fin,	hlt

tmp,	hex 0
pn1,	hex pi1
pn2,	hex pi2
pi1,	hex 0
pi2,	hex 1
		end