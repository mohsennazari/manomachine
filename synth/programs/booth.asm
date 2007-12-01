//////////////////////////////////////////////////////////////////////////////
// Mano machine project
// (c) Greg Toombs 2007
// 
// booth.hex
// 
// The Booth multiplier sample program.

// Multiplies two signed seven-bit numbers, since our word size is 16.
// 1 + 7*2 = 15

// The stored interrupt return address.
org 000
dat_ira,	bun cod_begin

// The interrupt request vector.
org 001
			bun dat_ira i
		
// The program start.

// Negate (two's-complement) and mask bs.
cod_begin,	lda dat_ba
			cma
			inc
			and dat_argmsk
		
// Shift bs into place.
			cil
			cil
			cil
			cil
			cil
			cil
			cil
			cil

			sta dat_bs
		
// Mask ba.
			lda dat_ba
			and dat_argmsk
		
// Shift ba into place.
			cil
			cil
			cil
			cil
			cil
			cil
			cil
			cil

			sta dat_ba

// Mask and shift bp.
			lda dat_bp
			and dat_argmsk
			cil
			sta dat_bp

// Main loop.

// Compare the last two bits and perform the main Booth operation.
cod_lop,	and dat_cmpmsk
			cir
			sze
			bun cod_bx1
cod_bx0,	sza
			bun cod_b10
cod_b00,	lda dat_bp
			bun cod_cont
cod_b10,	lda dat_bp
			add dat_bs
			bun cod_cont
cod_bx1,	sza
			bun cod_b11
cod_b01,	lda dat_bp
			add dat_ba
			bun cod_cont
cod_b11,	lda dat_bp

// Perform an (awful) 15-bit arithmetic shift-right.
cod_cont,	cle
			and dat_varmsk
			cil
			cil
			sze
			bun cod_dup
cod_nodup,	cir
			bun cod_shift
cod_dup,	cir
			cme
cod_shift,	cir
			cir

			sta dat_bp
			cle

			isz dat_lop
			bun cod_lop

			cir
			sta dat_bp
			hlt

// Program variables:

// Three basic Booth variables
dat_ba,		dec 60		// "Add"
dat_bs,		dec 0		// "Subtract"
dat_bp,		dec 50		// "Product"

dat_lop,	dec -7		// main loop counter


dat_cmpmsk,	hex 0003	// The comparison mask
dat_argmsk,	hex 007F	// The argument mask
dat_varmsk,	hex 7FFF	// The working variable mask

		
			end
