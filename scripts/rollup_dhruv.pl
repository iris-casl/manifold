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
my @mc_metrics= `cat metrics_multi | grep -v "^#"`;
my @router_metrics= `cat metrics_multi | grep -v "^#"`;
my @interface_metrics= `cat metrics_multi | grep -v "^#"`;
chomp @mc_metrics;
chomp @router_metrics;
chomp @interface_metrics;

print "Statistic,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63";
print "\n\n";

#foreach(@metrics)
#{
    my $metric = $_;

    
    my @interface_pkts_avg_lat=(1 .. 64);
    my @interface_pkts_in=(1 .. 64);
    my @interface_pkts_out=(1 .. 64);
    my @interface_pkts_total_lat=(1 .. 64);
    my @interface_flits_in=(1 .. 64);
    my @interface_flits_out=(1 .. 64);


    my @routers_pkts=(1 .. 64);
    my @routers_flits=(1 .. 64);
    my @routers_pkts_lat=(1 .. 64);
    my @routers_flits_per_pkt=(1 .. 64);
    my @routers_pkts_avg_latency=(1 .. 64);
    my @routers_last_flit_out_time=(1 .. 64);
#    print "$metric,,";
    open(DAT, $stat_file);
    while (my $line = <DAT>)
    {
        my $thr_id_printed = 1;
        chomp $line;
         #if($line =~ m/$metric/) 
         #{ 
         #     #$line =~ m/(\d+)\t(\d+.?\d*)/;
         #     $line =~ m/(\d+) = (\d+.?\d*)/;
         #     $thr_id = $1;
         #     $lat = $2;
         #     #print (" $lat ,");
         #}
#printing stats for MC#0
	 if($line =~ /MC \[0\]Total Request  for Thread/)
	 {
		$line =~ m/Thread (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [0]Total Request  for Thread, ";
		}
		print ("$lat ,");
		if($1 =~ m/63/)
		{
			print "\n";
		}
	 }
	 if($line =~ /MC \[0\]Hits  for Thread/)
	 {
		$line =~ m/Thread (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [0]Hits  for Thread";
		}
		print (" $lat ,");
		if($1 =~ m/63/)
		{
			print "\n";
		}

	 }
	 if($line =~ /MC \[0\]Hit Rate  for Thread/)
	 {
		$line =~ m/Thread (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [0]Hit Rate  for Thread, ";
		}
		print (" $lat ,");
		if($1 =~ m/63/)
		{
			print "\n";
		}
	 }
	 if($line =~ /MC \[0\]Average Latency  for Thread/)
	 {
		$line =~ m/Thread (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [0]Average Latency  for Thread, ";
		}
		print (" $lat ,");
		if($1 =~ m/63/)
		{
			print "\n";
		}
	 }




	 if($line =~ /MC \[0\]Total Request  for Bank/)
	 {
		$line =~ m/Bank (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [0]Total Request  for Bank, ";
		}
		print (" $lat ,");
		if($1 =~ m/7/)
		{
			print "\n";
		}
	 }
	 if($line =~ /MC \[0\]Hits  for Bank/)
	 {
		$line =~ m/Bank (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [0]Hits  for Bank, ";
		}
		print (" $lat ,");
		if($1 =~ m/7/)
		{
			print "\n";
		}
	 }
	 if($line =~ /MC \[0\]Hit Rate  for Bank/)
	 {
		$line =~ m/Bank (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [0]Hit Rate  for Bank, ";
		}
		print (" $lat ,");
		if($1 =~ m/7/)
		{
			print "\n";
		}
	 }
	 if($line =~ /MC \[0\]Average Latency  for Bank/)
	 {
		$line =~ m/Bank (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [0]Average Latency  for Bank, ";
		}
		print (" $lat ,");
		if($1 =~ m/7/)
		{
			print "\n";
		}
	 }

#printing stats for MC#9
         if($line =~ /MC \[9\]Total Request  for Thread/)
	 {
		$line =~ m/Thread (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [9]Total Request  for Thread, ";
		}
		print (" $lat ,");
		if($1 =~ m/63/)
		{
			print "\n";
		}
	 }
	 if($line =~ /MC \[9\]Hits  for Thread/)
	 {
		$line =~ m/Thread (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [9]Hits  for Thread";
		}
		print (" $lat ,");
		if($1 =~ m/63/)
		{
			print "\n";
		}

	 }
	 if($line =~ /MC \[9\]Hit Rate  for Thread/)
	 {
		$line =~ m/Thread (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [9]Hit Rate  for Thread, ";
		}
		print (" $lat ,");
		if($1 =~ m/63/)
		{
			print "\n";
		}
	 }
	 if($line =~ /MC \[9\]Average Latency  for Thread/)
	 {
		$line =~ m/Thread (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [9]Average Latency  for Thread, ";
		}
		print (" $lat ,");
		if($1 =~ m/63/)
		{
			print "\n";
		}
	 }
	 if($line =~ /MC \[9\]Total Request  for Bank/)
	 {
		$line =~ m/Bank (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [9]Total Request  for Bank, ";
		}
		print (" $lat ,");
		if($1 =~ m/7/)
		{
			print "\n";
		}
	 }
	 if($line =~ /MC \[9\]Hits  for Bank/)
	 {
		$line =~ m/Bank (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [9]Hits  for Bank, ";
		}
		print (" $lat ,");
		if($1 =~ m/7/)
		{
			print "\n";
		}
	 }
	 if($line =~ /MC \[9\]Hit Rate  for Bank/)
	 {
		$line =~ m/Bank (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [9]Hit Rate  for Bank, ";
		}
		print (" $lat ,");
		if($1 =~ m/7/)
		{
			print "\n";
		}
	 }
	 if($line =~ /MC \[9\]Average Latency  for Bank/)
	 {
		$line =~ m/Bank (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [9]Average Latency  for Bank, ";
		}
		print (" $lat ,");
		if($1 =~ m/7/)
		{
			print "\n";
		}
	 }

#printing stats for MC 18
	 if($line =~ /MC \[18\]Total Request  for Thread/)
	 {
		$line =~ m/Thread (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [18]Total Request  for Thread, ";
		}
		print (" $lat ,");
		if($1 =~ m/63/)
		{
			print "\n";
		}
	 }
	 if($line =~ /MC \[18\]Hits  for Thread/)
	 {
		$line =~ m/Thread (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [18]Hits  for Thread";
		}
		print (" $lat ,");
		if($1 =~ m/63/)
		{
			print "\n";
		}

	 }
	 if($line =~ /MC \[18\]Hit Rate  for Thread/)
	 {
		$line =~ m/Thread (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [18]Hit Rate  for Thread, ";
		}
		print (" $lat ,");
		if($1 =~ m/63/)
		{
			print "\n";
		}
	 }
	 if($line =~ /MC \[18\]Average Latency  for Thread/)
	 {
		$line =~ m/Thread (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [18]Average Latency  for Thread, ";
		}
		print (" $lat ,");
		if($1 =~ m/63/)
		{
			print "\n";
		}
	 }
	 if($line =~ /MC \[18\]Total Request  for Bank/)
	 {
		$line =~ m/Bank (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [18]Total Request  for Bank, ";
		}
		print (" $lat ,");
		if($1 =~ m/7/)
		{
			print "\n";
		}
	 }
	 if($line =~ /MC \[18\]Hits  for Bank/)
	 {
		$line =~ m/Bank (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [18]Hits  for Bank, ";
		}
		print (" $lat ,");
		if($1 =~ m/7/)
		{
			print "\n";
		}
	 }
	 if($line =~ /MC \[18\]Hit Rate  for Bank/)
	 {
		$line =~ m/Bank (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [18]Hit Rate  for Bank, ";
		}
		print (" $lat ,");
		if($1 =~ m/7/)
		{
			print "\n";
		}
	 }
	 if($line =~ /MC \[18\]Average Latency  for Bank/)
	 {
		$line =~ m/Bank (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [18]Average Latency  for Bank, ";
		}
		print (" $lat ,");
		if($1 =~ m/7/)
		{
			print "\n";
		}
	 }

#printing stats for MC 27
	 if($line =~ /MC \[27\]Total Request  for Thread/)
	 {
		$line =~ m/Thread (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [27]Total Request  for Thread, ";
		}
		print (" $lat ,");
		if($1 =~ m/63/)
		{
			print "\n";
		}
	 }
	 if($line =~ /MC \[27\]Hits  for Thread/)
	 {
		$line =~ m/Thread (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [27]Hits  for Thread";
		}
		print (" $lat ,");
		if($1 =~ m/63/)
		{
			print "\n";
		}

	 }
	 if($line =~ /MC \[27\]Hit Rate  for Thread/)
	 {
		$line =~ m/Thread (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [27]Hit Rate  for Thread, ";
		}
		print (" $lat ,");
		if($1 =~ m/63/)
		{
			print "\n";
		}
	 }
	 if($line =~ /MC \[27\]Average Latency  for Thread/)
	 {
		$line =~ m/Thread (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [27]Average Latency  for Thread, ";
		}
		print (" $lat ,");
		if($1 =~ m/63/)
		{
			print "\n";
		}
	 }
	 if($line =~ /MC \[27\]Total Request  for Bank/)
	 {
		$line =~ m/Bank (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [27]Total Request  for Bank, ";
		}
		print (" $lat ,");
		if($1 =~ m/7/)
		{
			print "\n";
		}
	 }
	 if($line =~ /MC \[27\]Hits  for Bank/)
	 {
		$line =~ m/Bank (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [27]Hits  for Bank, ";
		}
		print (" $lat ,");
		if($1 =~ m/7/)
		{
			print "\n";
		}
	 }
	 if($line =~ /MC \[27\]Hit Rate  for Bank/)
	 {
		$line =~ m/Bank (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [27]Hit Rate  for Bank, ";
		}
		print (" $lat ,");
		if($1 =~ m/7/)
		{
			print "\n";
		}
	 }
	 if($line =~ /MC \[27\]Average Latency  for Bank/)
	 {
		$line =~ m/Bank (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [27]Average Latency  for Bank, ";
		}
		print (" $lat ,");
		if($1 =~ m/7/)
		{
			print "\n";
		}
	 }



#printing stats for MC 36
if($line =~ /MC \[36\]Total Request  for Thread/)
	 {
		$line =~ m/Thread (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [36]Total Request  for Thread, ";
		}
		print (" $lat ,");
		if($1 =~ m/63/)
		{
			print "\n";
		}
	 }
	 if($line =~ /MC \[36\]Hits  for Thread/)
	 {
		$line =~ m/Thread (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [36]Hits  for Thread";
		}
		print (" $lat ,");
		if($1 =~ m/63/)
		{
			print "\n";
		}

	 }
	 if($line =~ /MC \[36\]Hit Rate  for Thread/)
	 {
		$line =~ m/Thread (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [36]Hit Rate  for Thread, ";
		}
		print (" $lat ,");
		if($1 =~ m/63/)
		{
			print "\n";
		}
	 }
	 if($line =~ /MC \[36\]Average Latency  for Thread/)
	 {
		$line =~ m/Thread (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [36]Average Latency  for Thread, ";
		}
		print (" $lat ,");
		if($1 =~ m/63/)
		{
			print "\n";
		}
	 }
	 if($line =~ /MC \[36\]Total Request  for Bank/)
	 {
		$line =~ m/Bank (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [36]Total Request  for Bank, ";
		}
		print (" $lat ,");
		if($1 =~ m/7/)
		{
			print "\n";
		}
	 }
	 if($line =~ /MC \[36\]Hits  for Bank/)
	 {
		$line =~ m/Bank (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [36]Hits  for Bank, ";
		}
		print (" $lat ,");
		if($1 =~ m/7/)
		{
			print "\n";
		}
	 }
	 if($line =~ /MC \[36\]Hit Rate  for Bank/)
	 {
		$line =~ m/Bank (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [36]Hit Rate  for Bank, ";
		}
		print (" $lat ,");
		if($1 =~ m/7/)
		{
			print "\n";
		}
	 }
	 if($line =~ /MC \[36\]Average Latency  for Bank/)
	 {
		$line =~ m/Bank (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [36]Average Latency  for Bank, ";
		}
		print (" $lat ,");
		if($1 =~ m/7/)
		{
			print "\n";
		}
	 }



#printing stats for MC 45

if($line =~ /MC \[45\]Total Request  for Thread/)
	 {
		$line =~ m/Thread (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [45]Total Request  for Thread, ";
		}
		print (" $lat ,");
		if($1 =~ m/63/)
		{
			print "\n";
		}
	 }
	 if($line =~ /MC \[45\]Hits  for Thread/)
	 {
		$line =~ m/Thread (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [45]Hits  for Thread";
		}
		print (" $lat ,");
		if($1 =~ m/63/)
		{
			print "\n";
		}

	 }
	 if($line =~ /MC \[45\]Hit Rate  for Thread/)
	 {
		$line =~ m/Thread (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [45]Hit Rate  for Thread, ";
		}
		print (" $lat ,");
		if($1 =~ m/63/)
		{
			print "\n";
		}
	 }
	 if($line =~ /MC \[45\]Average Latency  for Thread/)
	 {
		$line =~ m/Thread (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [45]Average Latency  for Thread, ";
		}
		print (" $lat ,");
		if($1 =~ m/63/)
		{
			print "\n";
		}
	 }
	 if($line =~ /MC \[45\]Total Request  for Bank/)
	 {
		$line =~ m/Bank (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [45]Total Request  for Bank, ";
		}
		print (" $lat ,");
		if($1 =~ m/7/)
		{
			print "\n";
		}
	 }
	 if($line =~ /MC \[45\]Hits  for Bank/)
	 {
		$line =~ m/Bank (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [45]Hits  for Bank, ";
		}
		print (" $lat ,");
		if($1 =~ m/7/)
		{
			print "\n";
		}
	 }
	 if($line =~ /MC \[45\]Hit Rate  for Bank/)
	 {
		$line =~ m/Bank (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [45]Hit Rate  for Bank, ";
		}
		print (" $lat ,");
		if($1 =~ m/7/)
		{
			print "\n";
		}
	 }
	 if($line =~ /MC \[45\]Average Latency  for Bank/)
	 {
		$line =~ m/Bank (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [45]Average Latency  for Bank, ";
		}
		print (" $lat ,");
		if($1 =~ m/7/)
		{
			print "\n";
		}
	 }


#printing stats for MC 54

if($line =~ /MC \[54\]Total Request  for Thread/)
	 {
		$line =~ m/Thread (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [54]Total Request  for Thread, ";
		}
		print (" $lat ,");
		if($1 =~ m/63/)
		{
			print "\n";
		}
	 }
	 if($line =~ /MC \[54\]Hits  for Thread/)
	 {
		$line =~ m/Thread (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [54]Hits  for Thread";
		}
		print (" $lat ,");
		if($1 =~ m/63/)
		{
			print "\n";
		}

	 }
	 if($line =~ /MC \[54\]Hit Rate  for Thread/)
	 {
		$line =~ m/Thread (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [54]Hit Rate  for Thread, ";
		}
		print (" $lat ,");
		if($1 =~ m/63/)
		{
			print "\n";
		}
	 }
	 if($line =~ /MC \[54\]Average Latency  for Thread/)
	 {
		$line =~ m/Thread (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [54]Average Latency  for Thread, ";
		}
		print (" $lat ,");
		if($1 =~ m/63/)
		{
			print "\n";
		}
	 }
	 if($line =~ /MC \[54\]Total Request  for Bank/)
	 {
		$line =~ m/Bank (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [54]Total Request  for Bank, ";
		}
		print (" $lat ,");
		if($1 =~ m/7/)
		{
			print "\n";
		}
	 }
	 if($line =~ /MC \[54\]Hits  for Bank/)
	 {
		$line =~ m/Bank (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [54]Hits  for Bank, ";
		}
		print (" $lat ,");
		if($1 =~ m/7/)
		{
			print "\n";
		}
	 }
	 if($line =~ /MC \[54\]Hit Rate  for Bank/)
	 {
		$line =~ m/Bank (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [54]Hit Rate  for Bank, ";
		}
		print (" $lat ,");
		if($1 =~ m/7/)
		{
			print "\n";
		}
	 }
	 if($line =~ /MC \[54\]Average Latency  for Bank/)
	 {
		$line =~ m/Bank (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [54]Average Latency  for Bank, ";
		}
		print (" $lat ,");
		if($1 =~ m/7/)
		{
			print "\n";
		}
	 }


#printing stats for MC 63
if($line =~ /MC \[63\]Total Request  for Thread/)
	 {
		$line =~ m/Thread (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [63]Total Request  for Thread, ";
		}
		print (" $lat ,");
		if($1 =~ m/63/)
		{
			print "\n";
		}
	 }
	 if($line =~ /MC \[63\]Hits  for Thread/)
	 {
		$line =~ m/Thread (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [63]Hits  for Thread";
		}
		print (" $lat ,");
		if($1 =~ m/63/)
		{
			print "\n";
		}

	 }
	 if($line =~ /MC \[63\]Hit Rate  for Thread/)
	 {
		$line =~ m/Thread (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [63]Hit Rate  for Thread, ";
		}
		print (" $lat ,");
		if($1 =~ m/63/)
		{
			print "\n";
		}
	 }
	 if($line =~ /MC \[63\]Average Latency  for Thread/)
	 {
		$line =~ m/Thread (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [63]Average Latency  for Thread, ";
		}
		print (" $lat ,");
		if($1 =~ m/63/)
		{
			print "\n";
		}
	 }
	 if($line =~ /MC \[63\]Total Request  for Bank/)
	 {
		$line =~ m/Bank (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [63]Total Request  for Bank, ";
		}
		print (" $lat ,");
		if($1 =~ m/7/)
		{
			print "\n";
		}
	 }
	 if($line =~ /MC \[63\]Hits  for Bank/)
	 {
		$line =~ m/Bank (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [63]Hits  for Bank, ";
		}
		print (" $lat ,");
		if($1 =~ m/7/)
		{
			print "\n";
		}
	 }
	 if($line =~ /MC \[63\]Hit Rate  for Bank/)
	 {
		$line =~ m/Bank (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [63]Hit Rate  for Bank, ";
		}
		print (" $lat ,");
		if($1 =~ m/7/)
		{
			print "\n";
		}
	 }
	 if($line =~ /MC \[63\]Average Latency  for Bank/)
	 {
		$line =~ m/Bank (\d+) = (\d+.?\d*)/;
		my $lat = $2;
		if($1 == 0)
		{
			print "MC [63]Average Latency  for Bank, ";
		}
		print (" $lat ,");
		if($1 =~ m/7/)
		{
			print "\n";
		}
	 }


#printing router statistics

         if($line =~ /router/ && $line =~ /\] packets:/) 
         { 
		    chomp $line;
            my $line2 = $line;
           $line =~ m/^*(\d*)]/;
            my $no = $1;
           $line2 =~ m/:*(\d+.?\d*)$/;
            my $crap = $1;
           $routers_pkts[$no] = $crap;
         }
         if($line =~ /router/ && $line =~ /flits/) 
         { 
		    chomp $line;
            my $line2 = $line;
           $line =~ m/^*(\d*)]/;
            my $no = $1;
           $line2 =~ m/:*(\d+.?\d*)$/;
            my $crap = $1;
           $routers_flits[$no] = $crap;
         }
         if($line =~ /router/ && $line =~ /packet latency/) 
         { 
		    chomp $line;
            my $line2 = $line;
           $line =~ m/^*(\d*)]/;
            my $no = $1;
           $line2 =~ m/:*(\d+.?\d*)$/;
            my $crap = $1;
           $routers_pkts_lat[$no] = $crap;
         }
         if($line =~ /router/ && $line =~ /flits\/packet/) 
         { 
		    chomp $line;
            my $line2 = $line;
           $line =~ m/^*(\d*)]/;
            my $no = $1;
           $line2 =~ m/:*(\d+.?\d*)$/;
            my $crap = $1;
           $routers_flits_per_pkt[$no] = $crap;
         }
         if($line =~ /router/ && $line =~ /average packet latency/) 
         { 
		    chomp $line;
            my $line2 = $line;
           $line =~ m/^*(\d*)]/;
            my $no = $1;
           $line2 =~ m/:*(\d+.?\d*)$/;
            my $crap = $1;
           $routers_pkts_avg_latency[$no] = $crap;
         }
         if($line =~ /router/ && $line =~ /last_flit_out_cycle/) 
         { 
		    chomp $line;
            my $line2 = $line;
           $line =~ m/^*(\d*)]/;
            my $no = $1;
           $line2 =~ m/:*(\d*e?\d+.?\d*)$/;
            my $crap = $1;
           $routers_last_flit_out_time[$no] = $crap;
         }



