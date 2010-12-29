/**
 * @file
 * @date Dec 15, 2010
 */

#ifndef ANYTERM_SCREEN_HPP_
#define ANYTERM_SCREEN_HPP_

#include "anyterm/cell.hpp"

#include <vector>
#include <algorithm>

namespace anyterm {

  class screen {
      typedef ::std::vector< cell > row_t;
      typedef ::std::vector< row_t* > cells_t;

      int row_count_;
      int column_count_;
      int scrollback_capacity_;
      cells_t cells_;
      unsigned int wrap_;

    public:
      screen(int row_count_in, int column_count_in, int scrollback_capacity_in = 0);
      ~screen();

      int rows() const;
      int cols() const;

      int scrollback() const;

      cell& operator()(int row_in, int column_in);
      const cell& operator()(int row_in, int column_in) const;

      void scroll_down(int top_in, int bottom_in, int count_in = 1);
      void scroll_up(int top_in, int bottom_in, int count_in = 1);

      int cursor_row_number_;
      int cursor_column_number_;
      bool cursor_visible_;

    private:
      int row_idx(int row_in) const;
      void clear_row(int row_in);
      void normalise_wrap();
  };

} // namespace anyterm

#endif /* ANYTERM_SCREEN_HPP_ */
