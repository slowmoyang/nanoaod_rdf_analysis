#include "nanoaod_rdf_analysis/parton_jet_matching.h"

#include "Math/GenVector/VectorUtil.h"
#include <cstdint>

using namespace ROOT; // RVec
using namespace ROOT::Math; // PtEtaPhiMVector


RVec<int64_t> matchPartonToJet(
    const RVec<PtEtaPhiMVector>& parton_vec,
    const RVec<PtEtaPhiMVector>& jet_vec,
    const double max_distance
) {

  // prepare vector of pairs with distances between
  // all partons to all jets in the input vectors
  std::vector<std::tuple<double, size_t, size_t> > match_candidate_vec;
  match_candidate_vec.reserve(parton_vec.size() * jet_vec.size());

  for (size_t parton_idx = 0; parton_idx < parton_vec.size(); ++parton_idx) {
    for (size_t jet_idx = 0; jet_idx < jet_vec.size(); ++jet_idx) {
      const double distance = VectorUtil::DeltaR(jet_vec.at(jet_idx), parton_vec.at(parton_idx));
      match_candidate_vec.emplace_back(distance, parton_idx, jet_idx);
    }
  }
  std::sort(match_candidate_vec.begin(), match_candidate_vec.end());

  // parton index to jet index
  RVec<int64_t> parton_jet_idx(parton_vec.size(), -1);

  while (not match_candidate_vec.empty()) {
    const auto& [distance, parton_idx, jet_idx] = match_candidate_vec.at(0);
    // use primitive outlier rejection if desired
    parton_jet_idx.at(parton_idx) = distance <= max_distance ? jet_idx : -1;

    // remove all values for the matched parton and the matched jet
    for (size_t other_idx = 0; other_idx < match_candidate_vec.size(); ++other_idx) {
      const auto& [other_distance, other_parton_idx, other_jet_idx] = match_candidate_vec.at(other_idx);
      if ((other_parton_idx == parton_idx) or (other_jet_idx == jet_idx)) {
        match_candidate_vec.erase(match_candidate_vec.begin() + other_idx);
        --other_idx;
      }
    }
  }

  return parton_jet_idx;
}

// TODO check if a jet is already seen
RVec<int64_t> convertPartonJetIdxToJetPartonIdx(
    const RVec<int64_t>& parton_jet_idx_vec,
    const size_t jet_vec_size) {

  RVec<int64_t> jet_parton_idx_vec(jet_vec_size, -1);

  const int64_t parton_vec_size = static_cast<int64_t>(parton_jet_idx_vec.size());

  for (int64_t parton_idx = 0; parton_idx < parton_vec_size; parton_idx++) {
    const int64_t parton_jet_idx = parton_jet_idx_vec.at(parton_idx);
    if (parton_jet_idx >= 0) {
      jet_parton_idx_vec.at(parton_jet_idx) = parton_idx;
    }
  }

  return jet_parton_idx_vec;
}
