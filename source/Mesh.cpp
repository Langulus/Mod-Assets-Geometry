///                                                                           
/// Langulus::Module::Assets::Geometry                                        
/// Copyright(C) 2016 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#include "Mesh.hpp"
#include "MeshLibrary.hpp"


/// Mesh construction                                                         
///   @param producer - the producer                                          
///   @param descriptor - instructions for generator                          
Mesh::Mesh(MeshLibrary* producer, const Descriptor& descriptor)
   : A::Mesh {MetaOf<::Mesh>(), producer, descriptor} {}

/// Mesh destruction                                                          
Mesh::~Mesh() {

}

/// Produce geometry data                                                     
///   @param verb - creation verb to satisfy                                  
void Mesh::Create(Verb& verb) {

}

/// React on environmental change                                             
void Mesh::Refresh() {

}

/// Get level of detail mesh                                                  
///   @param lod - the level of detail state to generate LOD from             
///   @return the new geometry                                                
Ref<A::Mesh> Mesh::GetLOD(const LOD& lod) const {
   TODO();
}
