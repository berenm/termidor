/**
 * @file
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE or copy at http://www.boost.org/LICENSE
 */

#include "anyterm/terminal.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/regex/pending/unicode_iterator.hpp>

#include <vte.h>

namespace anyterm {

  namespace {

    struct palette {
      GdkColor       colors[256];
      GdkColor const default_fg_color;
      GdkColor const default_bg_color;
      GdkColor const bold_fg_color;
      GdkColor const faint_fg_color;
      GdkColor const cursor_bg_color;
      GdkColor const bright_bg_color;

      palette() :
        default_fg_color({ 0xFF, 0xFF, 0xFF }),
        default_bg_color({ 0x00, 0x00, 0x00 }),
        bold_fg_color({ 0xFF, 0xFF, 0xFE }),
        faint_fg_color({ 0xFF, 0xFF, 0xFD }),
        cursor_bg_color({ 0x00, 0x00, 0x01 }),
        bright_bg_color({ 0x00, 0x00, 0x0 }) {
        for (std::size_t color = 0, value = 0xFFFF03; color < 256 && value < 0xFFFFFD; ++color, ++value) {
          this->colors[color].red   = (value >> 0x16) & 0xFF;
          this->colors[color].green = (value >> 0x08) & 0xFF;
          this->colors[color].blue  = (value >> 0x00) & 0xFF;
        }
      }

      void apply(GtkWidget* const widget) const {
        vte_terminal_set_colors(VTE_TERMINAL(widget), nullptr, nullptr, this->colors, 255);
        vte_terminal_set_color_foreground(VTE_TERMINAL(widget), &this->default_fg_color);
        vte_terminal_set_color_background(VTE_TERMINAL(widget), &this->default_bg_color);
        vte_terminal_set_color_cursor(VTE_TERMINAL(widget), &this->cursor_bg_color);
        vte_terminal_set_color_bold(VTE_TERMINAL(widget), &this->bold_fg_color);
        vte_terminal_set_color_dim(VTE_TERMINAL(widget), &this->faint_fg_color);
        vte_terminal_set_color_highlight(VTE_TERMINAL(widget), &this->bright_bg_color);
      }

      std::string get_color(GdkColor const& color) const {
        if (gdk_color_equal(&color, &this->default_fg_color))
          return attribute::default_foreground;

        if (gdk_color_equal(&color, &this->default_bg_color))
          return attribute::default_background;

        if (gdk_color_equal(&color, &this->bold_fg_color))
          return attribute::default_bold;

        if (gdk_color_equal(&color, &this->faint_fg_color))
          return attribute::default_faint;

        if (gdk_color_equal(&color, &this->cursor_bg_color))
          return attribute::default_cursor;

        if (gdk_color_equal(&color, &this->bright_bg_color))
          return attribute::default_bright;

        for (std::size_t i = 0; i < 256; ++i) {
          if (gdk_color_equal(&color, &this->colors[i]))
            return boost::lexical_cast< std::string >(i);
        }

        return "unknown";
      }

    };

    static void set_dirty_callback(VteTerminal*, void* data) {
      static_cast< anyterm::terminal* >(data)->set_dirty(true);
    }

    static void set_dirty_callback_i(VteTerminal*, int, void* data) {
      static_cast< anyterm::terminal* >(data)->set_dirty(true);
    }

    static void set_killed_callback(VteTerminal*, void* data) {
      static_cast< anyterm::terminal* >(data)->set_alive(false);
    }

  }

