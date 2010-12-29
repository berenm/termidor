/**
 * @file
 * @date Dec 29, 2010
 */

#include "anyterm/terminal.hpp"

#include <iostream>

namespace anyterm {

  static const bool debug_emulation = false;

  // This terminal emulation code attempts to emulate the Linux console, as described in
  // "man 4 console_codes".  This has quite a lot in common with other common terminals
  // (e.g. vt102, xterm etc) but there are always small differences.
  // The starting point for this code is ROTE, and it's because ROTE chose to emulate the
  // Linux console that this does the same.  ROTE's emulation is not, however, very
  // comprehensive.

  // [*] = This code is not implemented, but has been observed in use.

  // We really ought to implement an additional level of character set mode selection.  SI
  // and SO switch between G0 and G1 character sets, and we implement that assuming that G0
  // and G1 have their default meanings.  However, ESC ( and ESC ) can be used to change
  // the meaning of G0 and G1; we don't implement that (but could).  This would make pstree
  // -G work.


  terminal::terminal(int row_count_in, int column_count_in, screen& screen_in) :
    screen_(screen_in), row_count_(row_count_in), column_count_(column_count_in), saved_cursor_row(0),
        saved_cursor_col(0), scrolltop(0), scrollbottom(row_count_in - 1), charset_mode(0), crlf_mode(false),
        dirty(true), esc_state(normal)
  //  , pc850_to_ucs4("CP850", UCS4_NATIVE)
  {
    charset_modes[0] = cs_normal;
    charset_modes[1] = cs_vt100gr;
  }

  terminal::~terminal() {
  }

  int terminal::rows() const {
    return row_count_;
  }

  int terminal::cols() const {
    return column_count_;
  }

  bool terminal::was_dirty(void) {
    bool d = dirty;
    dirty = false;
    return d;
  }

  void terminal::clip_cursor() {
    screen_.cursor_row_number_ = ::std::max(screen_.cursor_row_number_, 0);
    screen_.cursor_row_number_ = ::std::min(screen_.cursor_row_number_, rows() - 1);
    screen_.cursor_column_number_ = ::std::max(screen_.cursor_column_number_, 0);
    screen_.cursor_column_number_ = ::std::min(screen_.cursor_column_number_, cols() - 1);
  }

  void terminal::cursor_line_down() {
    screen_.cursor_row_number_++;
    if (screen_.cursor_row_number_ <= scrollbottom) {
      return;
    }
    screen_.scroll_down(scrolltop, scrollbottom);
    screen_.cursor_row_number_ = scrollbottom;
  }

  void terminal::cursor_line_up() {
    screen_.cursor_row_number_--;
    if (screen_.cursor_row_number_ >= scrolltop) {
      return;
    }
    screen_.scroll_up(scrolltop, scrollbottom);
    screen_.cursor_row_number_ = scrolltop;
  }

  void terminal::write_normal_char(ucs4_char c) {
    switch (charset_modes[charset_mode]) {
      case cs_vt100gr:
        switch (c) {
          case 'j':
            c = L'\u255b';
            break; // lower right corner
          case 'k':
            c = L'\u2510';
            break; // upper right corner
          case 'l':
            c = L'\u250c';
            break; // upper left corner
          case 'm':
            c = L'\u2514';
            break; // lower left corner
          case 'n':
            c = L'\u253c';
            break; // crossing lines
          case 'o':
            c = L'\u2500';
            break; // horizontal line - scan 1
          case 'p':
            c = L'\u2500';
            break; // horizontal line - scan 3
          case 'q':
            c = L'\u2500';
            break; // horizontal line - scan 5
          case 'r':
            c = L'\u2500';
            break; // horizontal line - scan 7
          case 's':
            c = L'\u2500';
            break; // horizontal line - scan 9
          case 't':
            c = L'\u251c';
            break; // left T
          case 'u':
            c = L'\u2524';
            break; // right T
          case 'v':
            c = L'\u2534';
            break; // bottom T
          case 'w':
            c = L'\u252c';
            break; // top T
          case 'x':
            c = L'\u2502';
            break; // vertical bar
        }
        break;
      case cs_pc: {
        ::std::string ch(1, c);
        // FIXME:        pc850_to_ucs4.reset();
        // FIXME:        ucs4_string s = pc850_to_ucs4(ch);
        ucs4_string s;
        s.assign(ch.begin(), ch.end());

        c = s[0];
        break;
      }
      case cs_normal:
        break;
    }

    if (screen_.cursor_column_number_ >= cols()) {
      screen_.cursor_column_number_ = 0;
      cursor_line_down();
    }
    screen_(screen_.cursor_row_number_, screen_.cursor_column_number_) = cell(c, current_attrs);
    screen_.cursor_column_number_++;
  }

