#!/bin/csh -f

     set dir = /data/avgbrain1/atlas/talairach/obj

     foreach direction ( sag cor trans )
         pushd $dir >&! /dev/null
         set files = ( ${direction}* )
         popd >&! /dev/null
         foreach file ( $files )
             set slice = ( `echo $file | sed -e s/${direction}_// -e s/_128\.\*// -e s/_256\.\*// -e s/_512\.\*//` )
             if( $direction == "sag" ) set axis = x
             if( $direction == "cor" ) set axis = y
             if( $direction == "trans" ) set axis = z
             set filename = $file
             if( ${file:e} != "obj" ) set filename = ${file:r}
             echo $filename $axis $slice
         end
     end
