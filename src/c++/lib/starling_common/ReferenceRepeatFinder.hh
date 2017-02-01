// -*- mode: c++; indent-tabs-mode: nil; -*-
//
// Strelka - Small Variant Caller
// Copyright (c) 2009-2016 Illumina, Inc.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//

///
/// \author Sangtae Kim
///

#pragma once

#include <blt_util/blt_types.hh>
#include <blt_util/reference_contig_segment.hh>
#include <vector>

class ReferenceRepeatFinder
{
public:
    ReferenceRepeatFinder(
        const reference_contig_segment& ref,
        const unsigned maxRepeatUnitLength,
        const unsigned maxBufferSize,
        const unsigned minRepeatSpan):
        _ref(ref), _maxRepeatUnitLength(maxRepeatUnitLength),
        _maxBufferSize(maxBufferSize), _minRepeatSpan(minRepeatSpan),
        _repeatSpan(maxBufferSize, std::vector<unsigned>(maxRepeatUnitLength)),
        _isAnchor(maxBufferSize)
    {
    }

    /// checks if pos is an anchor position
    /// \param pos reference position
    /// \return true if pos is an anchor position, false otherwise
    bool isAnchor(pos_t pos) const
    {
        return _isAnchor[pos % _maxBufferSize];
    }

    void initRepeatSpan(pos_t pos);
    void updateRepeatSpan(pos_t pos);

private:
    const reference_contig_segment& _ref;
    const unsigned _maxRepeatUnitLength;
    const unsigned _maxBufferSize;
    const unsigned _minRepeatSpan;

    // to record reference repeat count
    std::vector<std::vector<unsigned>> _repeatSpan;
    std::vector<bool> _isAnchor;
};