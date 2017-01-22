#!/usr/bin/perl -w
use POSIX;

my $title = "styles";
my $stylesdir = "/usr/share/meawm/styles";
my $homestylesdir = "$ENV{HOME}/.meawm/styles";
@Styles = ();

if (! $ARGV[0]) {
    print_directory($stylesdir);
    print_directory($homestylesdir);
}
else {
    print_directory($ARGV[0]);
}

if (@Styles) {
#    print "[title] ($title)\n";
    foreach $Style (sort {$a cmp $b} @Styles) {
       print $Style;
    }
}                                                                     

sub print_directory {
	my $mode;
	my $temp;
	my @dir_list;

	opendir(DIR, $_[0]) || die "can't opendir $_[0]: $!";
	@dir_list = grep { /^[^\.].*/ || /^\.\..*/} readdir(DIR);
	closedir DIR;

	while ($temp = shift @dir_list) {
		$mode = (stat("$_[0]\/$temp"))[2];
		if (!($temp eq '..' || $temp eq '.')) {
			if (S_ISDIR($mode)) {
				print_directory("$_[0]\/$temp");
			} else {
				if (S_ISREG($mode)) {
					my $name;
					if ($temp eq 'stylerc') {
						$name = $_[0];
						$name =~ s/\/+$//;
						$name =~ s/^.*\///;
					} elsif ($temp =~ /(.+)\.style$/) {
						$name = $1;
					}
					if ($name) {
						$Item = "[item] ($name) \"restart(meawm --stylefile=" .
							"$_[0]\/$temp)\"\n";
						push(@Styles, $Item);
					}
				}
			}
		}
	}
	return 0;
}
