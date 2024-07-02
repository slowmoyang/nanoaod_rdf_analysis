#include "nanoaod_rdf_analysis/selection.h"
#include "nanoaod_rdf_analysis/utils.h"

#include "rdf_analysis/rdf_analysis.h"

#include "fmt/core.h"

#include <string>

using namespace ROOT;


const std::map<std::string, std::string> kBTagAlgoBranchMap = {
    {"robustParticleTransformer", "RobustParT"},
};


// input columns: "Jet_corr_pt", "Jet_corr_eta", "Jet_jetId", "Jet_btag{}AK4B",
//                "Jet_hadronFlavour"
//
// output columns: "jet_mask", "jet_p4", "jet_btag_discriminant", "jet_btag",
//                 "jet_hadron_flavour", "jet_size", "jet_pt", "jet_eta",
//                 "jet_phi", "jet_mass"
void runJetSelection(std::shared_ptr<RDFAnalysis> analysis,
                     const float jet_pt_min,
                     const float jet_abs_eta_max,
                     const uint8_t jet_id,
                     const std::string btag_algo,
                     const float btag_wp) {

  const auto jet_mask_expr = fmt::format(
      "(Jet_corr_pt > {:f}) && (abs(Jet_corr_eta) < {:f}) && (Jet_jetId == {:d})",
      jet_pt_min, jet_abs_eta_max, jet_id);

  const auto btag_algo_branch = kBTagAlgoBranchMap.at(btag_algo);

  const auto jet_btag_disc_expr = fmt::format(
      "Jet_btag{}AK4B[jet_mask]",
      btag_algo_branch);

  const auto jet_btag_expr = fmt::format("jet_btag_discriminant > {}",
                                         btag_wp);

  // corrected and selected jets
  analysis->define("jet_mask", jet_mask_expr);
  analysis->define("jet_p4", "Jet_corr_p4[jet_mask]");
  analysis->define("jet_btag_discriminant", jet_btag_disc_expr, true);
  analysis->define("jet_btag", jet_btag_expr, true);
  analysis->define("jet_hadron_flavour", "Jet_hadronFlavour[jet_mask]");
  analysis->define("jet_size", "jet_p4.size()");

  serialiseP4Vec(analysis, "jet_p4", "jet", true);
}
