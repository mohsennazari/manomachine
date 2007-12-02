//////////////////////////////////////////////////////////////////////////////
// Mano machine project
// (c) Greg Toombs 2007
// 
// sim.v
// 
// Contains the simulation code.

// The simulation timescale.
`timescale 1ns / 1ps

// The speed benchmark module.
module sim_speed();

// Instantiate glbl for the XST primitives to use.
// glbl glbl();

// Timing parameters (in ns).

// Assumed parameters:
parameter par_tclock0   = 8;   // The initial clock period.
parameter par_treset    = 100; // The time to wait for reset to finish.

// Timing variables.
realtime tclock;


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
    

    // Gradually increase the frequency to see when the processor breaks.
    for (tclock = par_tclock0; tclock >= 0; tclock = tclock - 0.01)
    begin
        reg_clock = ~reg_clock;
        #(tclock/2);
        reg_clock = ~reg_clock;
        #(tclock/2);
    end

end

endmodule
