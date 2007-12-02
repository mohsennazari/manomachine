//////////////////////////////////////////////////////////////////////////////
// Mano machine project
// (c) Greg Toombs 2007
// 
// booth.hex
// 
// The Booth multiplier sample program.
// Tests most operations, and synchronous i/o.

// Multiplies two signed seven-bit numbers, since our word size is 16.
// 1 + 7*2 = 15

// The stored interrupt return address.
org 000
dat_ira,    bun cod_begin

// The interrupt request vector.
org 001
            bun dat_ira i
        
// The program start.

// Tell the test fixture to give us data.
cod_begin,  lda dat_insig
            out
cod_out,	sko
            bun cod_out

// Read two 7-bit signed integers from the character bus.

// Get the first input.
cod_in1,    ski
            bun cod_in1
            inp
            sta dat_ba
            
// Get the second input.
cod_in2,    ski
            bun cod_in2
            inp
            sta dat_bp


// Negate (two's-complement) and mask bs.
            lda dat_ba
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
cod_lop,    and dat_cmpmsk
            cir
            sze
            bun cod_bx1
cod_bx0,    sza
            bun cod_b10
cod_b00,    lda dat_bp
            bun cod_cont
cod_b10,    lda dat_bp
            add dat_bs
            bun cod_cont
cod_bx1,    sza
            bun cod_b11
cod_b01,    lda dat_bp
            add dat_ba
            bun cod_cont
cod_b11,    lda dat_bp

// Perform an (awful) 15-bit arithmetic shift-right.
cod_cont,   cle
            and dat_varmsk
            cil
            cil
            sze
            bun cod_dup
cod_nodup,  cir
            bun cod_shift
cod_dup,    cir
            cme
cod_shift,  cir
            cir

            sta dat_bp
            cle

            isz dat_lop
            bun cod_lop

// We're done multiplying. Store the result.
            cir
            sta dat_bp
            
// Output the result.
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
            
            hlt

// Program variables:

// Three basic Booth variables
dat_ba,     dec 0    // "Add"
dat_bs,     dec 0    // "Subtract"
dat_bp,     dec 0    // "Product"

dat_lop,    dec -7   // main loop counter

dat_cmpmsk, hex 0003 // The comparison mask
dat_argmsk, hex 007F // The argument mask
dat_varmsk, hex 7FFF // The working variable mask

dat_insig,  hex 0005 // ASCII "Enquiry" code for input signal
        
            end
