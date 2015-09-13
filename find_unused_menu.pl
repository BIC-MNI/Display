#!/usr/local/bin/perl

    $menu = "menu/Display.menu.include";
    $input = "menu/input_menu.c";

    $list = `grep MENU_F $input`;

    foreach $string ( split( '^', $list ) )
    {
        chop $string;
        $string =~ s/.*\(//;
        $string =~ s/\).*//;
        $string =~ s/\s*//g;
        $out = `grep -l $string $menu`;
        if( !$out )
            { print( "$string\n" ); }
    }
