#!/usr/bin/perl 
#===============================================================================
#
#         FILE:  rollup_latency.pl
#
#        USAGE:  ./rollup_latency.pl  
#
#  DESCRIPTION:  
#
#      OPTIONS:  ---
# REQUIREMENTS:  ---
#         BUGS:  ---
#        NOTES:  ---
#       AUTHOR:  YOUR NAME (), 
#      COMPANY:  
#      VERSION:  1.0
#      CREATED:  05/12/2010 07:03:23 PM
#     REVISION:  ---
#===============================================================================

use File::Basename;
my $stat_files = $ARGV[0];
my @benches= `cat $stat_files | grep -v "^#"`;
#my @mc_metrics= `cat metrics_mc_multi | grep -v "^#"`;
#my @ni_metrics= `cat metrics_ni_multi | grep -v "^#"`;
#my @interface_metrics= `cat metrics_interface_multi | grep -v "^#"`;
#my @router_metrics= `cat metrics_router_multi | grep -v "^#"`;
#my @tpg_metrics= `cat metrics_tpg_multi | grep -v "^#"`;
my @router_metrics = (
    'packet latency:',
    'flits/packets:',
    'last_flit_out_cycle:',
    'packets:',
    'flits:'
    );

my @tpg_metrics = (
    'packets_out:',
    'avg_fwd_path_delay:',
    'min_pkt_latency:',
    'last_packet_out_cycle:',
    'packet_in:',
    'avg_round_trip_latency:',
    'avg_blp:',
    'total_unsink:',
    'avg_round_trip_hop_count:',
    'avg_round_trip_network_latency:',
    'avg_round_trip_memory_latency:',
    'avg_round_trip_waiting_in_ni:'
);

my @ni_metrics = (
    'last_pkt_out_cycle:',
    'total_missed_time:',
    'packets_in:',
    'packets_out:',
    'total_backward_time:',
    'avg_latency_waiting_in_ni\(fwd path\):',
    'avg_latency_waiting_in_ni\(bwd path\):'
);

my @mc_metrics = (
    'bus_busy_cycles:',
    'per_bus_occupancy:',
    'dram_busy_cycles:',
    'dram_read_cycles:',
    'dram_write_cycles:',
    'percentage_dram_read_cycles:',
    'percentage_dram_write_cycles:',
    'overall_page_hit_rate:',
    'overall_background_power\(mW\)',
    'overall_activate_precharge_power\(mW\)',
    'overall_read_write_termination_power\(mW\)',
    'total_dram_power\(mW\)'
);

chomp @mc_metrics;
chomp @ni_metrics;
chomp @interface_metrics;
chomp @router_metrics;
chomp @tpg_metrics;

print("\nRouter_metrics:,".@router_metrics." Int:,".@interface_metrics." MC:,".@mc_metrics." NI:,".@ni_metrics);

print "\nExperiment,stat,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63\n";
my @mc_stats;
my @ni_stats;
my @interface_stats;
my @router_stats;
my @tpg_stats;

#Rollup for tpg 
for( my $aa = 0; $aa<@tpg_metrics; $aa++)
{
    my $metric = $tpg_metrics[$aa];
    foreach(@benches)
    {
        my $stat_file = $_;
        chomp $stat_file;
        open(DAT, $stat_file);
        while (my $line = <DAT>)
        {
            chomp $line;
            if($line =~ /TPG/ && $line =~ /\] $metric/)
            {
                my $line2 = $line;
                $line =~ m/^*(\d*)]/;
                my $no = $1;
                $line2 =~ m/:*(\d*.?\d*e?\+?-?\d+)$/;
                my $crap = $1;
                $tpg_stats[$aa][$no] = $crap;
            }
        }
        close DAT;

        chomp @tpg_stats;
        print "\n".basename($stat_file).",TPG_$metric,";
        foreach(my$b=0; $b<64; $b++) 
        {
            print "$tpg_stats[$aa][$b],";
        }
    }
    print "\n";
}
print "\n\n";
    
