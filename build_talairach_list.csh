#!/bin/csh -f

     set dir = /data/avgbrain1/atlas/talairach/obj

     foreach direction ( sag cor trans )
         pushd $dir >&! /dev/null
         set files = ( ${direction}* )
         popd >&! /dev/null

         set positions =

         foreach file ( $files )
             set position = ( `echo $file | sed -e s/${direction}_// -e s/_128\.\*// -e s/_256\.\*// -e s/_512\.\*//` )
             set positions = ($positions $position)
         end

         set sorted_positions = (`echo_arguments_one_per_line.csh $positions | sort -u -n`)

         foreach position ( $sorted_positions )
             pushd $dir >&! /dev/null
             set position_files = ( ${direction}_${position}_* )
             popd >&! /dev/null

             foreach file ( $position_files )
                 if( $direction == "sag" ) set axis = x
                 if( $direction == "cor" ) set axis = y
                 if( $direction == "trans" ) set axis = z
                 set filename = $file
                 if( ${file:e} == "Z" )  set filename = ${file:r}
                 echo $filename $axis $position
             end
         end
     end
