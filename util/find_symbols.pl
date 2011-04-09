#!/usr/bin/env perl

$file = shift;

@lines = `objdump -t $file | grep "\.text"`;
foreach(@lines)
{
	next unless /^([0-9a-f]+) [gl].* (.+)$/;
	print "$1 $2\n";
}