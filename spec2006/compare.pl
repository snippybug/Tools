#!/usr/bin/env perl

use warnings;

if (@ARGV != 2) {
    die "Usage: compare.pl ref-file obj-file\n";
}

my $ref_file;
my $ref_cycles;
my %ref_hash;

open $ref_file, '<', $ARGV[0] or die "$!";
# Read reference file
while (<$ref_file>) {
    if (/# Event count.*: (\d+)/) {
        $ref_cycles = $1;
    }
    elsif (/[\d.]+%/) {
        my @fields = split;
        $_ = $fields[0];
        # 'Symbol' is key, and 'Overhead' is value
        $ref_hash{$fields[-1]} = s|(\d+.\d+)%|$1/100|er;
    }
}
close($ref_file);

my $obj_file;
my $obj_cycles;

open $obj_file, '<', $ARGV[1] or die "$!";
# Read object file
while (<$obj_file>) {
    if (/# Event count.*: (\d+)/) {
        $obj_cycles = $1;
    }
    elsif (/[\d.]+%/) {
        my @fields = split;
        my $overhead;
        $_ = $fields[0];
        $overhead = s|(\d+.\d+)%|$1/100|er;
        if ($overhead > 0.05) {
            # Overhead that is greater than 5% is interesting
            my $cur_cycle = $overhead * $obj_cycles;
            my $ref_cycle = $ref_hash{$fields[-1]} * $ref_cycles;
            if (($cur_cycle - $ref_cycle) > $ref_cycle * 0.5) {
                # > 50%
                #print("$fields[0]\t$ref_cycle\t$cur_cycle\t$fields[-1]\n");
                printf("%s\t%d\t%d\t%s\n", $fields[0], int($ref_cycle), int($cur_cycle), $fields[-1]);
            }
        }
    }
}
close($obj_file);