  void terminal::carriage_return() {
    screen_.cursor_column_number_ = 0;
  }

  void terminal::line_feed() {
    cursor_line_down();
    if (crlf_mode) {
      carriage_return();
    }
  }

  void terminal::backspace() {
    if (screen_.cursor_column_number_ > 0) {
      screen_.cursor_column_number_--;
    }
  }

  void terminal::tab() {
    do {
      write_normal_char(' ');
    } while (screen_.cursor_column_number_ % 8);
  }

  void terminal::reset() {
    for (int r = 0; r < rows(); ++r) {
      for (int c = 0; c < cols(); ++c) {
        screen_(r, c) = cell();
      }
    }
    screen_.cursor_row_number_ = 0;
    screen_.cursor_column_number_ = 0;
    screen_.cursor_visible_ = true;
    current_attrs = attributes();
    saved_cursor_row = 0;
    saved_cursor_col = 0;
    scrolltop = 0;
    scrollbottom = rows() - 1;
    charset_modes[0] = cs_normal;
    charset_modes[1] = cs_vt100gr;
    charset_mode = 0;
    crlf_mode = false;
  }

  void terminal::csi_SGR() {
    // Set attributes.  The new attributes are applied to the following characters.
    // Set attributes with no parameters means reset attributes to defaults.
    // Otherwise each parameter specifies a separate modification to the attributes.

    if (nparams == 0) {
      current_attrs = attributes();
      return;
    }

    for (int i = 0; i < nparams; ++i) {
      switch (params[i]) {
        case 0:
          current_attrs = attributes();
          break;
        case 1:
          current_attrs.bold_ = true;
          break;
        case 2:
          current_attrs.halfbright_ = true;
          break;
        case 4:
          current_attrs.underlined_ = true;
          break;
        case 5:
          current_attrs.blink_ = true;
          break;
        case 7:
          current_attrs.inverse_ = true;
          break;
          //case 8:  current_attrs.invisible  = true;   break;  (vt100 has this, but not Linux)
        case 10:
          charset_modes[charset_mode] = cs_normal;
          break;
        case 11:
          charset_modes[charset_mode] = cs_pc;
          break;
          //case 12: pc_charset = true; toggle_meta     break;
        case 21:
          current_attrs.bold_ = false;
          break; // Hmm, not sure about these; both
        case 22:
          current_attrs.halfbright_ = false;
          break; // are "set normal intensity".
        case 24:
          current_attrs.underlined_ = false;
          break;
        case 25:
          current_attrs.blink_ = false;
          break;
        case 27:
          current_attrs.inverse_ = false;
          break;
          //case 28: current_attrs.invisible  = false;  break; (as above)
        case 38:
          current_attrs.underlined_ = true;
          current_attrs.foreground_ = 7;
          break;
        case 39:
          current_attrs.underlined_ = false;
          current_attrs.foreground_ = 7;
          break;
        case 49:
          current_attrs.background_ = 0;
          break;

        default:
          if (params[i] >= 30 && params[i] <= 37) {
            current_attrs.foreground_ = params[i] - 30;
          } else if (params[i] >= 40 && params[i] <= 47) {
            current_attrs.background_ = params[i] - 40;
          } else {
            if (debug_emulation) {
              ::std::cout << "Ignoring attribute " << params[i] << "\n";
            }
          }
          break;
      }
    }
  }

