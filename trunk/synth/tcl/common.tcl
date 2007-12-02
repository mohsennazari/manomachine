onerror {resume}

quietly WaveActivateNextPane {} 0

TreeUpdate [SetDefaultTree]

configure wave -namecolwidth 229
configure wave -valuecolwidth 86
configure wave -justifyvalue left
configure wave -signalnamewidth 0
configure wave -snapdistance 10
configure wave -datasetprefix 0
configure wave -rowmargin 4
configure wave -childrowmargin 2
configure wave -gridoffset 0
configure wave -gridperiod 1
configure wave -griddelta 40
configure wave -timeline 0

update

WaveRestoreZoom {0 ps} {200 ns}

set RunLength "100 ns"
