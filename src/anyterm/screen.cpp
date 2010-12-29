/**
 * @file
 * @date Dec 16, 2010
 */

#include "anyterm/screen.hpp"

namespace anyterm {

  screen::screen(int row_count_in, int column_count_in, int scrollback_capacity_in) :
    row_count_(row_count_in), column_count_(column_count_in), scrollback_capacity_(scrollback_capacity_in),
        wrap_(0), cursor_row_number_(0), cursor_column_number_(0), cursor_visible_(true) {
    for (int r = 0; r < scrollback_capacity_in; ++r) {
      cells_.push_back(new row_t(column_count_in)); // leaks if it throws
    }
    for (int r = 0; r < row_count_in; ++r) {
      cells_.push_back(new row_t(column_count_in)); // leaks if it throws
    }
  }

  screen::~screen() {
    for (cells_t::iterator i = cells_.begin(); i != cells_.end(); ++i) {
      delete *i;
    }
  }

  int screen::rows() const {
    return row_count_;
  }

  int screen::cols() const {
    return column_count_;
  }

  int screen::scrollback() const {
    return scrollback_capacity_;
  }

  cell& screen::operator()(int row_in, int column_in) {
    return (*cells_[row_idx(row_in)])[column_in];
  }

  const cell& screen::operator()(int row_in, int column_in) const {
    return (*cells_[row_idx(row_in)])[column_in];
  }

  void screen::scroll_down(int top_in, int bottom_in, int count_in) {
    // If we're asked to scroll the whole visible screen down, we scroll
    // into the scrollback region.  Otherwise, the scrollback region is
    // not changed.
    if (top_in == 0 && bottom_in == rows() - 1) {
      wrap_ = (wrap_ + count_in) % (rows() + scrollback());
    } else {
      normalise_wrap();
      ::std::rotate(cells_.begin() + scrollback() + top_in,
                    cells_.begin() + scrollback() + top_in + count_in,
                    cells_.begin() + scrollback() + bottom_in + 1);
    }
    for (int r = bottom_in + 1 - count_in; r <= bottom_in; ++r) {
      clear_row(r);
    }
  }

  void screen::scroll_up(int top_in, int bottom_in, int count_in) {
    // Never touch the scrollback region.
    if (scrollback() == 0 && top_in == 0 && bottom_in == rows() - 1) {
      wrap_ = (wrap_ - count_in) % (rows() + scrollback());
    } else {
      normalise_wrap();
      ::std::rotate(cells_.begin() + scrollback() + top_in, cells_.begin() + scrollback() + bottom_in + 1
          - count_in, cells_.begin() + scrollback() + bottom_in + 1);
    }
    for (int r = top_in; r < top_in + count_in; ++r) {
      clear_row(r);
    }
  }

  int screen::row_idx(int row_in) const {
    return (row_in + scrollback() + wrap_) % (rows() + scrollback());
  }

  void screen::clear_row(int row_in) {
    row_t& row = *(cells_[row_idx(row_in)]);
    for (int c = 0; c < cols(); ++c) {
      // FIXME this should probably use the terminal's current attributes
      // (e.g. background colour)
      row[c] = cell();
    }
  }

  void screen::normalise_wrap() {
    if (wrap_ == 0) {
      return;
    }
    ::std::rotate(cells_.begin(), cells_.begin() + wrap_, cells_.end());
    wrap_ = 0;
  }

} // namespace anyterm
