///                                                                           
/// Langulus::Module::Assets::Geometry                                        
/// Copyright(C) 2016 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#include "GeometryLibrary.hpp"

LANGULUS_DEFINE_MODULE(
   GeometryLibrary, 9, "AssetsGeometry",
   "Geometry reader, writer and generator", "",
   GeometryLibrary, Model, Traits::Tesselation
)

/// Module construction                                                       
///   @param runtime - the runtime that owns the module                       
///   @param descriptor - instructions for configuring the module             
GeometryLibrary::GeometryLibrary(Runtime* runtime, const Descriptor&)
   : A::AssetModule {MetaOf<GeometryLibrary>(), runtime}
   , mModels {this} {
   Logger::Verbose(Self(), "Initializing...");
   Logger::Verbose(Self(), "Initialized");
}

/// Module update routine                                                     
///   @param dt - time from last update                                       
void GeometryLibrary::Update(Time) {

}

/// Create/Destroy GUI systems                                                
///   @param verb - the creation/destruction verb                             
void GeometryLibrary::Create(Verb& verb) {
   mModels.Create(verb);
}