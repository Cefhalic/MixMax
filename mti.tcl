# ModelSim script for testing int64 prng

file mkdir ./modelsim/libs/msim
cd modelsim

vlib libs/work
vlib libs/msim

vlib libs/msim/lib
vmap lib libs/msim/lib

vcom -2008 -work lib ../MixMax.vhd
vcom -2008 -work lib ../mti.vhd

vsim -t fs -voptargs="+acc" lib.test
set NumericStdNoWarnings 1
set StdArithNoWarnings 1

if { ! [batch_mode] } {
  noview *
  view wave
  config wave -signalnamewidth 1
  delete wave *
  add wave -hex *
}
force -freeze sim:/test/clk 1 0, 0 {50 fs} -r 100
run 10ps
wave zoom full
