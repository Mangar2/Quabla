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
 * @author Volker B�hm
 * @copyright Copyright (c) 2021 Volker B�hm
 */

#include "rootmoves.h"

using namespace ChessSearch;

void RootMove::init() {
	_valueOfLastSearch = -MAX_VALUE;

	_alphaOfLastSearch = -MAX_VALUE;
	_betaOfLastSearch = MAX_VALUE;

	_pvDepthOfLastSearch = 0;
	_depthOfLastSearch = 0;

	_nodeCountOfLastSearch = 0;
	_totalNodeCount = 0;
	_totalTableBaseHits = 0;
	_totalBitbaseHits = 0;
	_timeSpendToSearchMoveInMilliseconds = 0;

	_pvString = "";
	_isExcluded = false;
}

void RootMove::set(const SearchVariables& variables, uint64_t totalNodes, uint64_t tableBaseHits,
	uint64_t timeSpentInMilliseconds, const string& pvString, const PV& pvLine)
{
	_valueOfLastSearch = variables.bestValue;
	_alphaOfLastSearch = variables.alpha;
	_betaOfLastSearch = variables.beta;
	// _pvDepthOfLastSearch = variables.isInPV()
}



bool RootMove::doSearch(const SearchVariables& variables) const {
	if (_isExcluded) {
		return false;
	}
	if (_depthOfLastSearch < variables.remainingDepthAtPlyStart) {
		return true;
	}
	// If evaluated value was outside the window and is now inside the window, we need to search again
	if (_valueOfLastSearch >= _betaOfLastSearch && variables.beta > _betaOfLastSearch) {
		return true;
	}
	if (_valueOfLastSearch <= _alphaOfLastSearch && variables.alpha < _alphaOfLastSearch) {
		return true;
	}
	return false;
}

bool RootMove::operator<(const RootMove& rootMove) const {
}

bool RootMove::operator>(const RootMove& rootMove) const {

}
