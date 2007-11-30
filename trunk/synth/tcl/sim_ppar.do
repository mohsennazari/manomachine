onerror {resume}

vlib work
vlog +acc "netgen/par/main_timesim.v"
vlog +acc "sim/sim_ppar.v"

vsim +maxdelays -L simprims_ver -gui -sdftyp {/sim_ppar/uut=./main_timesim.sdf} work.sim_ppar

add mem /sim_ppar/uut/cor_mem_inst_B6/mem  -a hexadecimal -d hexadecimal -wo 16
add mem /sim_ppar/uut/cor_mem_inst_B10/mem -a hexadecimal -d hexadecimal -wo 16
add mem /sim_ppar/uut/cor_mem_inst_B14/mem -a hexadecimal -d hexadecimal -wo 16
add mem /sim_ppar/uut/cor_mem_inst_B18/mem -a hexadecimal -d hexadecimal -wo 16

add wave -noupdate -divider Input/Output
add wave -noupdate -format Logic /sim_ppar/uut/io_clock
add wave -noupdate -format Logic /sim_ppar/uut/io_fgi
add wave -noupdate -format Logic /sim_ppar/uut/io_fgiset
add wave -noupdate -format Literal -radix hexadecimal /sim_ppar/uut/io_inpr
add wave -noupdate -format Logic /sim_ppar/uut/io_fgo
add wave -noupdate -format Logic /sim_ppar/uut/io_fgoset
add wave -noupdate -format Literal -radix hexadecimal /sim_ppar/uut/io_outr
add wave -noupdate -divider Registers
add wave -noupdate -format Literal -radix hexadecimal /sim_ppar/uut/reg_ac
add wave -noupdate -format Literal -radix hexadecimal /sim_ppar/uut/reg_ar
add wave -noupdate -format Literal -radix hexadecimal /sim_ppar/uut/wir_dri
add wave -noupdate -format Literal -radix hexadecimal /sim_ppar/uut/reg_dro
add wave -noupdate -format Logic /sim_ppar/uut/reg_e
add wave -noupdate -format Logic /sim_ppar/uut/reg_ien
add wave -noupdate -format Logic /sim_ppar/uut/reg_memwe
add wave -noupdate -format Literal -radix hexadecimal /sim_ppar/uut/reg_pc
add wave -noupdate -format Logic /sim_ppar/uut/reg_s
add wave -noupdate -format Literal -radix hexadecimal /sim_ppar/uut/reg_sc

source "tcl/common.do"
