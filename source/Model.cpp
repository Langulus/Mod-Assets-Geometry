///                                                                           
/// Langulus::Module::Assets::Geometry                                        
/// Copyright(C) 2016 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#include "Model.hpp"
#include "GeometryLibrary.hpp"


/// Geometry construction                                                     
///   @param producer - the producer                                          
///   @param descriptor - instructions for generator                          
Model::Model(GeometryLibrary* producer, const Descriptor& descriptor)
   : A::Geometry {MetaOf<Model>(), producer, descriptor} {}

/// Geometry destruction                                                      
Model::~Model() {

}

/// Produce geometry data                                                     
///   @param verb - creation verb to satisfy                                  
void Model::Create(Verb& verb) {

}

/// React on environmental change                                             
void Model::Refresh() {

}