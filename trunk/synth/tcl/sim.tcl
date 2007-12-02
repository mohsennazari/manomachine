onerror {resume}

add mem {/sim/uut/pri_mem0/mem} -a hexadecimal -d hexadecimal -wo 16
add mem {/sim/uut/pri_mem1/mem} -a hexadecimal -d hexadecimal -wo 16
add mem {/sim/uut/pri_mem2/mem} -a hexadecimal -d hexadecimal -wo 16
add mem {/sim/uut/pri_mem3/mem} -a hexadecimal -d hexadecimal -wo 16

add wave -noupdate -divider Input/Output
add wave -noupdate -format Logic                      /sim/uut/io_clock
add wave -noupdate -format Logic                      /sim/uut/io_fgi
add wave -noupdate -format Logic                      /sim/uut/io_fgiset
add wave -noupdate -format Literal -radix hexadecimal /sim/uut/io_inpr
add wave -noupdate -format Logic                      /sim/uut/io_fgo
add wave -noupdate -format Logic                      /sim/uut/io_fgoset
add wave -noupdate -format Literal -radix hexadecimal /sim/uut/io_outr
add wave -noupdate -divider Registers
add wave -noupdate -format Literal -radix hexadecimal /sim/uut/reg_ac
add wave -noupdate -format Literal -radix hexadecimal /sim/uut/reg_ar
add wave -noupdate -format Literal -radix hexadecimal /sim/uut/wir_dri
add wave -noupdate -format Literal -radix hexadecimal /sim/uut/reg_dro
add wave -noupdate -format Logic                      /sim/uut/reg_e
add wave -noupdate -format Logic                      /sim/uut/reg_ien
add wave -noupdate -format Logic                      /sim/uut/reg_memwe
add wave -noupdate -format Literal -radix hexadecimal /sim/uut/reg_pc
add wave -noupdate -format Logic                      /sim/uut/reg_s
add wave -noupdate -format Literal -radix hexadecimal /sim/uut/reg_sc

source "tcl/common.tcl"
