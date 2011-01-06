/**
 * @file
 * @date Dec 16, 2010
 */

#include "anyterm/screen.hpp"

#include <iostream>

namespace anyterm {

//  screen::screen(cell_table_t::size_type const row_count_in,
//                 cell_table_t::size_type const column_count_in,
//                 cell_table_t::size_type const scrollback_capacity_in) :
//    __cell_table(::boost::extents[row_count_in][column_count_in]), __first_visible_row(0), wrap_(0),
//        cursor_row_number_(0), cursor_column_number_(0), cursor_visible_(true) {
//  }
//
//  screen::~screen() {
//  }
//
//  screen::cell_table_t::size_type screen::row_count() const {
//    return __cell_table.shape()[0];
//  }
//
//  screen::cell_table_t::size_type screen::column_count() const {
//    return __cell_table.shape()[1];
//  }
//
//  screen::cell_table_t::size_type screen::scrollback_count() const {
//    //    return scrollback_capacity_;
//    return 0;
//  }
//
//  screen::cell_table_t const& screen::table() const {
//    return __cell_table;
//  }
//
//  screen::row_view_t screen::row(cell_table_t::index const row_in) {
//    return __cell_table[indices[row_in][range()]];
//  }
//
//  screen::const_row_view_t screen::row(cell_table_t::index const row_in) const {
//    return __cell_table[indices[row_in][range()]];
//  }
//
//  screen::column_view_t screen::column(cell_table_t::index const column_in) {
//    return __cell_table[indices[range()][column_in]];
//  }
//
//  screen::const_column_view_t screen::column(cell_table_t::index const column_in) const {
//    return __cell_table[indices[range()][column_in]];
//  }
//
//  cell& screen::operator()(cell_table_t::index const row_in, cell_table_t::index const column_in) {
//    return __cell_table[row_in][column_in];
//  }
//
//  cell const& screen::operator()(cell_table_t::index const row_in, cell_table_t::index const column_in) const {
//    return __cell_table[row_in][column_in];
//  }
//
//  void screen::scroll_down(int top_in, int bottom_in, int count_in) {
//    // If we're asked to scroll the whole visible screen down, we scroll
//    // into the scrollback region.  Otherwise, the scrollback region is
//    // not changed.
//    //    if (top_in == 0 && bottom_in == row_count() - 1) {
//    //      wrap_ = (wrap_ + count_in) % (row_count() + scrollback_count());
//    //    } else {
//    //      normalise_wrap();
//    //      ::std::rotate(cells_.begin() + scrollback_count() + top_in, cells_.begin() + scrollback_count()
//    //          + top_in + count_in, cells_.begin() + scrollback_count() + bottom_in + 1);
//    //    }
//    //    for (int r = bottom_in + 1 - count_in; r <= bottom_in; ++r) {
//    //      clear_row(r);
//    //    }
//  }
//
//  void screen::scroll_up(int top_in, int bottom_in, int count_in) {
//    // Never touch the scrollback region.
//    //    if (scrollback_count() == 0 && top_in == 0 && bottom_in == row_count() - 1) {
//    //      wrap_ = (wrap_ - count_in) % (row_count() + scrollback_count());
//    //    } else {
//    //      normalise_wrap();
//    //      ::std::rotate(cells_.begin() + scrollback_count() + top_in, cells_.begin() + scrollback_count()
//    //          + bottom_in + 1 - count_in, cells_.begin() + scrollback_count() + bottom_in + 1);
//    //    }
//    //    for (int r = top_in; r < top_in + count_in; ++r) {
//    //      clear_row(r);
//    //    }
//  }
//
//  int screen::row_idx(int row_in) const {
//    return (row_in + scrollback_count() + wrap_) % (row_count() + scrollback_count());
//  }
//
//  void screen::clear_row(int row_in) {
//    //    row_t& row = *(cells_[row_idx(row_in)]);
//    //    for (int c = 0; c < column_count(); ++c) {
//    //      // FIXME this should probably use the terminal's current attributes
//    //      // (e.g. background colour)
//    //      row[c] = cell();
//    //    }
//  }
//
//  void screen::normalise_wrap() {
//    //    if (wrap_ == 0) {
//    //      return;
//    //    }
//    //    ::std::rotate(cells_.begin(), cells_.begin() + wrap_, cells_.end());
//    //    wrap_ = 0;
//  }

} // namespace anyterm
