onerror {resume}

vlib work
vlog +acc "netgen/par/main_timesim.v"
vlog +acc "sim/sim_speed.v"

vsim +maxdelays -L simprims_ver -gui -sdftyp {/sim/uut=./main_timesim.sdf} work.sim

add wave -noupdate -divider Input/Output
add wave -noupdate -format Logic /sim/uut/io_clock
add wave -noupdate -format Logic /sim/uut/io_fgi
add wave -noupdate -format Logic /sim/uut/io_fgiset
add wave -noupdate -format Literal -radix hexadecimal /sim/uut/io_inpr
add wave -noupdate -format Logic /sim/uut/io_fgo
add wave -noupdate -format Logic /sim/uut/io_fgoset
add wave -noupdate -format Literal -radix hexadecimal /sim/uut/io_outr
add wave -noupdate -divider Registers
add wave -noupdate -format Literal -radix hexadecimal /sim/uut/reg_ac
add wave -noupdate -format Literal -radix hexadecimal /sim/uut/reg_ar
add wave -noupdate -format Literal -radix hexadecimal /sim/uut/wir_dri
add wave -noupdate -format Literal -radix hexadecimal /sim/uut/reg_dro
add wave -noupdate -format Logic /sim/uut/reg_e
add wave -noupdate -format Logic /sim/uut/reg_ien
add wave -noupdate -format Logic /sim/uut/reg_memwe
add wave -noupdate -format Literal -radix hexadecimal /sim/uut/reg_pc
add wave -noupdate -format Logic /sim/uut/reg_s
add wave -noupdate -format Literal -radix hexadecimal /sim/uut/reg_sc

source "tcl/mem_ppar.do"
source "tcl/common.do"
