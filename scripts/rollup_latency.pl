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
#use strict;
#use warnings;
my $stat_file = $ARGV[0];

    my $metric = $_;
    my $total_latency = 0;
    my $no_nodes = 0;
    my $throughput = 0;
    my $throughput_out = 0;
    my $my_reqs= 0;
    open(DAT, $stat_file);
    while (my $line = <DAT>)
    {
        my $thr_id_printed = 1;
        chomp $line;
        if($line =~ /interface/ && $line =~ /avg_packet_latency/) 
        { 
            $line =~ m/(\d+)*: (\d+.\d+)/;
            print ("\n $line ");
            $total_latency = $total_latency + $2;
            $no_nodes++;
        }
        if($line =~ /interface/ && $line =~ /packets_in/) 
        {
            $line =~ m/(\d+)*: (\d+.?\d*)/;
            print ("\n $line ");
            $throughput = $throughput + $2;
        }
        if($line =~ /interface/ && $line =~ /packets_out/) 
        {
            $line =~ m/(\d+)*: (\d+.?\d*)/;
            print ("\n $line ");
            $throughput_out = $throughput_out + $2;
        }
        if($line =~ /MC/ && $line =~ /Total Request  for Thread/) 
        {
            $line =~ m/(\d+)*= (\d+.?\d*)/;
            print ("\n $line ");
            $mc_reqs = $mc_reqs + $2;
        }
    }
    if( $no_nodes != 0)
    {
        my $avg_lat = $total_latency/$no_nodes;
        print ("\n Avg Lat: $avg_lat");
        print ("\n No of packets_in: $throughput");
        print ("\n No of packets_out: $throughput_out");
        print ("\n MC reqs: $mc_reqs");
    }
    print"\n";
    close DAT;


