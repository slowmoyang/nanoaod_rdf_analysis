#ifndef NANOAOD_RDF_ANALYSIS_UTILS_H_
#define NANOAOD_RDF_ANALYSIS_UTILS_H_

#include "rdf_analysis/rdf_analysis.h"

#include "toml++/toml.h"


using namespace ROOT;


void deserialiseP4Vec(
    std::shared_ptr<RDFAnalysis> analysis,
    const char* prefix);


void serialiseP4Vec(
    std::shared_ptr<RDFAnalysis> analysis,
    const std::string column,
    const std::string new_prefix,
    const bool extend_output_branches);


#endif