#printing interface statistics
         if($line =~ /interface/ && $line =~ /avg_packet_latency/) 
         { 
		    chomp $line;
            my $line2 = $line;
           $line =~ m/^*(\d*)]/;
            my $no = $1;
           $line2 =~ m/:*(\d*e?\d+.?\d*)$/;
            my $crap = $1;
           $interface_pkts_avg_lat[$no] = $crap;
         }
         if($line =~ /interface/ && $line =~ /packets_in/) 
         {
		    chomp $line;
            my $line2 = $line;
           $line =~ m/^*(\d*)]/;
            my $no = $1;
           $line2 =~ m/:*(\d*e?\d+.?\d*)$/;
            my $crap = $1;
             $interface_pkts_in[$no] = $crap;
         }
         if($line =~ /interface/ && $line =~ /packets_out/) 
         {
		    chomp $line;
            my $line2 = $line;
           $line =~ m/^*(\d*)]/;
            my $no = $1;
           $line2 =~ m/:*(\d*e?\d+.?\d*)$/;
            my $crap = $1;
             $interface_pkts_out[$no] = $crap;
         }
         if($line =~ /interface/ && $line =~ /total_packet_latency/) 
         {
		    chomp $line;
            my $line2 = $line;
           $line =~ m/^*(\d*)]/;
            my $no = $1;
           $line2 =~ m/:*(\d*e?\d+.?\d*)$/;
            my $crap = $1;
             $interface_pkts_total_lat[$no] = $crap;
         } 

	 if($line =~ /interface/ && $line =~ /flits_in/) 
         {
		    chomp $line;
            my $line2 = $line;
           $line =~ m/^*(\d*)]/;
            my $no = $1;
           $line2 =~ m/:*(\d*e?\d+.?\d*)$/;
            my $crap = $1;
             $interface_flits_in[$no] = $crap;
         }
         if($line =~ /interface/ && $line =~ /flits_out/) 
         {
		    chomp $line;
            my $line2 = $line;
           $line =~ m/^*(\d*)]/;
            my $no = $1;
           $line2 =~ m/:*(\d*e?\d+.?\d*)$/;
            my $crap = $1;
             $interface_flits_out[$no] = $crap;
         }


  }
  print"\n\n";
  close DAT;


