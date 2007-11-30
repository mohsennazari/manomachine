//////////////////////////////////////////////////////////////////////////////
// Mano machine project
// (c) Greg Toombs 2007
// 
// booth.hex
// 
// The Booth multiplier sample program.

// Multiplies two seven-bit numbers, since our word size is 16.
// 1 + 7*2 = 15

// The stored interrupt return address.
org 000
ira,	bun beg

// The interrupt request vector.
org 001
		bun ira i
		
// The program start.

// Negate and mask bs.
beg,	lda ba
		cma
		inc
		and amk
		
// Shift bs into place.
sss,	cil
		isz sct
		bun sss
		
		sta bs
		
// Mask ba.
		lda ba
		and amk
		
// Shift ba into place.
ssa,	cil
		isz act
		bun ssa

		sta ba

// Mask and shift bp.
		lda bp
		and amk
		cil
		sta bp

// Main loop.

// Compare the last two bits.
lop,	and cmk
		cir
		sze
		bun bx1
		
		// Bit 0 == 0:
bx0,	cir
		sze
		bun b10
		
		// Bits 1:0 == 0:
b00,	lda bp
		bun con
		
		// Bits 1:0 == 2:
b10,	lda bp
		add bs
		bun con

		// Bit 0 == 1:
bx1,	cir
		sze
		bun b11
		
		// Bits 1:0 == 1:
b01,	lda bp
		add ba
		bun con

		// Bits 1:0 == 3:
b11,	lda bp
		bun con
		
con,	cir
		sta bp
		isz lct
		bun lop

		cir
		sta bp
		hlt

// Program variables:

// Three basic Booth variables
ba,		dec 60		// "Add"
bs,		dec 0		// "Subtract"
bp,		dec 50		// "Product"

sct,	dec -8		// bs setup loop counter
act,	dec -8		// ba setup loop counter
lct,	dec -8		// main loop counter


cmk,	hex 0003	// The comparison mask
amk,	hex 007F	// The argument mask

		
		end
