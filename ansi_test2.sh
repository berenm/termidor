#!/bin/bash
#
#  colors v1.03
#
#  A simple shell script to output an ANSI terminal color chart.
#  It may be useful when trying to customize your ANSI terminal
#  color scheme!
#
#  Written and placed in the public domain by Ian Abbott <ian@abbott.org>
#
#  ANSI terminal color chart generator
#  Author: Ian Abbott
#  License: BSD License, http://www.opensource.org/licenses/bsd-license.php
#  URL: http://www.snippetcenter.org/de/ansi-terminal-color-chart-generator-s1346.aspx

for h in 0 1; do
  echo -e "\\033[0;${h}m\\c"
  case $h in
  0) echo -e "Normal\\c" ;;
  1) echo -e "High\\c" ;;
  esac
  echo -e " intensity foreground (background color in parentheses)\\033[m"
  for f in 0 1 2 3 4 5 6 7; do
    for b in 0 1 2 3 4 5 6 7 8; do
      echo -e "\\033[${h};3${f}\\c"
      if [ $b -lt 8 ]; then
        echo -e ";4${b}m\\c"
      else
        echo -e "m\\c"
      fi
      case $f in
      0) echo -e " BLACK   \\c" ;;
      1) echo -e " RED     \\c" ;;
      2) echo -e " GREEN   \\c" ;;
      3) echo -e " YELLOW  \\c" ;;
      4) echo -e " BLUE    \\c" ;;
      5) echo -e " MAGENTA \\c" ;;
      6) echo -e " CYAN    \\c" ;;
      7) echo -e " WHITE   \\c" ;;
      esac
      case $b in
      8) echo -e "\\033[m" ;;
      *) echo -e "\\033[m \\c" ;;
      esac
    done
  done
  echo -e "\\033[${h}m\\c"
  for b in 0 1 2 3 4 5 6 7 8; do
    case $b in
    0) echo -e "(black)   \\c" ;;
    1) echo -e "(red)     \\c" ;;
    2) echo -e "(green)   \\c" ;;
    3) echo -e "(yellow)  \\c" ;;
    4) echo -e "(blue)    \\c" ;;
    5) echo -e "(magenta) \\c" ;;
    6) echo -e "(cyan)    \\c" ;;
    7) echo -e "(white)   \\c" ;;
    8) echo -e "(none)    \\c" ;;
    esac
  done
  echo -e "\\033[m\\n"
done
