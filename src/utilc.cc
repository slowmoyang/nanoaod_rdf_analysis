#include "nanoaod_rdf_analysis/utils.h"

#include "fmt/core.h"

#include "Math/Vector4D.h"
#include "Math/Vector4Dfwd.h"
#include "Math/PtEtaPhiM4D.h"


using namespace ROOT::Math; // PtEtaPhiMVector

// deserialise Nanoaod four momentum branches
void deserialiseP4Vec(std::shared_ptr<RDFAnalysis> analysis,
                      const char* prefix) {
  const auto expr = fmt::format(
      "Construct<ROOT::Math::PtEtaPhiMVector>({0}_pt, {0}_eta, {0}_phi, {0}_mass)",
      prefix
  );
  const auto name = fmt::format("{}_{}", prefix, "p4");
  analysis->define(name, expr);
}


void serialiseP4Vec(
    std::shared_ptr<RDFAnalysis> analysis,
    const std::string column,
    const std::string new_prefix,
    const bool extend_output_branches) {

  auto serialise_p4_vec = [](const RVec<PtEtaPhiMVector>& p4_vec) {
    const size_t size = p4_vec.size();

    std::array<RVec<float>, 4> output;
    for (RVec<float>& each : output) {
      each.reserve(size);
    }

    for (const PtEtaPhiMVector& p4 : p4_vec) {
      output.at(0).push_back(p4.pt());
      output.at(1).push_back(p4.eta());
      output.at(2).push_back(p4.phi());
      output.at(3).push_back(p4.mass());
    }
    return output;
  };

  const std::string tmp_column = column + "_serialised";

  analysis->define(tmp_column, serialise_p4_vec, {column});

  const std::vector<std::string> suffix_vec = {"pt", "eta", "phi", "mass"};
  for (size_t idx = 0; idx < suffix_vec.size(); idx++) {
    const std::string new_column = new_prefix + "_" + suffix_vec.at(idx);
    const std::string expr = tmp_column + "[" + std::to_string(idx) + "]";
    analysis->define(new_column, expr);

    if (extend_output_branches) {
      analysis->extendOutputBranches({new_column});
    }
  }

}
