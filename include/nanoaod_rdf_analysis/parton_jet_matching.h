#ifndef NANOAOD_RDF_ANALYSIS_PARTON_JET_MATCHING_H_
#define NANOAOD_RDF_ANALYSIS_PARTON_JET_MATCHING_H_

#include "Math/Vector4Dfwd.h"
#include "Math/GenVector/PtEtaPhiM4D.h"
#include "ROOT/RVec.hxx"

#include <cstdint>

using namespace ROOT; // RVec
using namespace ROOT::Math; // PtEtaPhiMVector


// Returns the indices of jets matched to partons
//
// match parton to jet with shortest distance
// starting with the shortest distance available
// apply some outlier rejection if desired
// See https://github.com/cms-sw/cmssw/blob/CMSSW_14_1_0_pre4/TopQuarkAnalysis/TopTools/src/JetPartonMatching.cc#L149-L196
RVec<int64_t> matchPartonToJet(
    const RVec<PtEtaPhiMVector>& parton_vec,
    const RVec<PtEtaPhiMVector>& jet_vec,
    const double max_distance
);

RVec<int64_t> convertPartonJetIdxToJetPartonIdx(
    const RVec<int64_t>& parton_jet_idx_vec,
    const size_t jet_vec_size
);

#endif // NANOAOD_RDF_ANALYSIS_PARTON_JET_MATCHING_H_