  terminal::terminal() :
    alive(false),
    dirty(true),
    widget(vte_terminal_new()) {

    vte_terminal_set_audible_bell(VTE_TERMINAL(this->widget), false);
    vte_terminal_set_visible_bell(VTE_TERMINAL(this->widget), false);
    vte_terminal_set_allow_bold(VTE_TERMINAL(this->widget), true);
    vte_terminal_set_scroll_on_output(VTE_TERMINAL(this->widget), true);
    vte_terminal_set_scroll_on_keystroke(VTE_TERMINAL(this->widget), true);
    vte_terminal_set_backspace_binding(VTE_TERMINAL(this->widget), VTE_ERASE_AUTO);
    vte_terminal_set_delete_binding(VTE_TERMINAL(this->widget), VTE_ERASE_AUTO);
    vte_terminal_set_emulation(VTE_TERMINAL(this->widget), "xterm");
    vte_terminal_set_encoding(VTE_TERMINAL(this->widget), "utf8");

    g_signal_connect(G_OBJECT(this->widget), "beep", G_CALLBACK(set_dirty_callback), this);
    g_signal_connect(G_OBJECT(this->widget), "contents-changed", G_CALLBACK(set_dirty_callback), this);
    g_signal_connect(G_OBJECT(this->widget), "cursor-moved", G_CALLBACK(set_dirty_callback), this);
    g_signal_connect(G_OBJECT(this->widget), "emulation-changed", G_CALLBACK(set_dirty_callback), this);
    g_signal_connect(G_OBJECT(this->widget), "encoding-changed", G_CALLBACK(set_dirty_callback), this);
    g_signal_connect(G_OBJECT(this->widget), "paste-clipboard", G_CALLBACK(set_dirty_callback), this);
    g_signal_connect(G_OBJECT(this->widget), "selection-changed", G_CALLBACK(set_dirty_callback), this);
    g_signal_connect(G_OBJECT(this->widget), "text-deleted", G_CALLBACK(set_dirty_callback), this);
    g_signal_connect(G_OBJECT(this->widget), "text-inserted", G_CALLBACK(set_dirty_callback), this);
    g_signal_connect(G_OBJECT(this->widget), "text-modified", G_CALLBACK(set_dirty_callback), this);
    g_signal_connect(G_OBJECT(this->widget), "text-scrolled", G_CALLBACK(set_dirty_callback_i), this);

    // g_signal_connect(G_OBJECT(this->widget), "current-directory-uri-changed", G_CALLBACK(set_dirty_callback), this);
    // g_signal_connect(G_OBJECT(this->widget), "current-file-uri-changed", G_CALLBACK(set_dirty_callback), this);
    g_signal_connect(G_OBJECT(this->widget), "decrease-font-size", G_CALLBACK(set_dirty_callback), this);
    g_signal_connect(G_OBJECT(this->widget), "increase-font-size", G_CALLBACK(set_dirty_callback), this);

    g_signal_connect(G_OBJECT(this->widget), "child-exited", G_CALLBACK(set_killed_callback), this);
    g_signal_connect(G_OBJECT(this->widget), "eof", G_CALLBACK(set_killed_callback), this);
  }

  terminal::~terminal() {
    gtk_widget_destroy(GTK_WIDGET(this->widget));
    ::close(vte_terminal_get_pty(VTE_TERMINAL(this->widget)));
  }

  void terminal::set_size(std::uint16_t const row_count, std::uint16_t const column_count) {
    vte_terminal_set_size(VTE_TERMINAL(this->widget), column_count, row_count);
  }

  void terminal::set_scrollback_size(std::int16_t const scrollback_size) {
    vte_terminal_set_scrollback_lines(VTE_TERMINAL(this->widget), scrollback_size);
  }

  std::uint32_t terminal::column_count() {
    return vte_terminal_get_column_count(VTE_TERMINAL(this->widget));
  }

  std::uint32_t terminal::row_count() {
    return vte_terminal_get_row_count(VTE_TERMINAL(this->widget));
  }

  std::uint32_t terminal::cursor_column() {
    glong column;
    glong row;

    vte_terminal_get_cursor_position(VTE_TERMINAL(this->widget), &column, &row);

    return static_cast< std::uint32_t >(column);
  }

  std::uint32_t terminal::cursor_row() {
    glong column;
    glong row;

    vte_terminal_get_cursor_position(VTE_TERMINAL(this->widget), &column, &row);

    return static_cast< std::uint32_t >(row);
  }