  void terminal::csi_SM() {
    // Set modes.
    // Not implemented.
    if (nparams != 1) {
      return;
    }

    switch (params[0]) {
      //case 3: DECCRM mode - display control characters  break;
      //case 4: DECIM mode - insert mode                  break;
      case 20:
        crlf_mode = true;
      default:
        if (debug_emulation) {
          ::std::cout << "Ignoring SM mode " << params[0] << "\n";
        }
    }
  }

  void terminal::csi_RM() {
    // Reset modes.
    // These modes are not implemented, so resetting them does nothing.
    if (nparams != 1) {
      return;
    }

    switch (params[0]) {
      case 3:
        break; // DECCRM mode - display control characters
      case 4:
        break; // DECIM mode - insert mode
      case 20:
        crlf_mode = false;
        break;
      default:
        if (debug_emulation) {
          ::std::cout << "Ignoring RM mode " << params[0] << "\n";
        }
    }
  }

  void terminal::csi_DSR() {
    // Report status.
    // Not implemented.
    if (nparams != 1) {
      return;
    }

    switch (params[0]) {
      //case 5: DSR - device status - reply ESC [ 0 n  break;
      //case 6: CPR - cursor position - reply ESC [ y ; x R  break;
      default:
        if (debug_emulation) {
          ::std::cout << "Ignoring status report request " << params[0] << "\n";
        }
    }
  }

  void terminal::csi_ED() {
    // Erase display.
    int start_row;
    int start_col;
    int end_row;
    int end_col;

    if (nparams > 0 && params[0] == 1) {
      start_row = 0;
      start_col = 0;
      end_row = screen_.cursor_row_number_;
      end_col = screen_.cursor_column_number_;

    } else if (nparams > 0 && params[0] == 2) {
      start_row = 0;
      start_col = 0;
      end_row = rows() - 1;
      end_col = cols() - 1;

    } else {
      start_row = screen_.cursor_row_number_;
      start_col = screen_.cursor_column_number_;
      end_row = rows() - 1;
      end_col = cols() - 1;
    }

    for (int r = start_row; r <= end_row; ++r) {
      for (int c = (r == start_row ? start_col : 0); c <= (r == end_row ? end_col : cols() - 1); ++c) {
        screen_(r, c) = cell(' ', current_attrs);
      }
    }
  }

  void terminal::csi_CUP() {
    // Move cursor to absolute position.
    // With no parameters, move to origin.
    if (nparams == 0) {
      screen_.cursor_row_number_ = 0;
      screen_.cursor_column_number_ = 0;
    } else {
      screen_.cursor_row_number_ = params[0] - 1;
      screen_.cursor_column_number_ = params[1] - 1;
      clip_cursor();
    }
  }

  void terminal::csi_HVP() {
    csi_CUP();
  }

  void terminal::csi_CUU() {
    // Cursor Up.
    int n = params[0];
    screen_.cursor_row_number_ -= n;
    clip_cursor();
  }

  void terminal::csi_CUD() {
    // Cursor Down.
    int n = params[0];
    screen_.cursor_row_number_ += n;
    clip_cursor();
  }

  void terminal::csi_VPR() {
    csi_CUD();
  }

  void terminal::csi_CUF() {
    // Cursor Forward (right).
    int n = params[0];
    screen_.cursor_column_number_ += n;
    clip_cursor();
  }

  void terminal::csi_HPR() {
    csi_CUF();
  }

  void terminal::csi_CUB() {
    // Cursor Back (left).
    int n = params[0];
    screen_.cursor_column_number_ -= n;
    clip_cursor();
  }

