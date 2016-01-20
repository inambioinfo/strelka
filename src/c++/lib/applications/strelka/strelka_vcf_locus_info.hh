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
/// \author Chris Saunders
///

#pragma once

#include "calibration/VariantScoringModel.hh"
#include <cassert>

#include <bitset>
#include <iosfwd>

#include "strelkaScoringFeatures.hh"


namespace STRELKA_VCF_FILTERS
{

enum index_t
{
    // SNVs and indels:
    HighDepth,
    LowEVS,
    // SNVs only:
    BCNoise,
    SpanDel,
    QSS_ref,
    // indels only:
    Repeat,
    iHpol,
    IndelBCNoise,
    QSI_ref,
    Nonref,
    SIZE
};

inline
const char*
get_label(const unsigned idx)
{
    switch (idx)
    {
    case HighDepth:
        return "HighDepth";
        //return "DP"; // old strelka workflow name
    case BCNoise:
        return "BCNoise";
    case SpanDel:
        return "SpanDel";
    case QSS_ref:
        return "QSS_ref";
    case Repeat:
        return "Repeat";
    case iHpol:
        return "iHpol";
    case IndelBCNoise:
        return "BCNoise";
    case QSI_ref:
        return "QSI_ref";
    case LowEVS:
        return "LowEVS";
    case Nonref:
        return "Nonref";
    default:
        assert(false && "Unknown vcf filter id");
        return nullptr;
    }
}
}

template<class _evs_featureset = STRELKA_SNV_SCORING_FEATURES>
struct strelka_shared_modifiers
{
    strelka_shared_modifiers()
    {
        clear();
    }

    void
    set_filter(const STRELKA_VCF_FILTERS::index_t i)
    {
        filters.set(i);
    }

    void
    set_feature(const typename _evs_featureset::index_t i,double val)
    {
        if (_isFeatureSet.test(i))
        {
            assert(false && "Set scoring feature twice");
        }
        _featureVal[i] = val;
        _isFeatureSet.set(i);
    }

    double
    get_feature(const typename _evs_featureset::index_t i) const
    {
        assert(_isFeatureSet.test(i));
        return this->_featureVal.at(i);
    }

    bool
    test_feature(const typename _evs_featureset::index_t i) const
    {
        return _isFeatureSet[i];
    }

    const feature_type&
    get_features() const
    {
        return _featureVal;
    }

    inline
    void
    write_filters(
        std::ostream& os) const
    {
        if (filters.none())
        {
            os << "PASS";
            return;
        }

        bool is_sep(false);
        for (unsigned i(0); i<STRELKA_VCF_FILTERS::SIZE; ++i)
        {
            if (! filters.test(i)) continue;

            if (is_sep)
            {
                os << ";";
            }
            else
            {
                is_sep=true;
            }
            os << STRELKA_VCF_FILTERS::get_label(i);
        }
    }

    /** write features to INFO field */
    void
    write_features(
        std::ostream& os) const
    {
        int ix = 0;
        for (auto const& val : _featureVal)
        {
            if (ix > 0)
            {
                os << ",";
            }
            os << _evs_featureset::get_feature_label(val.first) << ":" << val.second;
            ++ix;
        }
    }

    void
    clear()
    {
        filters.reset();
        _isFeatureSet.reset();
        _featureVal.clear();
        isEVS=false;
        EVS = 0;
    }

    std::bitset<STRELKA_VCF_FILTERS::SIZE> filters;
    bool isEVS;
    double EVS;

private:
    std::bitset<_evs_featureset::SIZE> _isFeatureSet;
    feature_type _featureVal; // holds scoring features
};


template<class _evs_featureset>
std::ostream&
operator<<(
    std::ostream& os,
    const strelka_shared_modifiers<_evs_featureset>& shmod)
{
    os << " filters: ";
    shmod.write_filters(os);

    return os;
}

typedef strelka_shared_modifiers<STRELKA_SNV_SCORING_FEATURES> strelka_shared_modifiers_snv;
typedef strelka_shared_modifiers<STRELKA_INDEL_SCORING_FEATURES> strelka_shared_modifiers_indel;

