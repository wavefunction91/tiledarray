/*
 * This file is a part of TiledArray.
 * Copyright (C) 2013  Virginia Tech
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef TILEDARRAY_DIST_EVAL_BINARY_EVAL_H__INCLUDED
#define TILEDARRAY_DIST_EVAL_BINARY_EVAL_H__INCLUDED

#include <TiledArray/dist_eval/dist_eval.h>
#include <TiledArray/zero_tensor.h>

namespace TiledArray {
  namespace detail {

    /// Binary, distributed tensor evaluator

    /// This object is used to evaluate the tiles of a distributed binary
    /// expression.
    /// \tparam Left The left argument type
    /// \tparam Right The right argument type
    /// \tparam Op The binary transform operator type
    /// \tparam Policy The tensor policy class
    template <typename Left, typename Right, typename Op, typename Policy>
    class BinaryEvalImpl :
      public DistEvalImpl<typename Op::result_type, Policy>,
      public std::enable_shared_from_this<BinaryEvalImpl<Left, Right, Op, Policy> >
    {
    public:
      typedef BinaryEvalImpl<Left, Right, Op, Policy> BinaryEvalImpl_; ///< This object type
      typedef DistEvalImpl<typename Op::result_type, Policy> DistEvalImpl_; ///< The base class type
      typedef typename DistEvalImpl_::TensorImpl_ TensorImpl_; ///< The base, base class type
      typedef Left left_type; ///< The left-hand argument type
      typedef Right right_type; ///< The right-hand argument type
      typedef typename DistEvalImpl_::size_type size_type; ///< Size type
      typedef typename DistEvalImpl_::range_type range_type; ///< Range type
      typedef typename DistEvalImpl_::shape_type shape_type; ///< Shape type
      typedef typename DistEvalImpl_::pmap_interface pmap_interface; ///< Process map interface type
      typedef typename DistEvalImpl_::trange_type trange_type; ///< Tiled range type
      typedef typename DistEvalImpl_::value_type value_type; ///< Tile type
      typedef typename DistEvalImpl_::eval_type eval_type; ///< Tile evaluation type
      typedef Op op_type; ///< Tile evaluation operator type

      using std::enable_shared_from_this<BinaryEvalImpl_>::shared_from_this;

    private:

      left_type left_; ///< Left argument
      right_type right_; ///< Right argument
      op_type op_; ///< binary element operator

    public:

      /// Construct a binary evaluator

      /// \param left The left-hand argument
      /// \param right The right-hand argument
      /// \param world The world where the tensor lives
      /// \param trange The tiled range object
      /// \param shape The tensor shape object
      /// \param pmap The tile-process map
      /// \param perm The permutation that is applied to tile indices
      /// \param op The tile transform operation
      BinaryEvalImpl(const left_type& left, const right_type& right,
          World& world, const trange_type& trange, const shape_type& shape,
          const std::shared_ptr<pmap_interface>& pmap, const Permutation& perm,
          const op_type& op) :
        DistEvalImpl_(world, trange, shape, pmap, perm),
        left_(left), right_(right), op_(op)
      {
        TA_ASSERT(left.trange() == right.trange());
      }

      virtual ~BinaryEvalImpl() { }

      /// Get tile at index \c i

      /// \param i The index of the tile
      /// \return A \c Future to the tile at index i
      /// \throw TiledArray::Exception When tile \c i is owned by a remote node.
      /// \throw TiledArray::Exception When tile \c i a zero tile.
      virtual Future<value_type> get_tile(size_type i) const {
        TA_ASSERT(TensorImpl_::is_local(i));
        TA_ASSERT(! TensorImpl_::is_zero(i));


        const size_type source_index = DistEvalImpl_::perm_index_to_source(i);
        const ProcessID source =  left_.owner(source_index); // Left and right
                                                  // should have the same owner

        const madness::DistributedID key(DistEvalImpl_::id(), i);
        return TensorImpl_::get_world().gop.template recv<value_type>(source, key);
      }

      /// Discard a tile that is not needed

      /// This function handles the cleanup for tiles that are not needed in
      /// subsequent computation.
      /// \param i The index of the tile
      virtual void discard_tile(size_type i) const { get_tile(i); }

    private:

      /// Task function for evaluating tiles

      /// \param i The tile index
      /// \param left The left-hand tile
      /// \param right The right-hand tile
      template <typename L, typename R>
      void eval_tile(const size_type i, L left, R right) {
        DistEvalImpl_::set_tile(i, op_(left, right));
      }

      /// Evaluate the tiles of this tensor

      /// This function will evaluate the children of this distributed evaluator
      /// and evaluate the tiles for this distributed evaluator. It will block
      /// until the tasks for the children are evaluated (not for the tasks of
      /// this object).
      /// \return The number of tiles that will be set by this process
      virtual int internal_eval() {

        // Evaluate child tensors
        left_.eval();
        right_.eval();

        // Task function argument types
        typedef typename madness::if_<std::is_const<typename op_type::first_argument_type>,
            const typename left_type::value_type,
                  typename left_type::value_type>::type &
                left_argument_type;
        typedef typename madness::if_<std::is_const<typename op_type::second_argument_type>,
            const typename right_type::value_type,
                  typename right_type::value_type>::type &
                right_argument_type;

        size_type task_count = 0ul;

        // Construct local iterator
        TA_ASSERT(left_.pmap() == right_.pmap());
        std::shared_ptr<BinaryEvalImpl_> self = shared_from_this();
        typename pmap_interface::const_iterator it = left_.pmap()->begin();
        const typename pmap_interface::const_iterator end = left_.pmap()->end();

        if(left_.is_dense() && right_.is_dense() && TensorImpl_::is_dense()) {
          // Evaluate tiles where both arguments and the result are dense
          for(; it != end; ++it) {
            // Get tile indices
            const size_type source_index = *it;
            const size_type target_index = DistEvalImpl_::perm_index_to_target(source_index);

            // Schedule tile evaluation task
            TensorImpl_::get_world().taskq.add(self,
                & BinaryEvalImpl_::template eval_tile<left_argument_type, right_argument_type>,
                target_index, left_.get(source_index), right_.get(source_index));

            ++task_count;
          }
        } else {
          // Evaluate tiles where the result or one of the arguments is sparse
          for(; it != end; ++it) {
            // Get tile indices
            const size_type index = *it;
            const size_type target_index = DistEvalImpl_::perm_index_to_target(index);

            if(! TensorImpl_::is_zero(target_index)) {
              // Schedule tile evaluation task
              if(left_.is_zero(index)) {
                TensorImpl_::get_world().taskq.add(self,
                  & BinaryEvalImpl_::template eval_tile<const ZeroTensor, right_argument_type>,
                  target_index, ZeroTensor(), right_.get(index));
              } else if(right_.is_zero(index)) {
                TensorImpl_::get_world().taskq.add(self,
                  & BinaryEvalImpl_::template eval_tile<left_argument_type, const ZeroTensor>,
                  target_index, left_.get(index), ZeroTensor());
              } else {
                TensorImpl_::get_world().taskq.add(self,
                  & BinaryEvalImpl_::template eval_tile<left_argument_type, right_argument_type>,
                  target_index, left_.get(index), right_.get(index));
              }

              ++task_count;
            } else {
              // Cleanup unused tiles
              if(! left_.is_zero(index))
                left_.discard(index);
              if(! right_.is_zero(index))
                right_.discard(index);
            }
          }
        }

        // Wait for child tensors to be evaluated, and process tasks while waiting.
        left_.wait();
        right_.wait();

        return task_count;
      }

    }; // class BinaryEvalImpl

  }  // namespace detail
}  // namespace TiledArray

#endif // TILEDARRAY_DIST_EVAL_BINARY_EVAL_H__INCLUDED
