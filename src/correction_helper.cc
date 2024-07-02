#include "nanoaod_rdf_analysis/correction_helper.h"

#include "rdf_analysis/rdf_analysis.h"
#include "nanoaod_tools/jet_energy_resolution.h"
#include "nanoaod_tools/jet_energy_scale.h"
#include "nanoaod_tools/lumi_mask.h"
#include "nanoaod_tools/jet_veto_map.h"
#include "nanoaod_tools/btagging.h"

#include "ROOT/RVec.hxx"
#include "Math/Vector4D.h"
#include "Math/Vector4Dfwd.h"
#include "Math/PtEtaPhiM4D.h"

#include <string>
#include <iterator>
#include <numeric>

using namespace ROOT;
using namespace correction;


// input: Jet_p4
// output: Jet_p4_jes
// Jet_p4_jes is an alias for Jet_p4
// TODO doc
void runJetEnergyScale(std::shared_ptr<RDFAnalysis> analysis) {
  analysis->alias("Jet_p4_jes", "Jet_p4");
}


// define nodes for JES correciton factor and JES-crrected jets
// input columns: "Jet_p4_jes", "Jet_genJetIdx", "GenJet_p4", "Rho_fixedGridRhoFastjetAll"
// output columns: "Jet_p4_jes_jer"
// intermediate columns: "Jet_c_jer"
void runJetEnergyResolution(
    std::shared_ptr<RDFAnalysis> analysis,
    Correction::Ref res_corr,
    Correction::Ref sf_corr,
    const std::string systematic) {

  auto prng = std::make_shared<TRandom3>(12345); // FIXME

  auto get_c_jer = [res_corr, sf_corr, systematic, prng](
      const RVec<PtEtaPhiMVector>& rec_jet_vec,
      const RVec<int16_t>& rec_jet_gen_jet_idx_vec,
      const RVec<PtEtaPhiMVector>& gen_jet_vec,
      const float rho
  ) -> RVec<float> {

    return getJetEnergyResolutionCorrectionFactor(
        rec_jet_vec,
        rec_jet_gen_jet_idx_vec,
        gen_jet_vec,
        rho,
        res_corr,
        sf_corr,
        systematic,
        prng
    );
  };

  analysis->define(
      /*name=*/"Jet_c_jer",
      /*expr=*/get_c_jer,
      /*columns=*/{
          "Jet_p4_jes",
          "Jet_genJetIdx",
          "GenJet_p4",
          "Rho_fixedGridRhoFastjetAll" // FIXME
      }
  );

  analysis->define(
      "Jet_p4_jes_jer",
      "Jet_c_jer * Jet_p4_jes"
  );
}


// input columns: "Jet_p4_jes_jer"
// output columns: "Jet_p4_jes_jer_jesunc"
// intermediate columns: "Jet_c_jes_unc"
void runJetEnergyScaleUncertainty(std::shared_ptr<RDFAnalysis> analysis,
                       const Correction::Ref jes_unc_correction,
                       const std::string systematic) {

  auto get_correction_factor = [jes_unc_correction, systematic](
      const RVec<PtEtaPhiMVector> jet_vec
  ) -> RVec<float> {
    return getJESUncertaintyCorrectionFactor(
        jet_vec, jes_unc_correction, systematic);
  };
  analysis->define("Jet_c_jes_unc", get_correction_factor, {"Jet_p4_jes_jer"});
  analysis->define("Jet_p4_jes_jer_jesunc", "Jet_c_jes_unc * Jet_p4_jes_jer");
}


// define a node to define lumi mask using golden json
// FIXME
void runLumiMask(std::shared_ptr<RDFAnalysis> analysis,
                 std::shared_ptr<LumiMask> lumi_mask) {

  auto run_lumi_mask = [lumi_mask](const uint32_t run,
                                   const uint32_t luminosity_block
  ) -> bool {
    return lumi_mask->select(run, luminosity_block);
  };

  analysis->filter(run_lumi_mask, {"run", "luminosityBlock"}, "Lumi Mask");
}


