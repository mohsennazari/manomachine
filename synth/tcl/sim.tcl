onerror {resume}

add mem {uut/cor_mem_inst/inst/mem} -a hexadecimal -d hexadecimal -wo 4

add wave -noupdate -divider Input/Output
add wave -noupdate -format  Logic                      uut/io_clock
add wave -noupdate -format  Logic                      uut/io_fgi
add wave -noupdate -format  Logic                      uut/io_fgiset
add wave -noupdate -format  Literal -radix hexadecimal uut/io_inpr
add wave -noupdate -format  Logic                      uut/io_fgo
add wave -noupdate -format  Logic                      uut/io_fgoset
add wave -noupdate -format  Literal -radix hexadecimal uut/io_outr
add wave -noupdate -divider Registers
add wave -noupdate -format  Literal -radix hexadecimal uut/reg_ac
add wave -noupdate -format  Literal -radix hexadecimal uut/reg_ar
add wave -noupdate -format  Literal -radix hexadecimal uut/wir_dri
add wave -noupdate -format  Literal -radix hexadecimal uut/reg_dro
add wave -noupdate -format  Logic                      uut/reg_e
add wave -noupdate -format  Logic                      uut/reg_ien
add wave -noupdate -format  Logic                      uut/reg_memwe
add wave -noupdate -format  Literal -radix hexadecimal uut/reg_pc
add wave -noupdate -format  Logic                      uut/reg_s
add wave -noupdate -format  Literal -radix hexadecimal uut/reg_sc

source "tcl/common.tcl"
