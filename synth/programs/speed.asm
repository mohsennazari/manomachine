//////////////////////////////////////////////////////////////////////////////
// Mano machine project
// (c) Greg Toombs 2007
// 
// speed.asm
// 
// The Fibonacci series "speed testing" sample program.
// Stores to memory instead of using the character bus.
// Tests direct and indirect memory, adding, and branching.

// Interrupt return address
org 000
dat_ira,	bun cod_loop

// Interrupt vector
org 001
			// Just return to normal program operation.
			bun dat_ira i

// Program start
			// Add the two numbers and halt on carry
cod_loop,	lda dat_pn1 i
			add dat_pn2 i
			sze
			bun cod_end

			// Store to a temporary variable
			sta dat_temp
			
			// Increment pointers
			lda dat_pn1
			inc
			sta dat_pn1
			lda dat_pn2
			inc
			sta dat_pn2
	
			// Store sum in second pointer and loop
			lda dat_temp
			sta dat_pn2 i
			bun cod_loop

cod_end,	hlt

dat_temp,	dec 0
dat_pn1,	hex dat_pi1
dat_pn2,	hex dat_pi2
dat_pi1,	dec 0
dat_pi2,	dec 1

			end