#Rollup for router 
for( my $aa = 0; $aa<@router_metrics; $aa++)
{
    my $metric = $router_metrics[$aa];
    foreach(@benches)
    {
        my $stat_file = $_;
        chomp $stat_file;
        open(DAT, $stat_file);
        while (my $line = <DAT>)
        {
            chomp $line;
            if($line =~ /router/ && $line =~ /\] $metric/)
            {
                my $line2 = $line;
                $line =~ m/^*(\d*)]/;
                my $no = $1;
                $line2 =~ m/:*(\d*.?\d*e?\+?-?\d+) ?$/;
                my $crap = $1;
                $router_stats[$aa][$no] = $crap;
            }
        }
        close DAT;

        chomp @router_stats;
        print "\n".basename($stat_file).",Router_$metric,";
        foreach(my$b=0; $b<64; $b++) 
        {
            print "$router_stats[$aa][$b],";
        }
    }
    print "\n";
}
print "\n\n";
    
#Rollup for interface
for( my $aa = 0; $aa<@interface_metrics; $aa++)
{
    my $metric = $interface_metrics[$aa];
    foreach(@benches)
    {
        my $stat_file = $_;
        chomp $stat_file;
        open(DAT, $stat_file);
        while (my $line = <DAT>)
        {
            chomp $line;
            if($line =~ /interface/ && $line =~ /\] $metric/)
            {
                my $line2 = $line;
                $line =~ m/^*(\d*)]/;
                my $no = $1;
                $line2 =~ m/:*(\d*.?\d*e?\+?-?\d+)$/;
                my $crap = $1;
                $interface_stats[$aa][$no] = $crap;
            }
        }
        close DAT;

        chomp @interface_stats;
        print "\n".basename($stat_file).",interface_$metric,";
        foreach(my$b=0; $b<64; $b++) 
        {
            print "$interface_stats[$aa][$b],";
        }
    }
    print "\n";
}
    print "\n\n";
    
#Rollup for NI
for( my $aa = 0; $aa<@ni_metrics; $aa++)
{
    my $metric = $ni_metrics[$aa];
    foreach(@benches)
    {
        my $stat_file = $_;
        chomp $stat_file;
        open(DAT, $stat_file);
        while (my $line = <DAT>)
        {
            chomp $line;
            if($line =~ /NI/ && $line =~ /\] $metric/)
            {
                my $line2 = $line;
                $line =~ m/^*(\d*)]/;
                my $no = $1;
                $line2 =~ m/:*(\d*.?\d*e?\+?-?\d+)$/;
                my $crap = $1;
                $ni_stats[$aa][$no] = $crap;
            }
        }
        close DAT;

        print "\n".basename($stat_file).",NI_$metric,";
        chomp @ni_stats;
        foreach(my$b=0; $b<64; $b++) 
        {
            print "$ni_stats[$aa][$b],";
        }
    }
    print "\n";
}
    print "\n\n";

#Rollup for MC
for( my $aa = 0; $aa<@mc_metrics; $aa++)
{
    my $metric = $mc_metrics[$aa];
    foreach(@benches)
    {
        my $stat_file = $_;
        chomp $stat_file;
        open(DAT, $stat_file);
        while (my $line = <DAT>)
        {
            chomp $line;
            if($line =~ /MC/ && $line =~ /\] $metric/)
            {
                my $line2 = $line;
                $line =~ m/^*(\d*)]/;
                my $no = $1;
                $line2 =~ m/:*(\d*.?\d*e?\+?-?\d+)$/;
                my $crap = $1;
                $mc_stats[$aa][$no] = $crap;
            }
        }
        close DAT;

        print "\n".basename($stat_file).",MC_$metric,";
        chomp @mc_stats;
        foreach(my$b=0; $b<64; $b++) 
        {
            print "$mc_stats[$aa][$b],";
        }
    }
    print "\n";
}
    print "\n\n";