  void terminal::csi_CNL() {
    // Cursor next line.
    csi_CUD();
    screen_.cursor_column_number_ = 0;
    clip_cursor();
  }

  void terminal::csi_CPL() {
    // Cursor previous line.
    csi_CUU();
    screen_.cursor_column_number_ = 0;
    clip_cursor();
  }

  void terminal::csi_CHA() {
    screen_.cursor_column_number_ = params[0] - 1;
    clip_cursor();
  }

  void terminal::csi_HPA() {
    csi_HPA();
  }

  void terminal::csi_VPA() {
    screen_.cursor_row_number_ = params[0] - 1;
    clip_cursor();
  }

  void terminal::csi_EL() {
    // Erase line.
    int start;
    int end;

    if (nparams > 0 && params[0] == 1) {
      start = 0;
      end = screen_.cursor_column_number_;

    } else if (nparams > 0 && params[0] == 2) {
      start = 0;
      end = cols() - 1;

    } else {
      start = screen_.cursor_column_number_;
      end = cols() - 1;
    }

    for (int i = start; i <= end; ++i) {
      screen_(screen_.cursor_row_number_, i) = cell(' ', current_attrs);
    }
  }

  void terminal::csi_ICH() {
    // Insert blanks.
    int n = params[0];
    for (int i = cols() - 1; i >= screen_.cursor_column_number_ + n; --i) {
      screen_(screen_.cursor_row_number_, i) = screen_(screen_.cursor_row_number_, i - n);
    }
    for (int i = screen_.cursor_column_number_; i < screen_.cursor_column_number_ + n; ++i) {
      screen_(screen_.cursor_row_number_, i) = cell(' ', current_attrs);
    }
  }

  void terminal::csi_DCH() {
    // Delete Characters.
    int n = params[0];
    for (int i = screen_.cursor_column_number_; i < cols() - n; ++i) {
      screen_(screen_.cursor_row_number_, i) = screen_(screen_.cursor_row_number_, i + n);
    }
    for (int i = cols() - n; i < cols(); ++i) {
      screen_(screen_.cursor_row_number_, i) = cell(' ', current_attrs);
    }
  }

  void terminal::csi_IL() {
    // Insert Line.
    int n = params[0];
    screen_.scroll_up(screen_.cursor_row_number_, scrollbottom, n);
  }

  void terminal::csi_DL() {
    // Delete Line.
    int n = params[0];
    screen_.scroll_down(screen_.cursor_row_number_, scrollbottom, n);
  }

  void terminal::csi_ECH() {
    // Erase characters.
    int n = params[0];
    for (int i = screen_.cursor_column_number_; i < screen_.cursor_column_number_ + n && i < cols(); ++i) {
      screen_(screen_.cursor_row_number_, i) = cell(' ', current_attrs);
    }
  }

  void terminal::csi_DECSTBM() {
    // Set scrolling region.
    int newtop;
    int newbottom;

    if (nparams == 0) {
      newtop = 0;
      newbottom = rows() - 1;

    } else if (nparams < 2) {
      return;

    } else {
      newtop = params[0] - 1;
      newbottom = params[1] - 1;
    }

    newtop = ::std::max(newtop, 0);
    newtop = ::std::min(newtop, rows() - 1);
    newbottom = ::std::max(newbottom, 0);
    newbottom = ::std::min(newbottom, rows() - 1);

    if (newtop > newbottom) {
      return;
    }

    scrolltop = newtop;
    scrollbottom = newbottom;
  }

  void terminal::csi_SAVECUR() {
    // Save cursor position.
    saved_cursor_row = screen_.cursor_row_number_;
    saved_cursor_col = screen_.cursor_column_number_;
  }

  void terminal::csi_RESTORECUR() {
    // Restore cursor position.
    screen_.cursor_row_number_ = saved_cursor_row;
    screen_.cursor_column_number_ = saved_cursor_col;
  }

