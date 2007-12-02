//////////////////////////////////////////////////////////////////////////////
// Mano machine project
// (c) Greg Toombs 2007
// 
// sim_ppar.v
// 
// Contains the post-place-and-route simulation code.

// The simulation timescale.
`timescale 1ns / 1ps

// The simulation module.
module sim();

// Instantiate glbl for the XST primitives to use.
glbl glbl();

// Timing parameters (in ns).

// Assumed parameters:
parameter par_treset = 100;  // The time to wait for reset to finish.
parameter par_tclock = 10.0; // The clock period to use.


// Input drivers.
reg       reg_clock;        // The clock driver.
reg       reg_fgiset;       // The character input flag set line driver.
reg       reg_fgoset;       // The character output flag set line driver.
reg[7:0]  reg_inpr;         // The character input bus driver.

// Instantiate the Unit Under Test. Leave outputs as implicit hierarchical
// signals (uut.*).
main_ppar uut (
    .io_clock(reg_clock),
    .io_fgiset(reg_fgiset),
    .io_fgoset(reg_fgoset),
    .io_inpr(reg_inpr)
);


// The main simulation block.
initial
begin
    // Initial register values.
    reg_clock <= 0;
    reg_fgiset <= 0;
    reg_fgoset <= 0;
    reg_inpr <= 8'hZZ;

    // Wait for global reset to finish.
    #par_treset;
    
    // Run the clock forever.
    forever
    begin
        reg_clock = ~reg_clock;
        #(par_tclock/2);
    end
end
  
endmodule
