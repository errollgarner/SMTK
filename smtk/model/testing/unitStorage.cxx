#include "smtk/model/Storage.h"
#include "smtk/model/ExportJSON.h"
#include "smtk/model/testing/helpers.h"

#include "cJSON.h"

#include <assert.h>

using namespace smtk::util;
using namespace smtk::model;
using namespace smtk::model::testing;

int main(int argc, char* argv[])
{
  (void)argc;
  (void)argv;
  UUIDsToEntities smTopology;
  UUIDsToArrangements smArrangements;
  UUIDsToTessellations smTessellation;
  Storage sm(&smTopology, &smArrangements, &smTessellation);

  UUIDArray uids = createTet(sm);

  unsigned int uc00Flags = sm.findEntity(uids[00])->entityFlags();
  assert( smtk::model::isVertex(uc00Flags)    && "isVertex(vertexFlags) incorrect");
  assert(!smtk::model::isEdge(uc00Flags)      && "isEdge(vertexFlags) incorrect");
  assert(!smtk::model::isFace(uc00Flags)      && "isFace(vertexFlags) incorrect");
  assert(!smtk::model::isRegion(uc00Flags)    && "isRegion(vertexFlags) incorrect");
  assert(!smtk::model::isChain(uc00Flags)     && "isChain(vertexFlags) incorrect");
  assert(!smtk::model::isLoop(uc00Flags)      && "isLoop(vertexFlags) incorrect");
  assert(!smtk::model::isShell(uc00Flags)     && "isShell(vertexFlags) incorrect");
  assert(!smtk::model::isVertexUse(uc00Flags) && "isVertexUse(vertexFlags) incorrect");
  assert(!smtk::model::isEdgeUse(uc00Flags)   && "isEdgeUse(vertexFlags) incorrect");
  assert(!smtk::model::isFaceUse(uc00Flags)   && "isFaceUse(vertexFlags) incorrect");

  UUIDs nodes = sm.entitiesOfDimension(0);
  UUIDs edges = sm.entitiesOfDimension(1);
  UUIDs faces = sm.entitiesOfDimension(2);
  UUIDs zones = sm.entitiesOfDimension(3);

  // Test the methods used to set/get string properties
  sm.setStringProperty(uids[21], "name", "Tetrahedron");
  smtk::model::StringList components;
  components.push_back("vx");
  components.push_back("vy");
  components.push_back("vz");
  sm.setStringProperty(uids[00], "velocity", components);
  sm.stringProperty(uids[21], "name")[0] = "Ignatius";
  sm.stringProperty(uids[21], "name").push_back("J");
  sm.stringProperty(uids[21], "name").push_back("Fumblemumbler");
  sm.setStringProperty(uids[21], "name", "Tetrahedron"); // Resets name to length 1.
  assert(sm.stringProperty(uids[00], "velocity")[0] == "vx");
  assert(sm.stringProperty(uids[00], "velocity")[1] == "vy");
  assert(sm.stringProperty(uids[00], "velocity").size() == 3); // Test multi-entry length.
  assert(sm.stringProperty(uids[21], "velocity").size() == 0); // Test missing entry length.
  assert(sm.stringProperty(uids[21], "name").size() == 1); // Test length of reset property.

  cJSON* root = cJSON_CreateObject();
  ExportJSON::fromModel(root, &sm);
  cJSON_AddItemToObject(root, "nodes", ExportJSON::fromUUIDs(nodes));
  cJSON_AddItemToObject(root, "edges", ExportJSON::fromUUIDs(edges));
  cJSON_AddItemToObject(root, "faces", ExportJSON::fromUUIDs(faces));
  cJSON_AddItemToObject(root, "zones", ExportJSON::fromUUIDs(zones));
  cJSON_AddItemToObject(root, "bdy(brd(uc13,2),1)", ExportJSON::fromUUIDs(sm.boundaryEntities(sm.bordantEntities(uids[13],2),1)));
  cJSON_AddItemToObject(root, "bdy(uc20,2)", ExportJSON::fromUUIDs(sm.boundaryEntities(uids[20],2)));
  cJSON_AddItemToObject(root, "brd(uc20,2)", ExportJSON::fromUUIDs(sm.bordantEntities(uids[20],2)));
  cJSON_AddItemToObject(root, "bdy(uc20,1)", ExportJSON::fromUUIDs(sm.boundaryEntities(uids[20],1)));
  cJSON_AddItemToObject(root, "brd(uc20,3)", ExportJSON::fromUUIDs(sm.bordantEntities(uids[20],3)));
  cJSON_AddItemToObject(root, "lower(uc21,1)", ExportJSON::fromUUIDs(sm.lowerDimensionalBoundaries(uids[21],1)));
  cJSON_AddItemToObject(root, "upper(uc00,3)", ExportJSON::fromUUIDs(sm.higherDimensionalBordants(uids[00],3)));
  std::cout << cJSON_Print(root) << "\n";
  cJSON_Delete(root);

  return 0;
}