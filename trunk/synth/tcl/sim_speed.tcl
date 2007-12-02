onerror {resume}

add mem /sim_speed/uut/cor_mem_inst_B6/mem  -a hexadecimal -d hexadecimal -wo 16
add mem /sim_speed/uut/cor_mem_inst_B10/mem -a hexadecimal -d hexadecimal -wo 16
add mem /sim_speed/uut/cor_mem_inst_B14/mem -a hexadecimal -d hexadecimal -wo 16
add mem /sim_speed/uut/cor_mem_inst_B18/mem -a hexadecimal -d hexadecimal -wo 16

add wave -noupdate -divider Input/Output
add wave -noupdate -format Logic                      /sim_speed/uut/io_clock
add wave -noupdate -format Logic                      /sim_speed/uut/io_fgi
add wave -noupdate -format Logic                      /sim_speed/uut/io_fgiset
add wave -noupdate -format Literal -radix hexadecimal /sim_speed/uut/io_inpr
add wave -noupdate -format Logic                      /sim_speed/uut/io_fgo
add wave -noupdate -format Logic                      /sim_speed/uut/io_fgoset
add wave -noupdate -format Literal -radix hexadecimal /sim_speed/uut/io_outr
add wave -noupdate -divider Registers
add wave -noupdate -format Literal -radix hexadecimal /sim_speed/uut/reg_ac
add wave -noupdate -format Literal -radix hexadecimal /sim_speed/uut/reg_ar
add wave -noupdate -format Literal -radix hexadecimal /sim_speed/uut/wir_dri
add wave -noupdate -format Literal -radix hexadecimal /sim_speed/uut/reg_dro
add wave -noupdate -format Logic                      /sim_speed/uut/reg_e
add wave -noupdate -format Logic                      /sim_speed/uut/reg_ien
add wave -noupdate -format Logic                      /sim_speed/uut/reg_memwe
add wave -noupdate -format Literal -radix hexadecimal /sim_speed/uut/reg_pc
add wave -noupdate -format Logic                      /sim_speed/uut/reg_s
add wave -noupdate -format Literal -radix hexadecimal /sim_speed/uut/reg_sc

source "tcl/common.tcl"
