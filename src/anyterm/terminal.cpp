/**
 * @file
 * @date Dec 29, 2010
 */

#include "anyterm/terminal.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/thread.hpp>

#include <iostream>
#include <iomanip>

namespace anyterm {

  struct palette {
      GdkColor colors[255];
      GdkColor default_fg_color;
      GdkColor default_bg_color;
      GdkColor bold_fg_color;
      GdkColor dim_fg_color;
      GdkColor cursor_bg_color;
      GdkColor highlight_bg_color;

      palette() :
        default_fg_color( { 0, 0, 0 }), default_bg_color( { 255, 255, 255 }), bold_fg_color( { 1, 0, 0 }),
            dim_fg_color( { 0, 1, 0 }), cursor_bg_color( { 254, 255, 255 }), highlight_bg_color( { 255,
                                                                                                   254,
                                                                                                   255 }) {
        int offset = 0;
        for (int color = 0; color < 255; ++color) {
          offset += ((color % 3) == 0 ? 1 : 0);
          colors[color].red = offset + ((color % 3) == 0 ? 1 : 0);
          colors[color].green = offset + ((color % 3) == 1 ? 1 : 0);
          colors[color].blue = offset + ((color % 3) == 2 ? 1 : 0);
        }
      }

      void apply(GtkWidget* terminal_inout) {
        vte_terminal_set_colors(VTE_TERMINAL(terminal_inout), NULL, NULL, colors, 255);
        vte_terminal_set_color_bold(VTE_TERMINAL(terminal_inout), &bold_fg_color);
        vte_terminal_set_color_foreground(VTE_TERMINAL(terminal_inout), &default_fg_color);
        vte_terminal_set_color_background(VTE_TERMINAL(terminal_inout), &default_bg_color);
        vte_terminal_set_color_dim(VTE_TERMINAL(terminal_inout), &dim_fg_color);
        vte_terminal_set_color_cursor(VTE_TERMINAL(terminal_inout), &cursor_bg_color);
        vte_terminal_set_color_highlight(VTE_TERMINAL(terminal_inout), &highlight_bg_color);
      }

      ::std::string get_color(GdkColor color) {
        if (gdk_color_equal(&color, &default_fg_color)) {
          return "def_fg";
        }
        if (gdk_color_equal(&color, &default_bg_color)) {
          return "def_bg";
        }
        if (gdk_color_equal(&color, &bold_fg_color)) {
          return "def_bold";
        }
        if (gdk_color_equal(&color, &dim_fg_color)) {
          return "def_dim";
        }
        if (gdk_color_equal(&color, &cursor_bg_color)) {
          return "cursor_bg";
        }
        if (gdk_color_equal(&color, &highlight_bg_color)) {
          return "highlight_bg";
        }
        for (int i = 0; i < 255; ++i) {
          if (gdk_color_equal(&color, &colors[i])) {
            return ::boost::lexical_cast< ::std::string >(i);
          }
        }

        return "unknown";
      }
  };

  terminal::terminal(::std::uint8_t const row_count_in,
                     ::std::uint8_t const column_count_in,
                     ::std::uint16_t const scrollback_count_in) {
    ::std::clog << "terminal()" << ::std::endl;
    __terminal = vte_terminal_new();

    //    g_timeout_add(250, check_terminated_static, this);

    //    g_signal_connect(G_OBJECT(__terminal), "eof", G_CALLBACK(gtk_main_quit), NULL);
    //    g_signal_connect(G_OBJECT(__terminal), "encoding-changed", G_CALLBACK(gtk_main_quit), NULL);
    //    g_signal_connect(G_OBJECT(__terminal), "emulation-changed", G_CALLBACK(gtk_main_quit), NULL);
    //    g_signal_connect(G_OBJECT(__terminal), "contents-changed", G_CALLBACK(gtk_main_quit), NULL);
    //    g_signal_connect(G_OBJECT(__terminal), "commit", G_CALLBACK(gtk_main_quit), NULL);
    //    g_signal_connect(G_OBJECT(__terminal), "child-exited", G_CALLBACK(gtk_main_quit), NULL);
    //    g_signal_connect(G_OBJECT(__terminal), "beep", G_CALLBACK(gtk_main_quit), NULL);

    set_size(row_count_in, column_count_in);
    set_scrollback_size(scrollback_count_in);

    vte_terminal_set_audible_bell(VTE_TERMINAL(__terminal), false);
    vte_terminal_set_visible_bell(VTE_TERMINAL(__terminal), false);
    vte_terminal_set_allow_bold(VTE_TERMINAL(__terminal), true);
    vte_terminal_set_scroll_on_output(VTE_TERMINAL(__terminal), true);
    vte_terminal_set_scroll_on_keystroke(VTE_TERMINAL(__terminal), true);
    vte_terminal_set_backspace_binding(VTE_TERMINAL(__terminal), VTE_ERASE_AUTO);
    vte_terminal_set_delete_binding(VTE_TERMINAL(__terminal), VTE_ERASE_AUTO);
    vte_terminal_set_emulation(VTE_TERMINAL(__terminal), "xterm");
    vte_terminal_set_encoding(VTE_TERMINAL(__terminal), "utf8");

    palette().apply(__terminal);
  }

