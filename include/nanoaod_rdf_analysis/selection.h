#ifndef NANOAOD_RDF_ANALYSIS_SELECTION_H_
#define NANOAOD_RDF_ANALYSIS_SELECTION_H_

#include "rdf_analysis/rdf_analysis.h"


void runJetSelection(std::shared_ptr<RDFAnalysis> analysis,
                     const float jet_pt_min,
                     const float jet_abs_eta_max,
                     const uint8_t jet_id,
                     const std::string btag_algo,
                     const float btag_wp);

#endif
