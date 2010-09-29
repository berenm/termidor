#!/bin/bash

function make_table
{
  echo -n "  {"
  awk 'BEGIN { L = -1; }
       !/^#/ && !/^ *$/ {
               N = $1+0; U = $2;
               if (N < L) { print "Table is out of order"; exit(1); }
               while (L<N-1) { L++; if (L>='$2') printf(" -1,"); }
               if (N<'$2') {
                 if (U+0 != N) {
                   print "expected 1-1 mapping does not hold for",N,U; exit(1);
                 }
               } else {
                 printf(" %s,",U);
               }
               L = N;
             }' < $1
  echo -n "}"
}


echo "iso8859_table_t iso8859_tables[] = {"

for n in 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16
do
  if [ $n == 12 ]
  then
    echo "  // ISO-8859-12 does not exist"
    echo "  {},"
  else
    echo "  // ISO-8859-${n}"
    make_table "tables/ISO8859/8859-${n}.TXT" 161
    echo ","
  fi
done

echo "};"



echo "macintosh_table_t macintosh_table = "
make_table "tables/VENDORS/APPLE/ROMAN.TXT" 128
echo ";"


