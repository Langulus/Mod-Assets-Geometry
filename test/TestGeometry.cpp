///                                                                           
/// Langulus::Module::Assets::Geometry                                        
/// Copyright (c) 2016 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: GPL-3.0-or-later                                 
///                                                                           
#include <Langulus/Mesh.hpp>
#include <Langulus/Testing.hpp>


SCENARIO("Loading non-existent file", "[mesh]") {
   static Allocator::State memoryState;

   for (int repeat = 0; repeat != 10; ++repeat) {
      GIVEN(std::string("Init and shutdown cycle #") + std::to_string(repeat)) {
         // Create root entity                                          
         auto root = Thing::Root<false>(
            "FileSystem",
            "AssetsGeometry"
         );

         WHEN("The mesh is created via abstractions") {
            REQUIRE_THROWS(root.CreateUnit<A::Mesh>("nonexistent.obj"));
            REQUIRE(root.GetUnits().IsEmpty());
         }
         
      #if LANGULUS_FEATURE(MANAGED_REFLECTION)
         WHEN("The mesh is created via tokens") {
            REQUIRE_THROWS(root.CreateUnitToken("Mesh", "nonexistent.obj"));
            REQUIRE(root.GetUnits().IsEmpty());
         }
      #endif

         // Check for memory leaks after each cycle                     
         REQUIRE(memoryState.Assert());
      }
   }
}

SCENARIO("Mesh creation", "[mesh]") {
   static Allocator::State memoryState;

   for (int repeat = 0; repeat != 10; ++repeat) {
      GIVEN(std::string("Init and shutdown cycle #") + std::to_string(repeat)) {
         // Create root entity                                          
         auto root = Thing::Root<false>(
            "FileSystem",
            "AssetsGeometry"
         );

         WHEN("The mesh is created via abstractions") {
            auto producedMesh = root.CreateUnit<A::Mesh>(Math::Box2 {});

            // Update once                                              
            root.Update({});
            root.DumpHierarchy();

            REQUIRE(producedMesh.GetCount() == 1);
            REQUIRE(producedMesh.CastsTo<A::Mesh>(1));
            REQUIRE(producedMesh.IsSparse());
            REQUIRE(root.GetUnits().GetCount() == 1);

            THEN("Generate a LOD level, that should be the same") {
               auto sameMesh = producedMesh.As<A::Mesh>().GetLOD({});
               root.DumpHierarchy();

               REQUIRE(sameMesh == producedMesh.As<A::Mesh*>());
            }
         }
         
      #if LANGULUS_FEATURE(MANAGED_REFLECTION)
         WHEN("The mesh is created via tokens") {
            auto producedMesh = root.CreateUnitToken("Mesh", Math::Box2 {});

            // Update once                                              
            root.Update({});
            root.DumpHierarchy();

            REQUIRE(producedMesh.GetCount() == 1);
            REQUIRE(producedMesh.CastsTo<A::Mesh>());
            REQUIRE(producedMesh.IsSparse());
            REQUIRE(root.GetUnits().GetCount() == 1);
         }
      #endif

         // Check for memory leaks after each cycle                     
         REQUIRE(memoryState.Assert());
      }
   }
}

SCENARIO("Loading OBJ file", "[mesh]") {
   static Allocator::State memoryState;

   for (int repeat = 0; repeat != 10; ++repeat) {
      GIVEN(std::string("Init and shutdown cycle #") + std::to_string(repeat)) {
         // Create root entity                                          
         auto root = Thing::Root<false>(
            "FileSystem",
            "AssetsGeometry",
            "AssetsImages"
         );

         WHEN("The mesh is created via abstractions") {
            auto producedMesh = root.CreateUnit<A::Mesh>("maxwell/maxwell.obj");

            // Update once                                              
            root.Update({});
            root.DumpHierarchy();

            REQUIRE(producedMesh.GetCount() == 1);
            REQUIRE(producedMesh.CastsTo<A::Mesh>(1));
            REQUIRE(producedMesh.IsSparse());
            REQUIRE(root.GetUnits().GetCount() == 1);
         }
         
      #if LANGULUS_FEATURE(MANAGED_REFLECTION)
         WHEN("The mesh is created via tokens") {
            auto producedMesh = root.CreateUnitToken("Mesh", "maxwell/maxwell.obj");

            // Update once                                              
            root.Update({});
            root.DumpHierarchy();

            REQUIRE(producedMesh.GetCount() == 1);
            REQUIRE(producedMesh.CastsTo<A::Mesh>());
            REQUIRE(producedMesh.IsSparse());
            REQUIRE(root.GetUnits().GetCount() == 1);
         }
      #endif

         // Check for memory leaks after each cycle                     
         REQUIRE(memoryState.Assert());
      }
   }
}
