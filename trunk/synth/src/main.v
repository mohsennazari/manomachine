//////////////////////////////////////////////////////////////////////////////
// Mano machine project
// (c) Greg Toombs 2007
// 
// main.v
// 
// Contains the main synthesis module.


// The simulation timescale.
`timescale 1ns / 1ps


// Module definition.
module main
(
    input            io_clock,  // The system clock.
    
    input      [7:0] io_inpr,   // The character input bus.
    output reg       io_fgi,    // The character input flag.
    input            io_fgiset, // The character input flag set line.
    output reg [7:0] io_outr,   // The character output register.
    output reg       io_fgo,    // The character output flag.
    input            io_fgoset  // The character output flag set line.
);


// High-order op code parameters (bits 14:12).
parameter[2:0]
    par_op_and   = 0, // (0xxx, 8xxx) And memory to accumulator.
    par_op_add   = 1, // (1xxx, 9xxx) Add...
    par_op_lda   = 2, // (2xxx, Axxx) Load...
    par_op_sta   = 3, // (3xxx, Bxxx) Store accumulator to memory.
    par_op_bun   = 4, // (4xxx, Cxxx) Branch unconditionally.
    par_op_bsa   = 5, // (5xxx, Dxxx) Branch and save return address.
    par_op_isz   = 6, // (6xxx, Exxx) Increment memory and skip if zero.
    par_op_regio = 7; // (7xxx, Fxxx) Register or input/output operation.


// Full-word op codes parameters.
parameter[15:0]
    par_op_cla = 16'h7800, // Clear accumulator.
    par_op_cle = 16'h7400, //    ...extra carry flag.
    par_op_cma = 16'h7200, // Complement accumulator.
    par_op_cme = 16'h7100, //         ...extra carry flag.
    par_op_cir = 16'h7080, // Circulate accumulator and extra carry flag right.
    par_op_cil = 16'h7040, //                                         ...left.
    par_op_inc = 16'h7020, // Increment accumulator.
    par_op_spa = 16'h7010, // Skip next instruction if accumulator is positive.
    par_op_sna = 16'h7008, //                                      ...negative.
    par_op_sza = 16'h7004, //                                      ...zero.
    par_op_sze = 16'h7002, //                    ...if extra carry is zero.
    par_op_hlt = 16'h7001, // Halt processor.
    
    par_op_inp = 16'hF800, // Input character to accumulator.
    par_op_out = 16'hF400, // Output character from accumulator.
    par_op_ski = 16'hF200, // Skip on input flag.
    par_op_sko = 16'hF100, //      ...output flag.
    par_op_ion = 16'hF080, // Interrupt on.
    par_op_iof = 16'hF040; //        ...off.


// State counter parameters. These are re-encoded as one-hot.
parameter[4:0]
    par_sc_instreq  = 01, // Request an instruction.
    par_sc_instwait = 02, // Wait for the instruction to appear on the bus.
    par_sc_instexec = 03, // Start execution of an instruction.
    par_sc_andwait  = 04, // Wait for the data to appear for an and.
    par_sc_addwait  = 05, //                                 ...add.
    par_sc_ldawait  = 06, //                                 ...lda.
    par_sc_iszwait  = 07, //                                 ...isz.
    par_sc_andexec  = 08, // Execute an and.
    par_sc_addexec  = 09, //         ...add.
    par_sc_ldaexec  = 10, //         ...lda.
    par_sc_iszexec  = 11, //         ...isz.
    par_sc_andint   = 12, // Execute (before an interrupt) an and.
    par_sc_addint   = 13, //                               ...add.
    par_sc_ldaint   = 14, //                               ...lda.
    par_sc_iandwait = 15, // Wait for an indirect address for an and.
    par_sc_iaddwait = 16, //                                  ...add.
    par_sc_ildawait = 17, //                                  ...lda.
    par_sc_istawait = 18, //                                  ...sta.
    par_sc_ibunwait = 19, //                                  ...bun.
    par_sc_ibsawait = 20, //                                  ...bsa.
    par_sc_iiszwait = 21, //                                  ...isz.
    par_sc_iandexec = 22, // Move to the direct version of an and.
    par_sc_iaddexec = 23, //                               ...add.
    par_sc_ildaexec = 24, //                               ...lda.
    par_sc_iiszexec = 25, //                               ...isz. 
    par_sc_istaexec = 26, // Perform (after an indirection) a sta.
    par_sc_ibunexec = 27, //                               ...bun.
    par_sc_ibsaexec = 28; //                               ...bsa.


// Registers.
reg[15:0] reg_ac;    // The accumulator.
reg[11:0] reg_ar;    // The internal data address bus.
reg[15:0] reg_dro;   // The output data register (to internal memory).
reg       reg_e;     // The extra carry flag.
reg       reg_ien;   // The interrupt enable flag.
reg       reg_memwe; // The data memory write flag.
reg[11:0] reg_pc;    // The program counter.
reg       reg_s;     // The processor start/stop request flag. Start = 1.
reg[4:0]  reg_sc;    // The instruction state machine sequence counter.


// Wires.
wire[16:0] wir_add;  // The full sum of the accumulator and the operand.
wire[11:0] wir_ar;   // The address part of wir_dri.
wire[15:0] wir_dri;  // The input data bus (from internal memory).
wire       wir_fgi;  // The effective character input flag.
wire       wir_fgo;  // The effective character output flag.
wire       wir_i;    // The indirect flag part of wir_dri.
wire[15:0] wir_idri; // The incremented wir_dri, forced down to 16 bits to
                     // avoid horrible breakage.
wire[2:0]  wir_op;   // The 3-bit opcode part of wir_dri.
wire       wir_sign; // The accumulator sign flag.


// Wire assignments.
assign wir_add  = {{1'b0, reg_ac} + {1'b0, wir_dri}};
assign wir_ar   = wir_dri[11:0];
assign wir_fgi  = io_fgi | io_fgiset;
assign wir_fgo  = io_fgo | io_fgoset;
assign wir_i    = wir_dri[15];
assign wir_idri = wir_dri + 1;
assign wir_int  = reg_ien && (wir_fgi || wir_fgo);
assign wir_op   = wir_dri[14:12];
assign wir_sign = reg_ac[15];


// Memory instantiation.
cor_mem cor_mem_inst
(
	.addr(reg_ar   ),
	.clk (io_clock ),
	.din (reg_dro  ),
	.dout(wir_dri  ),
	.we  (reg_memwe)
);


// Initial register values.
initial
begin
    io_outr <= 8'hZZ; // Initially output is disabled.
    io_fgi  <= 0;     // Default value of input flag (from specifications)
    io_fgo  <= 1;     // Default value of output flag (from specifications)
    
    reg_ac  <= 0;
    reg_e   <= 0;
    reg_ien <= 0;
    reg_pc  <= 0;
    
    reg_s  <= 1;               // Initially the processor is not halted.
    reg_sc <= par_sc_instwait; // Wait for the first instruction.
end


// The instruction fetch task.
task tsk_fetch;
    // The program counter to be fetched to.
    input[11:0] wir_pc;
begin
    if (wir_int)
    begin
        // An interrupt will be performed, altering the fetch.
        
        // Disable further interrupts for now.
        reg_ien <= 0;
        
        // Write current program counter to address 0.
        reg_memwe <= 1;
        reg_ar <= 0;
        reg_dro <= wir_pc;
        
        // Jump to instruction address 1.
        reg_pc <= 1;
    end
    else
    begin
        // Perform a normal fetch.
        reg_memwe <= 0;
        reg_ar <= wir_pc;
        reg_pc <= wir_pc;
    end
        
    // Update the sequence counter.
    if (!(reg_sc == par_sc_andwait ||
          reg_sc == par_sc_addwait ||
          reg_sc == par_sc_ldawait))
        reg_sc <= wir_int ? par_sc_instreq : par_sc_instwait;
end
endtask


// Clocked logic.
always @(posedge io_clock)
begin
    // Obey character communication flag set lines.
    if (io_fgiset)
        io_fgi <= 1;
    if (io_fgoset)
        io_fgo <= 1;

    // Only obey the clock if the start signal is asserted (i.e. the processor
    // is not halted).
    if (reg_s)
    begin
        // Examine the state counter and act accordingly.
        
        case (reg_sc)
        
        // Request an instruction.
        par_sc_instreq:
            tsk_fetch(reg_pc);
        
        // Wait for the instruction to appear on the data bus.
        par_sc_instwait:
            reg_sc <= par_sc_instexec;
        
        // Execute the instruction on the data bus.
        par_sc_instexec:
        begin
            // Deal with simple instructions.
            case (wir_dri)
            par_op_cla:
                reg_ac <= 0;
            par_op_cle:
                reg_e <= 0;
            par_op_cma:
                reg_ac <= ~reg_ac;
            par_op_cme:
                reg_e <= ~reg_e;
            par_op_cir: begin
                reg_ac <= {reg_e, reg_ac[15:1]};
                reg_e <= reg_ac[0];
            end
            par_op_cil: begin
                reg_ac <= {reg_ac[14:0], reg_e};
                reg_e <= reg_ac[15];
            end
            par_op_inc:
                reg_ac <= reg_ac + 1;
            par_op_hlt:
                reg_s <= 0;
            par_op_inp: begin
                reg_ac[7:0] <= io_inpr;
                io_fgi <= 0;
            end
            par_op_out: begin
                io_outr <= reg_ac[7:0];
                io_fgo <= 0;
            end
            par_op_ion:
                reg_ien <= 1;
            par_op_iof:
                reg_ien <= 0;
            endcase

            // Check for skips or branches, update the program counter, and
            // perform a fetch.
            if ((wir_dri == par_op_spa && !wir_sign) ||
                (wir_dri == par_op_sna &&  wir_sign) ||
                (wir_dri == par_op_sza && !reg_ac  ) ||
                (wir_dri == par_op_sze && !reg_e   ) ||
                (wir_dri == par_op_ski &&  io_fgi  ) ||
                (wir_dri == par_op_sko &&  io_fgo  ))
            begin
                // Skip the next instruction.
                tsk_fetch(reg_pc + 2);
            end
            else
            begin
                if (wir_op == par_op_bun || wir_op == par_op_bsa)
                begin
                    if (!wir_i)
                    begin
                        // Branch.
                        if (wir_op == par_op_bun)
                            tsk_fetch(wir_ar);
                        else
                            reg_pc <= wir_ar + 1;
                    end
                end
                else
                begin
                    // Fetch normally.
                    if (wir_op == par_op_regio)
                        tsk_fetch(reg_pc + 1);
                    else
                        reg_pc <= reg_pc + 1;
                end
            end
        
            if (wir_op != par_op_regio)
            begin
                // Update the state counter for memory operations.
                case (wir_op)
                par_op_and:
                    reg_sc <= wir_i ? par_sc_iandwait : par_sc_andwait;
                par_op_add:
                    reg_sc <= wir_i ? par_sc_iaddwait : par_sc_addwait;
                par_op_lda:
                    reg_sc <= wir_i ? par_sc_ildawait : par_sc_ldawait;
                par_op_bun:
                    if (wir_i)
                        reg_sc <= par_sc_ibunwait;
                par_op_bsa:
                    if (wir_i)
                        reg_sc <= par_sc_ibsawait;
                par_op_sta:
                    reg_sc <= wir_i ? par_sc_istawait : par_sc_instreq;
                par_op_isz:
                    reg_sc <= wir_i ? par_sc_iiszwait : par_sc_iszwait;
                endcase
            
                // Update the read/write flag, address, and output data
                // registers for memory operations.
                
                if (wir_op == par_op_sta || wir_op == par_op_bsa)
                begin
                    reg_memwe <= !wir_i;
                    reg_dro <= (wir_op == par_op_sta) ? reg_ac : reg_pc;
                end
                else
                    reg_memwe <= 0;
                    
                reg_ar <= wir_ar;
            end
        end
        
        // Wait for the data for memory instructions to appear on the data bus.
        
        par_sc_andwait:
        begin
            tsk_fetch(reg_pc);
            reg_sc <= wir_int ? par_sc_andint : par_sc_andexec;
        end
        
        par_sc_addwait:
        begin
            tsk_fetch(reg_pc);
            reg_sc <= wir_int ? par_sc_addint : par_sc_addexec;
        end
        
        par_sc_ldawait:
        begin
            tsk_fetch(reg_pc);
            reg_sc <= wir_int ? par_sc_ldaint : par_sc_ldaexec;
        end
        
        par_sc_iszwait:
            reg_sc <= par_sc_iszexec;
        
        // Execute memory instructions.
        
        par_sc_andexec:
        begin
            reg_ac <= reg_ac & wir_dri;
            reg_sc <= par_sc_instexec;
        end
        
        par_sc_addexec:
        begin
            reg_ac <= wir_add[15:0];
            reg_e  <= wir_add[16];
            reg_sc <= par_sc_instexec;
        end
        
        par_sc_ldaexec:
        begin
            reg_ac <= wir_dri;
            reg_sc <= par_sc_instexec;
        end
        
        par_sc_iszexec:
        begin
            reg_memwe <= 1;
            reg_dro <= wir_idri;
            if (wir_idri == 0)
            begin
                reg_pc <= reg_pc + 1;
            end
            reg_sc <= par_sc_instreq;
        end
        
        // Execute memory instructions after an interrupt has been performed.
        
        par_sc_andint:
        begin
            reg_ac <= reg_ac & wir_dri;
            reg_sc <= par_sc_instreq;
        end
        
        par_sc_addint:
        begin
            reg_ac <= wir_add[15:0];
            reg_e  <= wir_add[16];
            reg_sc <= par_sc_instreq;
        end
        
        par_sc_ldaint:
        begin
            reg_ac <= wir_dri;
            reg_sc <= par_sc_instreq;
        end
        
        // Wait for the indirect address to appear on the data bus.
        
        par_sc_iandwait:
            reg_sc <= par_sc_iandexec;
        
        par_sc_iaddwait:
            reg_sc <= par_sc_iaddexec;
        
        par_sc_ildawait:
            reg_sc <= par_sc_ildaexec;
        
        par_sc_istawait:
            reg_sc <= par_sc_istaexec;
        
        par_sc_ibunwait:
            reg_sc <= par_sc_ibunexec;
        
        par_sc_ibsawait:
            reg_sc <= par_sc_ibsaexec;
        
        par_sc_iiszwait:
            reg_sc <= par_sc_iiszexec;
        
        // Execute indirections by updating the address bus and moving to the
        // direct memory instruction states.
        
        par_sc_iandexec:
        begin
            reg_ar <= wir_dri;
            reg_sc <= par_sc_andwait;
        end
            
        par_sc_iaddexec:
        begin
            reg_ar <= wir_dri;
            reg_sc <= par_sc_addwait;
        end
            
        par_sc_ildaexec:
        begin
            reg_ar <= wir_dri;
            reg_sc <= par_sc_ldawait;
        end
        
        par_sc_iiszexec:
        begin
            reg_ar <= wir_dri;
            reg_sc <= par_sc_iszwait;
        end
        
        // Execute these indirect instructions immediately.
            
        par_sc_istaexec:
        begin
            reg_memwe <= 1;
            reg_ar <= wir_dri;
            reg_sc <= par_sc_instreq;
        end
        
        par_sc_ibunexec:
            tsk_fetch(wir_dri);
        
        par_sc_ibsaexec:
        begin
            reg_memwe <= 1;
            reg_ar <= wir_dri;
            reg_pc <= wir_dri + 1;
            reg_sc <= par_sc_instreq;
        end
            
        endcase
        
    end
end

endmodule
