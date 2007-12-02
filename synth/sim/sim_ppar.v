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
module sim_ppar();

// Behavioural parameters.

parameter par_input = 0; // Should we send input data (for the booth
                         // multiplier)?

// Timing parameters (in ns).

// Assumed parameters:
parameter par_treset = 100;  // The time to wait for reset to finish.
parameter par_tclock = 10;   // The clock period to use.


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

task tick;
begin
    reg_clock <= 1;
    #(par_tclock/2);
    reg_clock <= 0;
    #(par_tclock/2);
end
endtask


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
        if (!uut.io_fgo)
        begin
            // Slow output device
            repeat (10)
                tick();
            $display("Output: 8'h%h", uut.io_outr);
            reg_fgoset <= 1;
            tick();
            reg_fgoset <= 0;
            
            // If the processor output an ascii ENQ, we give it two numbers to
            // multiply:
            if (uut.io_outr == 8'h05 && par_input)
            begin
                // Slow input device
                repeat (10)
                    tick();
                reg_inpr <= 60;
                reg_fgiset <= 1;
                tick();
                reg_fgiset <= 0;
                
                while (uut.io_fgi)
                    tick();
                
                repeat (10)
                    tick();
                reg_inpr <= 50;
                reg_fgiset <= 1;
                tick();
                reg_fgiset <= 0;
                
                while (uut.io_fgi)
                    tick();
            end
        end
        tick();
    end
end

endmodule
