#!/usr/bin/perl

%comps = ();
$comps{"fetch"}  = "ZPIPE-fetch";
$comps{"decode"}  = "ZPIPE-decode";
$comps{"alloc"}  = "ZPIPE-alloc";
$comps{"exec"}  = "ZPIPE-exec";
$comps{"commit"}  = "ZPIPE-commit";

$comps{"bpred"}  = "ZCOMPS-bpred,ZCOMPS-fusion,ZCOMPS-btb,ZCOMPS-ras";
$comps{"memdep"}  = "ZCOMPS-memdep";
$comps{"prefetch"}  = "ZCOMPS-prefetch";
$comps{"dram"}  = "ZCOMPS-dram";
$comps{"MC"}  = "ZCOMPS-MC";

$component = $ARGV[0];

unless(defined $component) { die "must specify a component name"; }
unless(defined $comps{$component}) { die "unknown component name: $component"; }

@comp_list = split(',',$comps{$component});

while($comp = shift @comp_list)
{
  $comp_age = -M $pref.".list";
  $dir_age = -M $comp;

  if(-e $comp."-list.h" and $dir_age < $comp_age)
  {
    # object file has been made since directory modified
    print "Skipping $comp.list\n";
    next;
  }

  print "Generating $comp.list\n";

  $files = `ls -1 $comp/*.c*`;
  @files = split(/\s/,$files);

  unless (open (OUTF,">$comp.list"))
    { die "couldn't write to $comp.list"; }

  print OUTF "/* DO NOT EDIT THIS FILE.  This file was automatically\n";
  print OUTF "   generated by make_def_lists.pl. */\n\n";

  foreach $file (@files)
  {
    print OUTF "#include \"$file\"\n";
  }

  close OUTF;

}
