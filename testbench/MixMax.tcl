# -----------------------------------------------------------------------------------------------------------------------
# Andrew W. Rose, 2022
# Imperial College London HEP group
# and
# Centre for High-throughput digital electronics and embedded machine learning
# -----------------------------------------------------------------------------------------------------------------------

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
  # add wave -divider
  # add wave -hex MixMaxVhdInstance/*
  # add wave -hex MixMaxVhdInstance/W(0)
  # add wave -hex MixMaxVhdInstance/W(13)
  # add wave -hex MixMaxVhdInstance/PartialSumOverOld
  # add wave -hex MixMaxVhdInstance/SumOverNew
  # add wave -hex MixMaxVhdInstance/RotatedPreviousPartialSumOverOld
  # add wave -hex MixMaxVhdInstance/PreSum
}

force -freeze sim:/Testbench/clk 1 0, 0 {1 ns} -r 2ns
run 5ms
wave zoom full
