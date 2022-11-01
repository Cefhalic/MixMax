# 200MHz
# create_clock -period 5.000 -name Clk_1 -waveform {0.000 2.50} [get_ports Clk]

# 250MHz
# create_clock -period 4.000 -name Clk_1 -waveform {0.000 2.00} [get_ports Clk]

# 300MHz
create_clock -period 3.333 -name Clk_1 -waveform {0.000 1.666} [get_ports Clk]

# 333MHz
# create_clock -period 3.000 -name Clk_1 -waveform {0.000 1.50} [get_ports Clk]

# 400MHz
# create_clock -period 2.500 -name Clk_1 -waveform {0.000 1.25} [get_ports Clk]