  void terminal::csi_DECSET() {
    if (nparams != 1) {
      return;
    }

    switch (params[0]) {
      //case 1:    Change cursor key prefix    break;
      //case 3:    80/132-column mode          break;
      //case 5:    Reverse video mode          break;
      //case 6:    Scroll-region-relative cursor addressing mode  break;
      //case 7:    Autowrap mode               break; [*]
      //case 8:    Autorepeat                  break;
      //case 9:    X10 mouse reporting         break;
      case 25:
        screen_.cursor_visible_ = true;
        break;
        //case 1000: X11 mouse reporting         break;
      default:
        if (debug_emulation) {
          ::std::cout << "Ignoring DECSET " << params[0] << "\n";
        }
    }
  }

  void terminal::csi_DECRST() {
    if (nparams != 1) {
      return;
    }

    switch (params[0]) {
      //case 1:    Change cursor key prefix     break;
      //case 3:    80/132-column mode           break;
      //case 5:    Reverse video mode           break;
      //case 6:    Scroll-region-relative cursor addressing mode  break;
      //case 7:    Autowrap mode                break;
      //case 8:    Autorepeat                   break;
      //case 9:    X10 mouse reporting          break;
      case 25:
        screen_.cursor_visible_ = false;
        break;
        //case 1000: X11 mouse reporting          break;
      default:
        if (debug_emulation) {
          ::std::cout << "Ignoring DECSET " << params[0] << "\n";
        }
    }
  }