  void terminal::login(std::string const& username) {
    std::string command = "su ";

    command += username;
    command += " - ";

    // char const* args[] = { "/bin/sh", "-c", const_cast< char* > (command.c_str()), nullptr };
    char const* args[] = { "/bin/bash", "-l", nullptr };
    vte_terminal_fork_command_full(VTE_TERMINAL(this->widget), VTE_PTY_DEFAULT, nullptr, const_cast< char** >(args), nullptr, static_cast< GSpawnFlags >(0), nullptr, nullptr, nullptr, nullptr);

    this->alive = true;
  }

  bool terminal::is_alive() const {
    return this->alive;
  }

  void terminal::set_alive(bool const alive) {
    this->alive = alive;
  }

  bool terminal::is_dirty() const {
    return this->dirty;
  }

  void terminal::set_dirty(bool const dirty) {
    this->dirty = dirty;
  }

  void terminal::write(std::string const& data) {
    vte_terminal_feed_child(VTE_TERMINAL(this->widget), data.c_str(), data.size());
    this->dirty = true;
  }

  screen terminal::read() {
    anyterm::palette const palette;

    palette.apply(GTK_WIDGET(this->widget));

    GArray* attributes_array = g_array_new(false, false, sizeof(VteCharAttributes));
    char*   text             = vte_terminal_get_text_include_trailing_spaces(VTE_TERMINAL(this->widget), nullptr, nullptr, attributes_array);
    this->dirty = false;

    screen::text_lines lines;
    if (text != nullptr) {
      typedef boost::u8_to_u32_iterator< std::string::iterator > to_u32;

      std::string screen_contents(text);
      delete[] text;

      std::wstring wcontents(to_u32(screen_contents.begin()), to_u32(screen_contents.end()));
      boost::split(lines, wcontents, boost::is_any_of(L"\n"));
    }

    screen screen(lines);

    std::uint32_t first_row = g_array_index(attributes_array, VteCharAttributes, 0).row;
    std::uint32_t last_row;
    std::uint32_t current_row    = 0;
    std::uint32_t current_column = 0;
    for (std::uint32_t i = 0; i < attributes_array->len; ++i) {
      VteCharAttributes vte_attributes = g_array_index(attributes_array, VteCharAttributes, i);
      attribute         cell_attributes;

      for (std::uint32_t last_column = current_column + 1; last_column < vte_attributes.column; ++last_column) {
        screen.set_attribute(current_row, last_column, attribute());
      }

      current_row    = std::min(static_cast< std::uint32_t >(vte_attributes.row - first_row), static_cast< std::uint32_t >(vte_attributes.row));
      current_column = vte_attributes.column;

      cell_attributes.set_background(palette.get_color(vte_attributes.back));
      cell_attributes.set_foreground(palette.get_color(vte_attributes.fore));
      cell_attributes.set_strikethrough(vte_attributes.strikethrough);
      cell_attributes.set_underlined(vte_attributes.underline);
      cell_attributes.set_row(current_row);
      cell_attributes.set_column(current_column);
      cell_attributes.set_bold(vte_attributes.bold);
      cell_attributes.set_faint(vte_attributes.half);
      cell_attributes.set_bright(vte_attributes.standout);
      cell_attributes.set_blink(vte_attributes.blink);

      screen.set_attribute(current_row, current_column, cell_attributes);
    }
    last_row = current_row;

    attribute cursor_attribute      = screen.get_attribute(cursor_row() - first_row, cursor_column());
    attribute cursor_next_attribute = screen.get_attribute(cursor_row() - first_row, cursor_column() + 1);
    cursor_attribute.set_cursor(true);
    cursor_next_attribute.set_cursor(false);

    current_row    = cursor_row();
    current_row    = std::min(last_row, std::min(current_row - first_row, current_row));
    current_column = cursor_column();
    screen.set_attribute(current_row, current_column, cursor_attribute);
    screen.set_attribute(current_row, current_column + 1, cursor_next_attribute);

    g_array_free(attributes_array, TRUE);

    return screen;
  } // read

} // namespace anyterm
