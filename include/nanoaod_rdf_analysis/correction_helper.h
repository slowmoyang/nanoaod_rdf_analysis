#ifndef NANOAOD_RDF_ANALYSIS_CORRECTION_HELPER_H_
#define NANOAOD_RDF_ANALYSIS_CORRECTION_HELPER_H_

#include "rdf_analysis/rdf_analysis.h"
#include "nanoaod_tools/lumi_mask.h"

#include "correction.h"
#include "toml++/toml.hpp"


using namespace correction;


void runJetEnergyScale(std::shared_ptr<RDFAnalysis> analysis);


void runJetEnergyResolution(
    std::shared_ptr<RDFAnalysis> analysis,
    Correction::Ref res_corr,
    Correction::Ref sf_corr,
    const std::string systematic);


void runJetEnergyScaleUncertainty(
    std::shared_ptr<RDFAnalysis> analysis,
    const Correction::Ref jes_unc_correction,
    const std::string systematic);


// define a node to define lumi mask using golden json
void runLumiMask(std::shared_ptr<RDFAnalysis> analysis,
                 std::shared_ptr<LumiMask> lumi_mask);


void runJetVetoMap(std::shared_ptr<RDFAnalysis> analysis,
                   Correction::Ref jet_veto_map,
                   const std::string jet_veto_map_type);


void runNoiseFilter(std::shared_ptr<RDFAnalysis> analysis,
                    const toml::array* noise_filter_arr);


void runPileupWeighting(
    std::shared_ptr<RDFAnalysis> analysis,
    Correction::Ref pileup_weighting,
    const std::string pileup_weighting_type);


void runBTaggingFixedWPCorrection(std::shared_ptr<RDFAnalysis> analysis,
                                  std::shared_ptr<const Correction> correction_bc,
                                  std::shared_ptr<const Correction> correction_light,
                                  const std::string systematic,
                                  const std::string working_point);

void runBTaggingShapeCorrection(std::shared_ptr<RDFAnalysis> analysis,
                                Correction::Ref correction,
                                const std::string systematic);

#endif
