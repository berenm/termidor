/**
 * @file
 * @date Dec 15, 2010
 */

#ifndef ANYTERM_SCREEN_HPP_
#define ANYTERM_SCREEN_HPP_

#include "anyterm/attribute.hpp"

#include <boost/multi_array.hpp>

#include <vector>
#include <algorithm>

namespace anyterm {

  struct screen {
      typedef ::std::vector< ::std::string > line_vector_t;
      typedef ::std::vector< attribute > attribute_vector_t;

      screen(line_vector_t const& lines_in, attribute_vector_t attributes_in) :
        __lines(lines_in), __attributes(attributes_in) {
        ::std::sort(__attributes.begin(), __attributes.end());
      }
      ~screen() {
      }

      //      typedef ::boost::multi_array< cell, 2 > cell_table_t;
      //      typedef cell_table_t::array_view< 1 >::type row_view_t;
      //      typedef cell_table_t::array_view< 1 >::type column_view_t;
      //      typedef cell_table_t::const_subarray< 1 >::type sub_array_t;
      //      typedef cell_table_t::const_array_view< 1 >::type const_row_view_t;
      //      typedef cell_table_t::const_array_view< 1 >::type const_column_view_t;

      line_vector_t const& lines() const {
        return __lines;
      }
      attribute_vector_t const& attributes() const {
        return __attributes;
      }

    private:
      line_vector_t __lines;
      attribute_vector_t __attributes;

      //      typedef cell_table_t::index_range range;
      //      cell_table_t::index_gen indices;
      //      cell_table_t __cell_table;
      //
      //      cell_table_t::size_type __first_visible_row;
      //      unsigned int wrap_;
      //
      //    public:
      //      screen(cell_table_t::size_type const row_count_in,
      //             cell_table_t::size_type const column_count_in,
      //             cell_table_t::size_type const scrollback_capacity_in = 0);
      //      ~screen();
      //
      //      cell_table_t::size_type row_count() const;
      //      cell_table_t::size_type column_count() const;
      //      cell_table_t::size_type scrollback_count() const;
      //
      //      cell_table_t const& table() const;
      //
      //      row_view_t row(cell_table_t::index const row_in);
      //      const_row_view_t row(cell_table_t::index const row_in) const;
      //      column_view_t column(cell_table_t::index const column_in);
      //      const_column_view_t column(cell_table_t::index const column_in) const;
      //
      //      cell& operator()(cell_table_t::index const row_in, cell_table_t::index const column_in);
      //      cell const& operator()(cell_table_t::index const row_in, cell_table_t::index const column_in) const;
      //
      //      void scroll_down(int top_in, int bottom_in, int count_in = 1);
      //      void scroll_up(int top_in, int bottom_in, int count_in = 1);
      //
      //      int cursor_row_number_;
      //      int cursor_column_number_;
      //      bool cursor_visible_;
      //
      //    private:
      //      int row_idx(int row_in) const;
      //      void clear_row(int row_in);
      //      void normalise_wrap();
  };

} // namespace anyterm

#endif /* ANYTERM_SCREEN_HPP_ */
