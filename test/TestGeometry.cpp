///                                                                           
/// Langulus::Module::Assets::Geometry                                        
/// Copyright(C) 2016 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#include "Main.hpp"
#include <Entity/Thing.hpp>
#include <Math/Primitives/Box.hpp>
#include <catch2/catch.hpp>

/// See https://github.com/catchorg/Catch2/blob/devel/docs/tostring.md        
CATCH_TRANSLATE_EXCEPTION(::Langulus::Exception const& ex) {
   const Text serialized {ex};
   return ::std::string {Token {serialized}};
}


SCENARIO("Mesh creation", "[mesh]") {
   Allocator::State memoryState;

   for (int repeat = 0; repeat != 10; ++repeat) {
      GIVEN(std::string("Init and shutdown cycle #") + std::to_string(repeat)) {
         // Create root entity                                          
         Thing root;
         root.SetName("ROOT");

         // Create runtime at the root                                  
         root.CreateRuntime();

         // Load modules                                                
         root.LoadMod("FileSystem");
         root.LoadMod("AssetsGeometry");

      #if LANGULUS_FEATURE(MANAGED_REFLECTION)
         WHEN("The mesh is created via tokens") {
            auto producedMesh = root.CreateUnitToken("Mesh", Math::Box2 {});

            // Update once                                              
            root.Update(Time::zero());

            THEN("Various traits change") {
               root.DumpHierarchy();

               REQUIRE(producedMesh.GetCount() == 1);
               REQUIRE(producedMesh.CastsTo<A::Mesh>());
               REQUIRE(producedMesh.IsSparse());
            }
         }
      #endif

         WHEN("The mesh is created via abstractions") {
            auto producedMesh = root.CreateUnit<A::Mesh>(Math::Box2 {});

            // Update once                                              
            root.Update(Time::zero());

            THEN("Various traits change") {
               root.DumpHierarchy();

               REQUIRE(producedMesh.GetCount() == 1);
               REQUIRE(producedMesh.CastsTo<A::Mesh>(1));
               REQUIRE(producedMesh.IsSparse());
            }
         }

         // Check for memory leaks after each cycle                     
         REQUIRE(memoryState.Assert());
      }
   }
}

