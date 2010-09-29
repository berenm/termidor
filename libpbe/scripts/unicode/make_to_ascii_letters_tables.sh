# scripts/unicode/make_to_ascii_letters_tables.sh
# This file is part of libpbe; see http://svn.chezphil.org/libpbe/
# (C) 2008 Philip Endecott

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

# Download the Unicode character database:
# wget http://www.unicode.org/Public/5.1.0/ucd/UnicodeData.txt

# Extract the decompositions:
cat UnicodeData.txt |
awk -F ';' '$6!="" {print $1,$6}' |
awk '/</ {printf("%s ",$1); for(i=3;i<=NF;i++) printf("%s ",$i); printf("\n");}
     !/</ {print}' > decompositions

# Each line in decompositions starts with the source character and is followed by the
# sequence of characters that it may be replaced by.  This replacement needs to be
# applied recursively:
while true
do

awk '{if (FNR==NR) {d=""; for(i=2;i<=NF;i++) {d = d $i " "}; dec[$1]=d;}
              else {printf("%s ",$1);
                    for(i=2;i<=NF;i++) {
                      if ($i in dec) {printf("%s",dec[$i]);} else {printf("%s ",$i);}
                    } 
                    printf("\n");}
     }' decompositions decompositions > new_decompositions

if cmp decompositions new_decompositions
then
  break
fi

mv new_decompositions decompositions

done

mv new_decompositions full_decompositions


########


# For the puposes of converting strings to text searchable ASCII equivalents, we're only
# interested in determining the ASCII letter base characters.  Strip all the other characters
# in the expansion, and if none are left remove the line.

awk '{printf("%s ",$1);
      for(i=2;i<=NF;i++) {
        n=strtonum("0x" $i);
        if (n>=65 && n<=90 || n>=97 && n<=122)
          printf("%s ",$i)
        }
        printf("\n");
     }' full_decompositions |
awk '{if (NF>1) print}' |


# Now convert the letter codes to actual letters, and convert to lower case.

awk '{printf("%s \"",$1);
      for(i=2;i<=NF;i++) {
        v=strtonum("0x" $i);
        c=sprintf("%c",v);
        printf("%c",tolower(c));
      } 
      printf("\"\n");
     }' |

# Add 'identity' conversions for a-z and 'tolower' conversions for A-Z.

awk 'BEGIN {for(i=65;i<=90;i++) printf("%04X \"%c\"\n",i,i+32);
            for(i=97;i<=122;i++) printf("%04X \"%c\"\n",i,i);}
     {print}' > all_decompositions


# Add some more local decompositions.

cat local_decompositions >>all_decompositions


# Split it up into one file per unicode page

rm *.d0
awk '{page=substr($1,1,length($1)-2); print>(page ".d0")}' all_decompositions

# Reformat them as struct initialisers:

for i in *.d0
do

n=`basename $i .d0`

echo -n "char_expansion_page_${n}_t to_ascii_letters_page_${n} [] = {";

awk '{n=strtonum("0x" $1); a[n%256]=$2;}
 END {for(i=0;i<256;i++) {
        if (i in a) printf("%s, ",a[i]); else printf("\"\", ");
      }
      printf("};\n")
     }' $i

done > to_ascii_letters_tables.cc


########


# This is similar to the preceeding section, except that digits are also retained.
# Strip all the other characters in the expansion, and if none are left remove the line.

awk '{printf("%s ",$1);
      for(i=2;i<=NF;i++) {
        n=strtonum("0x" $i);
        if (n>=65 && n<=90 || n>=97 && n<=122 || n>=48 && n<=57)
          printf("%s ",$i)
        }
        printf("\n");
     }' full_decompositions |
awk '{if (NF>1) print}' |


# Now convert the alnum codes to actual alnums, and convert letters to lower case.

awk '{printf("%s \"",$1);
      for(i=2;i<=NF;i++) {
        v=strtonum("0x" $i);
        c=sprintf("%c",v);
        printf("%c",tolower(c));
      } 
      printf("\"\n");
     }' |

# Add 'identity' conversions for a-z and 0-9 and 'tolower' conversions for A-Z.

awk 'BEGIN {for(i=65;i<=90;i++)  printf("%04X \"%c\"\n",i,i+32);
            for(i=97;i<=122;i++) printf("%04X \"%c\"\n",i,i);
            for(i=48;i<=57;i++)  printf("%04X \"%c\"\n",i,i);}
     {print}' > all_decompositions


# Add some more local decompositions.

cat local_decompositions >>all_decompositions


# Split it up into one file per unicode page

rm *.d0
awk '{page=substr($1,1,length($1)-2); print>(page ".d0")}' all_decompositions

# Reformat them as struct initialisers:

for i in *.d0
do

n=`basename $i .d0`

echo -n "char_expansion_page_${n}_t to_ascii_alnums_page_${n} [] = {";

awk '{n=strtonum("0x" $1); a[n%256]=$2;}
 END {for(i=0;i<256;i++) {
        if (i in a) printf("%s, ",a[i]); else printf("\"\", ");
      }
      printf("};\n")
     }' $i

done > to_ascii_alnums_tables.cc


########


# For conversion to ASCII for approximation of missing characters when recoding, we're
# interested in any printable ASCII characters in the decomposition.

awk '{printf("%s ",$1);
      for(i=2;i<=NF;i++) {
        n=strtonum("0x" $i);
        if (n>=32 && n<127)
          printf("%s ",$i)
        }
        printf("\n");
     }' full_decompositions |
awk '{if (NF>1) print}' |


# Now convert the character codes to actual characters.

awk '{printf("%s \"",$1);
      for(i=2;i<=NF;i++) {
        v=strtonum("0x" $i);
        printf("%c",v);
      } 
      printf("\"\n");
     }' |

# Add 'identity' conversions for all (printable) ASCII characters

awk 'BEGIN {for(i=32;i<127;i++) printf("%04X \"%c\"\n",i,i);}
     {print}' > all_decompositions


# Add some more local decompositions.

cat local_decompositions >>all_decompositions


# Split it up into one file per unicode page

rm *.d1
awk '{page=substr($1,1,length($1)-2); print>(page ".d1")}' all_decompositions


# Reformat them as struct initialisers:

for i in *.d1
do

n=`basename $i .d1`

echo -n "ascii_char_expansion_page_${n}_t to_ascii_page_${n} [] = {";

awk '{n=strtonum("0x" $1); a[n%256]=substr($0,6);}
 END {for(i=0;i<256;i++) {
        if (i in a) printf("%s, ",a[i]); else printf("\"\", ");
      }
      printf("};\n")
     }' $i

done |

# Munge " and \:

sed 's/"\(["\]\)"/"\\\1"/g' > to_ascii_tables.cc


