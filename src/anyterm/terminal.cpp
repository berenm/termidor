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

#include <boost/regex/pending/unicode_iterator.hpp>

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
        default_fg_color( { 0xFF, 0xFF, 0xFF }), default_bg_color( { 0x00, 0x00, 0x00 }),
            bold_fg_color( { 0xFF, 0xFF, 0xFE }), dim_fg_color( { 0xFF, 0xFF, 0xFD }),
            cursor_bg_color( { 0x00, 0x00, 0x01 }), highlight_bg_color( { 0x00, 0x00, 0x02 }) {
        for (int color = 0, value = 0xFFFF03; color < 255 && value < 0xFFFFFD; ++color, ++value) {
          colors[color].red = (value >> 0x16) & 0xFF;
          colors[color].green = (value >> 0x08) & 0xFF;
          colors[color].blue = (value >> 0x00) & 0xFF;
        }
      }

      void apply(GtkWidget* terminal_inout) {
        vte_terminal_set_colors(VTE_TERMINAL(terminal_inout), NULL, NULL, colors, 255);
        vte_terminal_set_color_foreground(VTE_TERMINAL(terminal_inout), &default_fg_color);
        vte_terminal_set_color_background(VTE_TERMINAL(terminal_inout), &default_bg_color);
        vte_terminal_set_color_cursor(VTE_TERMINAL(terminal_inout), &cursor_bg_color);
        vte_terminal_set_color_bold(VTE_TERMINAL(terminal_inout), &bold_fg_color);
        vte_terminal_set_color_dim(VTE_TERMINAL(terminal_inout), &dim_fg_color);
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

  static void set_dirty_callback(VteTerminal*, void* data_in) {
    reinterpret_cast< terminal* > (data_in)->set_dirty(true);
  }
  static void set_dirty_callback_i(VteTerminal*, int, void* data_in) {
    reinterpret_cast< terminal* > (data_in)->set_dirty(true);
  }

  static void set_killed_callback(VteTerminal*, void* data_in) {
    reinterpret_cast< terminal* > (data_in)->set_alive(false);
  }

  terminal::terminal() :
    __alive(false), __dirty(false) {
    __terminal = vte_terminal_new();

    vte_terminal_set_audible_bell(VTE_TERMINAL(__terminal), false);
    vte_terminal_set_visible_bell(VTE_TERMINAL(__terminal), false);
    vte_terminal_set_allow_bold(VTE_TERMINAL(__terminal), true);
    vte_terminal_set_scroll_on_output(VTE_TERMINAL(__terminal), true);
    vte_terminal_set_scroll_on_keystroke(VTE_TERMINAL(__terminal), true);
    vte_terminal_set_backspace_binding(VTE_TERMINAL(__terminal), VTE_ERASE_AUTO);
    vte_terminal_set_delete_binding(VTE_TERMINAL(__terminal), VTE_ERASE_AUTO);
    vte_terminal_set_emulation(VTE_TERMINAL(__terminal), "xterm");
    vte_terminal_set_encoding(VTE_TERMINAL(__terminal), "utf8");

    g_signal_connect(G_OBJECT(__terminal), "beep", G_CALLBACK(set_dirty_callback), this);
    g_signal_connect(G_OBJECT(__terminal), "contents-changed", G_CALLBACK(set_dirty_callback), this);
    g_signal_connect(G_OBJECT(__terminal), "cursor-moved", G_CALLBACK(set_dirty_callback), this);
    g_signal_connect(G_OBJECT(__terminal), "emulation-changed", G_CALLBACK(set_dirty_callback), this);
    g_signal_connect(G_OBJECT(__terminal), "encoding-changed", G_CALLBACK(set_dirty_callback), this);
    g_signal_connect(G_OBJECT(__terminal), "paste-clipboard", G_CALLBACK(set_dirty_callback), this);
    g_signal_connect(G_OBJECT(__terminal), "selection-changed", G_CALLBACK(set_dirty_callback), this);
    g_signal_connect(G_OBJECT(__terminal), "text-deleted", G_CALLBACK(set_dirty_callback), this);
    g_signal_connect(G_OBJECT(__terminal), "text-inserted", G_CALLBACK(set_dirty_callback), this);
    g_signal_connect(G_OBJECT(__terminal), "text-modified", G_CALLBACK(set_dirty_callback), this);
    g_signal_connect(G_OBJECT(__terminal), "text-scrolled", G_CALLBACK(set_dirty_callback_i), this);

    g_signal_connect(G_OBJECT(__terminal), "child-exited", G_CALLBACK(set_killed_callback), this);
    g_signal_connect(G_OBJECT(__terminal), "eof", G_CALLBACK(set_killed_callback), this);
  }

  terminal::~terminal() {
    gtk_widget_destroy(__terminal);
    ::close(vte_terminal_get_pty(VTE_TERMINAL( __terminal)));
  }

  void terminal::set_size(::std::uint16_t const row_count_in, ::std::uint16_t const column_count_in) {
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

  void terminal::login(::std::string const& username_in) {
    ::std::string command = "su ";
    command += username_in;
    command += " - ";

    //    char const* args[] = { "/bin/sh", "-c", const_cast< char* > (command.c_str()), NULL };
    char const* args[] = { "/bin/bash", "-l", NULL };
    vte_terminal_fork_command(VTE_TERMINAL(__terminal),
                              "/bin/bash",
                              const_cast< char** > (args),
                              NULL,
                              NULL,
                              false,
                              false,
                              false);

    set_alive(true);
  }

  bool terminal::is_alive() const {
    return __alive;
  }
  void terminal::set_alive(bool const alive_in) {
    __alive = alive_in;
  }

  bool terminal::is_dirty() const {
    return __dirty;
  }
  void terminal::set_dirty(bool const dirty_in) {
    __dirty = dirty_in;
  }

  void terminal::write(::std::string const& data_in) {
    vte_terminal_feed_child(VTE_TERMINAL(__terminal), data_in.c_str(), data_in.size());
  }

  screen terminal::read() {
    palette().apply(__terminal);

    GArray* attributes_array = g_array_new(false, false, sizeof(VteCharAttributes));
    char* text = vte_terminal_get_text_include_trailing_spaces(VTE_TERMINAL(__terminal), NULL, NULL, attributes_array);
    __dirty = false;

    screen::line_vector_t lines;
    if (text != NULL) {
      typedef ::boost::u8_to_u32_iterator< ::std::string::iterator > to_u32;
      ::std::string screen_contents(text);
      delete[] text;

      ::std::wstring wcontents(to_u32(screen_contents.begin()), to_u32(screen_contents.end()));
      ::boost::split(lines, wcontents, ::boost::is_any_of(L"\n"));
    }

    screen screen_out(lines);

    palette palette;
    ::std::uint32_t first_row = g_array_index(attributes_array, VteCharAttributes, 0).row;
    ::std::uint32_t last_row;
    ::std::uint32_t current_row = 0;
    ::std::uint32_t current_column = 0;
    for (::std::uint32_t i = 0; i < attributes_array->len; ++i) {
      VteCharAttributes vte_attributes = g_array_index(attributes_array, VteCharAttributes, i);
      attribute cell_attributes;

      for (::std::uint32_t last_column = current_column + 1; last_column < vte_attributes.column; ++last_column) {
        screen_out.set_attribute(current_row, last_column, attribute());
      }

      current_row = ::std::min(vte_attributes.row - first_row, vte_attributes.row);
      current_column = vte_attributes.column;

      cell_attributes.set_background(palette.get_color(vte_attributes.back));
      cell_attributes.set_foreground(palette.get_color(vte_attributes.fore));
      cell_attributes.set_strikethrough(vte_attributes.strikethrough);
      cell_attributes.set_underlined(vte_attributes.underline);
      cell_attributes.set_row(current_row);
      cell_attributes.set_column(current_column);

      screen_out.set_attribute(current_row, current_column, cell_attributes);
    }
    last_row = current_row;

    attribute cursor_attribute = screen_out.get_attribute(cursor_row() - first_row, cursor_column());
    attribute cursor_next_attribute = screen_out.get_attribute(cursor_row() - first_row, cursor_column() + 1);
    cursor_attribute.set_cursor(true);
    cursor_next_attribute.set_cursor(false);

    current_row = cursor_row();
    current_row = ::std::min(last_row, ::std::min(current_row - first_row, current_row));
    current_column = cursor_column();
    screen_out.set_attribute(current_row, current_column, cursor_attribute);
    screen_out.set_attribute(current_row, current_column + 1, cursor_next_attribute);

    g_array_free(attributes_array, TRUE);
    return screen_out;
  }

} // namespace anyterm
