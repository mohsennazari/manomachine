//////////////////////////////////////////////////////////////////////////////
// Mano machine project
// (c) Greg Toombs 2007
// 
// fibb.asm
// 
// The Fibonacci series sample program.
// Calculates elements and outputs them on-the-fly.
// Tests basic instructions and synchronous i/o.

org 000
dat_ira,    bun cod_loop

// Don't do anything if there's an interrupt.
org 001
            bun dat_ira i

// Main loop.
cod_loop,   lda dat_n0
			add dat_n1
			sze
			bun cod_end
			sta dat_temp
			lda dat_n1
			sta dat_n0
			lda dat_temp
			sta dat_n1

// Output the calculated series element (16 bits).
            out
cod_out1,   sko
            bun cod_out1
            
            cir
            cir
            cir
            cir
            cir
            cir
            cir
            cir

            out
cod_out2,   sko            
            bun cod_out2

            bun cod_loop
        
cod_end,    hlt

dat_n0,     dec 0
dat_n1,     dec 1
dat_temp,   dec 0
        
            end
