/**
 * @license
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @author Volker Böhm
 * @copyright Copyright (c) 2021 Volker Böhm
 * @Overview
 * Implements some type and constant defintions used for search algorithms
 */

#ifndef __SEARCHDEF_H
#define __SEARCHDEF_H

#include <assert.h>
#include "../basics/types.h"

namespace ChessSearch {
	typedef uint32_t ply_t;
	typedef int32_t value_t;

	const value_t MAX_VALUE = 30000;
	const value_t MIN_MATE_VALUE = MAX_VALUE - 1000;
	const value_t WINNING_BONUS = 5000;

	// the draw value is reseved and signales a forced draw (stalemate, repetition)
	const value_t DRAW_VALUE = 1;
}

#endif // __SEARCHDEF_H