void runJetVetoMap(std::shared_ptr<RDFAnalysis> analysis,
                   Correction::Ref jet_veto_map,
                   const std::string jet_veto_map_type) {

  auto get_jet_veto_map = [jet_veto_map, jet_veto_map_type](
      const RVec<float> &eta_vec,
      const RVec<float> &phi_vec) {
    return getJetVetoMap(jet_veto_map, jet_veto_map_type, eta_vec, phi_vec);
  };

  analysis->filter(get_jet_veto_map, {"Jet_eta", "Jet_phi"}, "Jet Veto Map");
}


void runNoiseFilter(std::shared_ptr<RDFAnalysis> analysis,
                   const toml::array* noise_filter_arr) {
  std::string expr = "";
  for (uint32_t idx = 0; idx < noise_filter_arr->size(); idx++) {
    if (idx > 0) {
      expr += " & ";
    }
    const std::string token = noise_filter_arr->at(idx).value_or("");
    if (token.empty()) {
      throw std::runtime_error("got a zero-length noise filter");
    }
    expr += token;
  }

  analysis->filter(expr, "Noise Filter");
}


void runPileupWeighting(
    std::shared_ptr<RDFAnalysis> analysis,
    Correction::Ref pileup_weighting,
    const std::string pileup_weighting_type)  {

  auto run_pileup_weighting_sf = [pileup_weighting, pileup_weighting_type](
      const float num_true_interactions) {
    return pileup_weighting->evaluate(
        {num_true_interactions, pileup_weighting_type});
  };

  analysis->define(
      "weight_pileup_weighting",
      run_pileup_weighting_sf,
      {"Pileup_nTrueInt"},
      /*extend=*/true);
}



// https://btv-wiki.docs.cern.ch/ScaleFactors/
void runBTaggingFixedWPCorrection(std::shared_ptr<RDFAnalysis> analysis,
                                  std::shared_ptr<const Correction> correction_bc,
                                  std::shared_ptr<const Correction> correction_light,
                                  const std::string systematic,
                                  const std::string working_point) {
  auto define_weight_vec = [correction_bc, correction_light, systematic, working_point](
      const RVec<uint8_t>& jet_hadron_flavour_vec,
      const RVec<PtEtaPhiMVector>& jet_p4_vec
  ) -> RVec<float> {
    return getBTaggingFixedWPCorrection(
        jet_hadron_flavour_vec,
        jet_p4_vec,
        correction_bc,
        correction_light,
        systematic,
        working_point
    );
  };

  const std::string jet_weight_branch = "jet_weight_btag_wp";
  const std::string weight_branch = "weight_btag_wp";

  analysis->define(jet_weight_branch,
                   define_weight_vec,
                   {"jet_hadron_flavour", "jet_p4"});

  analysis->define(
      weight_branch,
      [](const RVec<float> vec) -> float {
        return std::accumulate(
            std::begin(vec),
            std::end(vec),
            1.0f,
            std::multiplies<float>()
        );
      },
      {"jet_weight_btag_wp"}
  );

  analysis->extendOutputBranches({jet_weight_branch, weight_branch});
}


void runBTaggingShapeCorrection(std::shared_ptr<RDFAnalysis> analysis,
                                Correction::Ref correction,
                                const std::string systematic) {

  auto define_weight_vec = [correction, systematic](
      const RVec<uint8_t>& jet_hadron_flavour_vec,
      const RVec<PtEtaPhiMVector>& jet_p4_vec,
      const RVec<float>& jet_btag_discriminant_vec
  ) -> RVec<float> {
    return getBTaggingShapeCorrection(
        jet_hadron_flavour_vec,
        jet_p4_vec,
        jet_btag_discriminant_vec,
        correction,
        systematic);
  };

  analysis->define("jet_weight_btag_shape", define_weight_vec,
                   {"jet_hadron_flavour", "jet_p4", "jet_btag_discriminant"},
                   /*extend=*/true);

  analysis->define(
      "weight_btag_shape",
      [](const RVec<float> vec) -> float {
        return std::accumulate(
            std::begin(vec),
            std::end(vec),
            1.0f,
            std::multiplies<float>()
        );
      },
      {"jet_weight_btag_shape"},
      /*extend=*/true);
}
