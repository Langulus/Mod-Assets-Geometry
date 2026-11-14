///                                                                           
/// Langulus::Module::Art::Geometry                                           
/// Copyright (c) 2016 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include <Langulus/CppAPI/Mesh.hpp>
#include <Langulus/Testing.hpp>
#include <Langulus/Allocator.hpp>
#include <Langulus/Primitives/TBox.hpp>

using namespace Langulus;
using Things::Thing;


SCENARIO("Loading non-existent file") {
   static MemoryState memoryState;

   GIVEN("Init and shutdown cycles") {
      for (int repeat = 0; repeat != 10; ++repeat) {
         // Create root entity                                          
         auto root = Thing::Root<false>(
            "FileSystem",
            "AssetsGeometry"
         );

         WHEN("The mesh is created via abstractions") {
            REQUIRE_THROWS(root.CreatePart<Things::Mesh>("nonexistent.obj"));
            REQUIRE(root.GetParts().IsEmpty());
         }
         
      #if LANGULUS_FEATURE(MANAGED_REFLECTION)
         WHEN("The mesh is created via tokens") {
            REQUIRE_THROWS(root.CreatePartToken("Mesh", "nonexistent.obj"));
            REQUIRE(root.GetParts().IsEmpty());
         }
      #endif

         REQUIRE(memoryState.Assert());
         REQUIRE_FALSE(Allocator::CollectGarbage());
      }
   }
}

SCENARIO("Mesh creation") {
   static MemoryState memoryState;

   GIVEN("Init and shutdown cycles") {
      for (int repeat = 0; repeat != 10; ++repeat) {
         // Create root entity                                          
         auto root = Thing::Root<false>(
            "FileSystem",
            "AssetsGeometry"
         );

         WHEN("The mesh is created via abstractions") {
            auto producedMesh = root.CreatePart<Things::Mesh>(Math::Box2 {});

            // Update once                                              
            root.Update({});
            root.DumpHierarchy();

            REQUIRE(producedMesh.GetCount() == 1);
            REQUIRE(producedMesh.CastsTo<Things::Mesh>(1));
            REQUIRE(producedMesh.IsSparse());
            REQUIRE(root.GetParts().GetCount() == 1);

            THEN("Generate a LOD level, that should be the same") {
               auto sameMesh = producedMesh.As<Things::Mesh>().GetLOD({});
               root.DumpHierarchy();

               REQUIRE(sameMesh == producedMesh.As<Things::Mesh*>());
            }
         }
         
      #if LANGULUS_FEATURE(MANAGED_REFLECTION)
         WHEN("The mesh is created via tokens") {
            auto producedMesh = root.CreatePartToken("Mesh", Math::Box2 {});

            // Update once                                              
            root.Update({});
            root.DumpHierarchy();

            REQUIRE(producedMesh.GetCount() == 1);
            REQUIRE(producedMesh.CastsTo<Things::Mesh>());
            REQUIRE(producedMesh.IsSparse());
            REQUIRE(root.GetParts().GetCount() == 1);
         }
      #endif

         REQUIRE(memoryState.Assert());
         REQUIRE_FALSE(Allocator::CollectGarbage());
      }
   }
}

SCENARIO("Loading OBJ file") {
   static MemoryState memoryState;

   GIVEN("Init and shutdown cycles") {
      for (int repeat = 0; repeat != 10; ++repeat) {
         // Create root entity                                          
         auto root = Thing::Root<false>(
            "FileSystem",
            "AssetsGeometry",
            "AssetsImages"
         );

         WHEN("The mesh is created via abstractions") {
            auto producedMesh = root.CreatePart<Things::Mesh>("maxwell/maxwell.obj");

            // Update once                                              
            root.Update({});
            root.DumpHierarchy();

            REQUIRE(producedMesh.GetCount() == 1);
            REQUIRE(producedMesh.CastsTo<Things::Mesh>(1));
            REQUIRE(producedMesh.IsSparse());
            REQUIRE(root.GetParts().GetCount() == 1);
         }
         
      #if LANGULUS_FEATURE(MANAGED_REFLECTION)
         WHEN("The mesh is created via tokens") {
            auto producedMesh = root.CreatePartToken("Mesh", "maxwell/maxwell.obj");

            // Update once                                              
            root.Update({});
            root.DumpHierarchy();

            REQUIRE(producedMesh.GetCount() == 1);
            REQUIRE(producedMesh.CastsTo<Things::Mesh>());
            REQUIRE(producedMesh.IsSparse());
            REQUIRE(root.GetParts().GetCount() == 1);
         }
      #endif

         REQUIRE(memoryState.Assert());
         REQUIRE_FALSE(Allocator::CollectGarbage());
      }
   }
}
