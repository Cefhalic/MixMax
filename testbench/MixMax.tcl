# ModelSim script for testing int64 prng

file mkdir ./modelsim/libs/msim
cd modelsim

vlib libs/work
vlib libs/msim
vlib libs/msim/lib
vmap lib libs/msim/lib

vcom -2008 -work lib ../firmware/MixMax.vhd ../testbench/Testbench.vhd

vsim -t ns -voptargs="+acc" lib.Testbench
set NumericStdNoWarnings 1
set StdArithNoWarnings 1

if { ! [batch_mode] } {
  noview *
  view wave
  config wave -signalnamewidth 1
  delete wave *
  add wave -hex *
}
force -freeze sim:/Testbench/clk 1 0, 0 {1 ns} -r 2ns
run 500ns
wave zoom full