print "Interface Average Packet Latency,";
foreach (@interface_pkts_avg_lat) {
  print "$_,";
}
print "\n";

print "Interface Packets Received,";
foreach (@interface_pkts_in) {
  print "$_,";
}
print "\n";


print "Interface Packets Sent,";
foreach (@interface_pkts_out) {
  print "$_,";
}
print "\n";

print "Interface Total packet latency,";
foreach (@interface_pkts_total_lat) {
  print "$_,";
}
print "\n";


print "Interface Flits In,";
foreach (@interface_flits_in) {
  print "$_,";
}
print "\n";


print "Interface Flits Out,";
foreach (@interface_flits_out) {
  print "$_,";
}
print "\n\n\n";


print "Router packets,";
foreach (@routers_pkts) {
  print "$_,";
}
print "\n";

print "Router Flits,";
foreach (@routers_flits) {
  print "$_,";
}
print "\n";


print "Router Packets latency,";
foreach (@routers_pkts_lat) {
  print "$_,";
}
print "\n";


print "Router Flits per packet,";
foreach (@routers_flits_per_pkt) {
  print "$_,";
}
print "\n";


print "Router Packets Avg latency,";
foreach (@routers_pkts_avg_latency) {
  print "$_,";
}
print "\n";


print "Router Last Flit out time,";
foreach (@routers_last_flit_out_time) {
  print "$_,";
}
print "\n";





#}
