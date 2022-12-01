create_project vivado-mt19937 ./vivado-mt19937 -part xcku15p-ffva1760-1-e -force

set_property target_language VHDL [current_project]
set_property simulator_language VHDL [current_project]

add_files -norecurse ./firmware/MT19937.vhd
set_property FILE_TYPE {VHDL 2008} [get_files ./firmware/MT19937.vhd]

add_files -fileset constrs_1 -norecurse ./xilinx/Clk.xdc

update_compile_order -fileset sources_1

launch_runs synth_1 impl_1 -jobs 4

# open_run impl_1
# report_utilization -name utilization_1
# report_timing_summary -delay_type min_max -report_unconstrained -check_timing_verbose -max_paths 10 -input_pins -routable_nets -name timing_1
