# ModelSim script for testing int64 prng

file mkdir ./modelsim/libs/msim
cd modelsim

vmap -c

vlib libs/work
vlib libs/msim
vlib libs/msim/lib
vmap lib libs/msim/lib

vcom -2008 -work lib ../MixMax.vhd

vsim -t fs -voptargs="+acc" lib.MixMax
set NumericStdNoWarnings 1
set StdArithNoWarnings 1

if { ! [batch_mode] } {
  noview *
  view wave
  config wave -signalnamewidth 1
  delete wave *
  add wave -hex *
}

force -freeze sim:/mixmax/Clk 1 0, 0 {50 fs} -r 100
run 5.65ps
wave zoom full
