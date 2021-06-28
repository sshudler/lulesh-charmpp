#include <string>

#include "lulesh_insitu.h"



void CharmAscentData::pup(PUP::er& p) {
  if(p.isUnpacking()) {
    std::string st_data;
    p|st_data;
    // CkPrintf("st_data unpacking: %s\n", st_data.c_str());
    // CkPrintf("st_data unpacking\n");
    m_condNode.parse(st_data, "conduit_base64_json");
    // bool flag = m_cond_node["topologies/mesh/elements/dims/i"].dtype().is_int32();
    // CkPrintf("m_cond_node is int32: %d\n", flag);
  }
  else {
    std::string st_data = m_condNode.to_string("conduit_base64_json");
    // CkPrintf("st_data packing: %s\n", st_data.c_str());
    // CkPrintf("st_data packing\n");
    p|st_data;
  }
}


InSitu::InSitu() {
  // usesAtSync = true;
  setMigratable(false);
}


InSitu::InSitu(CkMigrateMessage *msg) : CBase_InSitu(msg) {
  // usesAtSync = true;
  delete msg;
}


void 
InSitu::pup(PUP::er &p) {
  CBase_InSitu::pup(p);

  // CkPrintf("pup, thisIndex = %d\n", thisIndex);

  p|m_ascentData;
  p|m_statusEp;
  p|m_statusId;
}


void 
InSitu::ExecuteAscent(bool init_phase, int inner_chare_aid) {

  // conduit::Node ascent_opt;
  // ascent_opt["mpi_comm"] = MPI_Comm_c2f(MPI_COMM_WORLD);
  // ascent_opt["runtime/type"] = "ascent";
  // m_ascent.open(ascent_opt);

  ascent::Ascent ascent;
  ascent.open();
  // m_ascent.publish(m_ascent_node);

  /////
  // std::vector<double> isovals({150000.0, 200000.0, 250000.0});
  // std::vector<double> isovals({150000.0});
  std::vector<double> isovals({3.0});
  std::vector<int64_t> radix_v({2, 4});   // Assume 8 chares

  conduit::Node extract;
  extract["e1/type"] = "bflow_iso";
  extract["e1/params/field"] = "p";
  extract["e1/params/iso_values"].set_float64_vector(isovals);
  extract["e1/params/image_name"] = "iso_img";
  extract["e1/params/width"] = 1024;
  extract["e1/params/height"] = 1024;
  extract["e1/params/radices"].set_int64_vector(radix_v);
  if (init_phase) {
    int ep = CkIndex_InSitu::InSituCreated(NULL);
    CkArrayID aid = thisProxy;
    int aid_v = CkGroupID(aid).idx;
    extract["e1/params/ascent_chare_ep"] = ep;
    extract["e1/params/ascent_chare_aid"] = aid_v;

    extract["e1/params/ascent_status_ep"] = m_statusEp;
    extract["e1/params/ascent_status_aid"] = m_statusId;
  }
  else {
    extract["e1/params/inner_chare_aid"] = inner_chare_aid;

    extract["e2/type"] = "relay";
    extract["e2/params/path"] = "output";
    extract["e2/params/protocol"] = "blueprint/mesh/hdf5";
  }

  conduit::Node actions;

  conduit::Node &add_extract = actions.append();
  add_extract["action"] = "add_extracts";
  add_extract["extracts"] = extract;

  conduit::Node &execute = actions.append();
  execute["action"] = "execute";

  ////
  // m_ascent_data.m_cond_node["state/chare_msg"] = (uint64_t)m;
  ////

  ascent.publish(m_ascentData.m_condNode);
  ascent.execute(actions);

  ascent.close();
  /////

}


void
InSitu::CreateInSituChares(const CharmAscentData& data, int status_ep, int status_id) {

  m_ascentData = data;
  m_statusEp = status_ep;
  m_statusId = status_id;

  if (m_ascentData.m_condNode["state/domain_id"].as_int32() == 0)
    ExecuteAscent(true);
}


void
InSitu::InSituCreated(CkArrayCreatedMsg *m) {

  CkArrayID aid = thisProxy;
  int aid_v = CkGroupID(m->aid).idx;
  ExecuteAscent(false, aid_v);
}


#include "insitu.def.h"