  terminal::~terminal() {
    gtk_widget_destroy(__terminal);
    ::close(vte_terminal_get_pty(VTE_TERMINAL(__terminal)));

    ::std::clog << "~terminal()" << ::std::endl;
  }

  void terminal::set_size(::std::uint8_t const row_count_in, ::std::uint8_t const column_count_in) {
    vte_terminal_set_size(VTE_TERMINAL(__terminal), column_count_in, row_count_in);
  }
  void terminal::set_scrollback_size(::std::int16_t const scrollback_size_in) {
    vte_terminal_set_scrollback_lines(VTE_TERMINAL(__terminal), scrollback_size_in);
  }

  ::std::uint32_t terminal::column_count() {
    return vte_terminal_get_column_count(VTE_TERMINAL(__terminal));
  }
  ::std::uint32_t terminal::row_count() {
    return vte_terminal_get_row_count(VTE_TERMINAL(__terminal));
  }

  ::std::uint32_t terminal::cursor_column() {
    ::std::int64_t column_out;
    ::std::int64_t row;
    vte_terminal_get_cursor_position(VTE_TERMINAL(__terminal), &column_out, &row);
    return column_out;
  }
  ::std::uint32_t terminal::cursor_row() {
    ::std::int64_t column;
    ::std::int64_t row_out;
    vte_terminal_get_cursor_position(VTE_TERMINAL(__terminal), &column, &row_out);
    return row_out;
  }

  void terminal::fork_command(::std::string const& command_in) {
    char* args[] = { "/bin/sh", "-c", const_cast< char* > (command_in.c_str()), NULL };
    vte_terminal_fork_command(VTE_TERMINAL(__terminal), "/bin/sh", args, NULL, NULL, false, false, false);
  }

  void terminal::write(::std::string const& string_in) {
    vte_terminal_feed_child(VTE_TERMINAL(__terminal), string_in.c_str(), string_in.size());
  }

  //  struct screen_builder: ::std::unary_function< char, void > {
  //      screen_builder(GArray* attributes_array_in, screen& screen_inout) :
  //        __row(0), __column(0), __index(0), __attributes_array(attributes_array_in), __screen(screen_inout) {
  //      }
  //
  //      void operator()(char const character_in) {
  //        VteCharAttributes vte_attributes = g_array_index(__attributes_array, VteCharAttributes, __index);
  //        attributes cell_attributes;
  //
  //        cell_attributes.set_background(__palette.get_color(vte_attributes.back));
  //        cell_attributes.set_foreground(__palette.get_color(vte_attributes.fore));
  //        cell_attributes.set_strikethrough(vte_attributes.strikethrough);
  //        cell_attributes.set_underlined(vte_attributes.underline);
  //
  //        __screen(__row, __column) = cell(character_in);
  //        __screen(vte_attributes.row, vte_attributes.column) = cell(__screen(vte_attributes.row,
  //                                                                            vte_attributes.column),
  //                                                                   cell_attributes);
  //        if (character_in == '\n') {
  //          __column = 0;
  //          __row++;
  //        } else {
  //          __column++;
  //        }
  //
  //        __index++;
  //      }
  //
  //    private:
  //      palette __palette;
  //      ::std::uint16_t __row;
  //      ::std::uint16_t __column;
  //      ::std::uint8_t __index;
  //      GArray* __attributes_array;
  //      screen& __screen;
  //  };
  //
  //  struct debug_printer: ::std::unary_function< void, char > {
  //      void operator()(char const char_in) {
  //        if (::boost::is_print()(char_in)) {
  //          ::std::clog << char_in;
  //        } else {
  //          ::std::clog << "\\x" << ::std::hex << ::std::setfill('0') << ::std::setw(2)
  //              << static_cast< ::std::uint32_t > (char_in) << ::std::dec << ::std::setw(0);
  //        }
  //      }
  //  };
  //
  //  bool longer_than(::std::string const& lhs_in, ::std::string const& rhs_in) {
  //    return lhs_in.size() < rhs_in.size();
  //  }

  screen terminal::read() {
    GArray* attributes_array = g_array_new(false, false, sizeof(VteCharAttributes));
    char* text = vte_terminal_get_text(VTE_TERMINAL(__terminal), NULL, NULL, attributes_array);

    ::std::vector< attribute > attributes;
    ::std::vector< ::std::string > lines;

    if (text != NULL) {
      ::std::string screen_contents(text);
      delete[] text;

      palette palette;
      ::std::int32_t first_row = g_array_index(attributes_array, VteCharAttributes, 0).row;
      for (::std::uint32_t i = 0; i < attributes_array->len; ++i) {
        VteCharAttributes vte_attributes = g_array_index(attributes_array, VteCharAttributes, i);
        attribute cell_attributes;

        cell_attributes.set_background(palette.get_color(vte_attributes.back));
        cell_attributes.set_foreground(palette.get_color(vte_attributes.fore));
        cell_attributes.set_strikethrough(vte_attributes.strikethrough);
        cell_attributes.set_underlined(vte_attributes.underline);
        cell_attributes.set_row(vte_attributes.row - first_row);
        cell_attributes.set_column(vte_attributes.column);

        attributes.push_back(cell_attributes);
      }

      ::boost::split(lines, screen_contents, ::boost::is_any_of("\n"));
    }

    g_array_free(attributes_array, TRUE);
    return screen(lines, attributes);
  }

} // namespace anyterm