  void terminal::write_char(ucs4_char c) {
    if (c <= 31) {
      switch (c) {
        case '\a': /* bell */
          break;
        case '\b':
          backspace();
          break;
        case '\t':
          tab();
          break;
        case '\n': /* fall through */
        case '\v': /* fall through */
        case '\f':
          line_feed();
          break;
        case '\r':
          carriage_return();
          break;
        case '\x0E':
          charset_mode = 1;
          break;
        case '\x0F':
          charset_mode = 0;
          break;
        case '\x18': /* fall through */
        case '\x1A':
          esc_state = normal;
          break;
        case '\x1B':
          esc_state = seen_esc;
          break;
        default:
          if (debug_emulation) {
            ::std::cout << "Ignoring ctrl character " << static_cast< unsigned int > (c) << "\n";
            break;
          }
      }
      dirty = true;

    } else if (c == 0x9b) {
      // Is there a conflict between 9b==CSI and a UTF8 or ISO-8859 interpretation?
      esc_state = seen_csi;
      nparams = 0;
      params[0] = 1;

    } else {
      switch (esc_state) {
        case normal:
          write_normal_char(c);
          dirty = true;
          break;

        case seen_esc:
          switch (c) {
            case 'c':
              reset();
              dirty = true;
              esc_state = normal;
              break;
            case 'D':
              line_feed();
              dirty = true;
              esc_state = normal;
              break;
            case 'E':
              carriage_return();
              dirty = true;
              esc_state = normal;
              break;
              //case 'H': set_tab_stop();                esc_state=normal; break; [*]
            case 'M':
              cursor_line_up();
              dirty = true;
              esc_state = normal;
              break;
              //case 'Z': dec_priv_ident();              esc_state=normal; break; // kernel returns ESC [ ? 6 c
              //case '7': save_state();                  esc_state=normal; break; // save cursor pos, attributes, charsets
              //case '8': restore_state();   dirty=true; esc_state=normal; break;
            case '[':
              esc_state = seen_csi;
              nparams = 0;
              params[0] = 1;
              break;
              //case '%':                                esc_state=seen_escpercent; break;  // select character set based on next char
              // @=8859-1, G=8=UTF-8
              //case '#':                                esc_state=seen_eschash; break;     // ESC # 8 = fill screen with Es
            case '(':
              esc_state = seen_esclparen;
              break; // select G0 charset based on next char
            case ')':
              esc_state = seen_escrparen;
              break; // select G1 charset based on next char
              //case '>': numeric_keypad_mode();         esc_state=normal; break;
              //case '=': application_keypad_mode();     esc_state=normal; break;
              //case ']':                                esc_state=seen_escrbraket; break;
              // ESC ] P nrrggbb = set palette; colour n (hex)
              // ESC ] R = reset palette [*]
            default:
              if (debug_emulation) {
                ::std::cout << "Ignoring esc character " << c << "\n";
              }
              esc_state = normal;
          }
          break;

        case seen_csi: /* fall through */
        case seen_csi_private:
          if (c >= '0' && c <= '9') {
            if (nparams == 0) {
              nparams = 1;
              params[0] = 0;
            }
            params[nparams - 1] = params[nparams - 1] * 10 + (c - '0');

          } else if (c == ';') {
            if (nparams >= nparams_max) {
              return;
            }
            nparams++;
            params[nparams - 1] = 0;

          } else if (c == '?') {
            esc_state = seen_csi_private;

          } else {
            if (esc_state == seen_csi_private) {
              switch (c) {
                //case 'c': Unknown; code seen but not described in 'man console_codes' [*]
                case 'h':
                  csi_DECSET();
                  break;
                case 'l':
                  csi_DECRST();
                  break;
                default:
                  if (debug_emulation) {
                    ::std::cout << "Ignoring private csi character " << c << "\n";
                  }
              }
            } else {
              switch (c) {
                case '@':
                  csi_ICH();
                  break;
                case 'A':
                  csi_CUU();
                  break;
                case 'B':
                  csi_CUD();
                  break;
                case 'C':
                  csi_CUF();
                  break;
                case 'D':
                  csi_CUB();
                  break;
                case 'E':
                  csi_CNL();
                  break;
                case 'F':
                  csi_CPL();
                  break;
                case 'G':
                  csi_CHA();
                  break;
                case 'H':
                  csi_CUP();
                  break;
                case 'J':
                  csi_ED();
                  break;
                case 'K':
                  csi_EL();
                  break;
                case 'L':
                  csi_IL();
                  break;
                case 'M':
                  csi_DL();
                  break;
                case 'P':
                  csi_DCH();
                  break;
                case 'X':
                  csi_ECH();
                  break;
                case 'a':
                  csi_HPR();
                  break;
                  //case 'c': csi_DA();  break;  // Reply ESC [ ? 6 c
                case 'd':
                  csi_VPA();
                  break;
                case 'e':
                  csi_VPR();
                  break;
                case 'f':
                  csi_HVP();
                  break;
                  //case 'g': csi_TBC(); break;  // Clear tab stop [*]
                case 'h':
                  csi_SM();
                  break;
                case 'l':
                  csi_RM();
                  break;
                case 'm':
                  csi_SGR();
                  break;
                case 'n':
                  csi_DSR();
                  break;
                  //case 'q': csi_DECLL(); break; // Set keyboard LEDs
                case 'r':
                  csi_DECSTBM();
                  break;
                case 's':
                  csi_SAVECUR();
                  break;
                case 'u':
                  csi_RESTORECUR();
                  break;
                case '`':
                  csi_HPA();
                  break;
                default:
                  if (debug_emulation) {
                    ::std::cout << "Ignoring csi character " << c << "\n";
                  }
              }
            }
            dirty = true;
            esc_state = normal;

          }
          break;

        case seen_esclparen: /* fall through */
        case seen_escrparen: {
          charset_mode_t m = cs_normal;
          switch (c) {
            case 'B':
              m = cs_normal;
              break;
            case '0':
              m = cs_vt100gr;
              break;
            case 'U':
              m = cs_pc;
              break;
          }
          if (esc_state == seen_esclparen) {
            charset_modes[0] = m;
          } else {
            charset_modes[1] = m;
          }
          esc_state = normal;
          break;
        }
      }
    }
  }

  void terminal::write(ucs4_string data) {
    for (size_t i = 0; i < data.length(); ++i) {
      write_char(data[i]);
    }
  }

} // namespace anyterm
