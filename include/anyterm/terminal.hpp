/**
 * @file
 * @date Dec 15, 2010
 */

#ifndef ANYTERM_TERMINAL_HPP_
#define ANYTERM_TERMINAL_HPP_

#include "anyterm/cell.hpp"
#include "anyterm/attributes.hpp"
#include "anyterm/screen.hpp"
#include "unicode.hh"
//#include "Iconver.hh"

#include <string>

namespace anyterm {

  class terminal {
    public:
      terminal(int row_count_in, int column_count_in, screen& screen_in);
      ~terminal();

      int rows() const;
      int cols() const;

      screen& screen_;

      bool was_dirty();

      void write(ucs4_string data);

    private:
      int row_count_;
      int column_count_;

      int saved_cursor_row;
      int saved_cursor_col;

      int scrolltop;
      int scrollbottom;

      attributes current_attrs;

      enum charset_mode_t {
        cs_normal, cs_pc, cs_vt100gr
      };
      charset_mode_t charset_modes[2];
      int charset_mode;

      bool crlf_mode;

      bool dirty;

      int nparams;
      static const int nparams_max = 16;
      int params[nparams_max];

      enum esc_state_t {
        normal, seen_esc, seen_csi, seen_csi_private, seen_esclparen, seen_escrparen
      };
      esc_state_t esc_state;

      //      pbe::Iconver< pbe::permissive, char, ucs4_char > pc850_to_ucs4;

      void clip_cursor();
      void cursor_line_down();
      void cursor_line_up();
      void write_normal_char(ucs4_char c);
      void carriage_return();
      void line_feed();
      void backspace();
      void tab();
      void reset();
      void csi_SGR();
      void csi_SM();
      void csi_RM();
      void csi_DSR();
      void csi_ED();
      void csi_CUP();
      void csi_HVP();
      void csi_CUU();
      void csi_CUD();
      void csi_VPR();
      void csi_CUF();
      void csi_HPR();
      void csi_CUB();
      void csi_CNL();
      void csi_CPL();
      void csi_CHA();
      void csi_HPA();
      void csi_VPA();
      void csi_EL();
      void csi_ICH();
      void csi_DCH();
      void csi_IL();
      void csi_DL();
      void csi_ECH();
      void csi_DECSTBM();
      void csi_SAVECUR();
      void csi_RESTORECUR();
      void csi_DECSET();
      void csi_DECRST();
      void write_char(ucs4_char c);
  };

} // namespace anyterm

#endif /* ANYTERM_TERMINAL_HPP_ */
