///                                                                           
/// Langulus::Module::Assets::Geometry                                        
/// Copyright(C) 2016 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#include "Main.hpp"
#include <Langulus/Mesh.hpp>
#include <catch2/catch.hpp>

/// See https://github.com/catchorg/Catch2/blob/devel/docs/tostring.md        
CATCH_TRANSLATE_EXCEPTION(::Langulus::Exception const& ex) {
   const Text serialized {ex};
   return ::std::string {Token {serialized}};
}

SCENARIO("Loading non-existent file", "[mesh]") {
   static Allocator::State memoryState;

   for (int repeat = 0; repeat != 10; ++repeat) {
      GIVEN(std::string("Init and shutdown cycle #") + std::to_string(repeat)) {
         // Create root entity                                          
         Thing root;
         root.SetName("ROOT");
         root.CreateRuntime();
         root.LoadMod("FileSystem");
         root.LoadMod("AssetsGeometry");

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
         Thing root;
         root.SetName("ROOT");
         root.CreateRuntime();
         root.LoadMod("FileSystem");
         root.LoadMod("AssetsGeometry");

         WHEN("The mesh is created via abstractions") {
            auto producedMesh = root.CreateUnit<A::Mesh>(Math::Box2 {});

            // Update once                                              
            root.Update(Time::zero());
            root.DumpHierarchy();

            REQUIRE(producedMesh.GetCount() == 1);
            REQUIRE(producedMesh.CastsTo<A::Mesh>(1));
            REQUIRE(producedMesh.IsSparse());
            REQUIRE(root.GetUnits().GetCount() == 1);
         }
         
      #if LANGULUS_FEATURE(MANAGED_REFLECTION)
         WHEN("The mesh is created via tokens") {
            auto producedMesh = root.CreateUnitToken("Mesh", Math::Box2 {});

            // Update once                                              
            root.Update(Time::zero());
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
         Thing root;
         root.SetName("ROOT");
         root.CreateRuntime();
         root.LoadMod("FileSystem");
         root.LoadMod("AssetsGeometry");
         root.LoadMod("AssetsImages");

         WHEN("The mesh is created via abstractions") {
            auto producedMesh = root.CreateUnit<A::Mesh>("maxwell/maxwell.obj");

            // Update once                                              
            root.Update(Time::zero());
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
            root.Update(Time::zero());
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
