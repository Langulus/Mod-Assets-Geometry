#include "MContent.hpp"
#include "Geometry/GeometryTools.hpp"

#define PC_VERBOSE_GEOMETRY(a) //pcLogSelfVerbose << a

/// Geometry generator construction															
///	@param classid - type override for the geometry									
///	@param producer - the geometry producer											
CGeneratorGeometry::CGeneratorGeometry(DMeta classid, MContent* producer)
   : AGeometry {classid}
   , TProducedFrom {producer} {
   mConstruct = { ClassMeta() };
   ClassValidate();
}

/// Geometry destructor																			
CGeneratorGeometry::~CGeneratorGeometry() {
   Uninitialize();
}

/// Add generators, traits and data to content											
///	@param verb - the creation verb														
void CGeneratorGeometry::Create(Verb& verb) {
   verb.GetArgument().ForEachDeep([&](const Block& group) {
   EitherDoThis
      group.ForEach([&](const Trait& trait) {
         switch (trait.GetTraitSwitch()) {
         case Traits::Topology::Switch:
            // Set topology															
            SetTopology(trait.As<DataID>().GetMeta());
            verb.Done();
            break;
         case Traits::Tesselation::Switch: {
            // Set tesselation														
            auto tesselation = trait.AsCast<real>();
            SetTesselation(static_cast<pcptr>(tesselation < 0 ? 0 : tesselation));
            verb.Done();
            break;
         }
         case Traits::TextureMapper::Switch:
            // Set texture mapper													
            SetTextureMapper(trait.As<Mapper>());
            verb.Done();
            break;
         case Traits::Bilateral::Switch:
            // Set texture mapper													
            SetBilaterality(trait.As<bool>());
            verb.Done();
            break;
         default:
            // Set any other kind of trait										
            mConstruct.Set(trait);
            verb.Done();
         }
      })
   OrThis
      group.ForEach([&](const DataID& type) {
         const auto meta = type.GetMeta();
         if (meta->Is<ATriangle>() || meta->Is<ALine>() || meta->Is<APoint>()) {
            // Set topology															
            SetTopology(meta);
            verb.Done();
         }
         else if (meta->InterpretsAs<APoint>()) {
            // Position generator													
            AddDataDeclaration<Traits::Position>(meta);
            verb.Done();
         }
         else if (meta->InterpretsAs<Normal>()) {
            // Normal generator														
            AddDataDeclaration<Traits::Aim>(meta);
            verb.Done();
         }
         else if (meta->InterpretsAs<ASampler>()) {
            // Texture coordinates generator										
            AddDataDeclaration<Traits::Sampler>(meta);
            verb.Done();
         }
         else if (meta->InterpretsAs<AMatrix>()) {
            // Instance generator													
            AddDataDeclaration<Traits::ModelTransform>(meta);
            verb.Done();
         }
         else if (meta->InterpretsAs<AQuaternion>()) {
            // Instance generator													
            AddDataDeclaration<Traits::ModelTransform>(meta);
            verb.Done();
         }
         else if (meta->InterpretsAs<AColor>()) {
            // Color generator														
            AddDataDeclaration<Traits::Color>(meta);
            verb.Done();
         }
         else if (meta->InterpretsAs<AInteger>(1)) {
            // Index generator														
            AddDataDeclaration<Traits::Index>(meta);
            verb.Done();
         }
         else if (meta->InterpretsAs<ACode>()) {
            // Code generator															
            AddDataDeclaration<Traits::Code>(meta);
            verb.Done();
         }
      })
   OrThis
      group.ForEach([&](const Mapper& type) {
         // Set texture mapping														
         SetTextureMapper(type);
         verb.Done();
      })
   AndReturnIfDone;

      // Fallback - just push group to descriptor								
      mConstruct << Any{ group };
      verb.Done();
   });
}

/// Add a declaration to the content descriptor											
///	@param construct - the construct to push to the descriptor					
void CGeneratorGeometry::AddDeclaration(const Construct& construct) {
   if (!construct && construct.GetCharge().IsDefault())
      mConstruct << construct.GetMeta()->GetID();
   else
      mConstruct << construct;
   mExplicitlyDefined = true;
}

/// Generate the raw geometry from the content descriptor. This is never		
/// called by the content manager. Call it yourself when the data is required	
void CGeneratorGeometry::Generate() {
   if (mGenerated)
      return;

   // No Create() was called inside generator, so we call default			
   if (!mExplicitlyDefined) {
      mExplicitlyDefined = DefaultCreate();
      if (!mExplicitlyDefined)
         throw Except::Content(pcLogSelfError 
            << "No instructions to generate " << mConstruct);
   }

   // Use all available generators													
   PC_VERBOSE_GEOMETRY("Generating " << mConstruct);

   if (GetData<Traits::Index>() && mIndexGenerator) {
      // Generate indices																
      mIndexGenerator(this);
      PC_VERBOSE_GEOMETRY("Indices generated");
   }

   bool hasVertices = false;
   if (GetData<Traits::Position>() && mVertexGenerator) {
      // Generate positions															
      mVertexGenerator(this);
      PC_VERBOSE_GEOMETRY("Vertex positions generated");
      hasVertices = true;
   }

   bool hasTexture = false;
   if (GetData<Traits::Sampler>() && mTextureCoordGenerator) {
      // Generate texture coordinates												
      mTextureCoordGenerator(this);
      PC_VERBOSE_GEOMETRY("Texture coordinates generated");
      hasTexture = true;
   }

   if (GetData<Traits::Aim>(0) && mNormalGenerator) {
      // Generate normals																
      mNormalGenerator(this);
      PC_VERBOSE_GEOMETRY("Normals generated");

      if (GetData<Traits::Aim>(1) && mTangentGenerator && hasTexture && hasVertices) {
         // Generate tangents															
         mTangentGenerator(this);
         PC_VERBOSE_GEOMETRY("Tangents generated");

         if (GetData<Traits::Aim>(2) && mBitangentGenerator) {
            // Generate bitangents													
            mBitangentGenerator(this);
            PC_VERBOSE_GEOMETRY("Bitangents generated");
         }
      }
   }

   if (	GetData<Traits::ModelTransform>() && 
         GetData<Traits::ModelTransform>()->InterpretsAs<AMatrix>() && 
         mInstanceGenerator
   ) {
      // Generate instances															
      mInstanceGenerator(this);
      PC_VERBOSE_GEOMETRY("Instance data generated");
   }

   if (	GetData<Traits::ModelTransform>() && 
         GetData<Traits::ModelTransform>()->InterpretsAs<AQuaternion>() && 
         mRotationGenerator
   ) {
      // Generate rotations															
      mRotationGenerator(this);
      PC_VERBOSE_GEOMETRY("Rotation data generated");
   }

   if (GetData<Traits::Color>() && mColorGenerator) {
      // Generate colors																
      mColorGenerator(this);
      PC_VERBOSE_GEOMETRY("Colors generated");
   }

   if (GetData<Traits::Code>() && mCodeGenerator) {
      // Generate code																	
      mCodeGenerator(this);
      PC_VERBOSE_GEOMETRY("Code generated");
   }

   // Invoke the tesselator, if required											
   if (GetTesselation() > 0 && mTesselator)
      mTesselator(this);

   // Done																					
   if (!GetDataList()) {
      mGenerated = false;
      throw Except::Content(pcLogSelfError << "No content was generated");
   }

   PC_VERBOSE_GEOMETRY(ccGreen << "Generated");
   mGenerated = true;
}

/// Get LOD																							
///	@param lod - the lod state with data required for LOD picking				
///	@return the LODed geometry																
const AGeometry* CGeneratorGeometry::GetLOD(const LodState& lod) const {
   if (mLodGenerator)
      return mLodGenerator(this, lod);
   return this;
}

/// Uninitialize the content																	
void CGeneratorGeometry::Uninitialize() {
   AContent::Uninitialize();
   mVertexGenerator = nullptr;
   mNormalGenerator = nullptr;
   mTangentGenerator = nullptr;
   mBitangentGenerator = nullptr;
   mTextureCoordGenerator = nullptr;
   mInstanceGenerator = nullptr;
   mRotationGenerator = nullptr;
   mColorGenerator = nullptr;
   mIndexGenerator = nullptr;
   mTesselator = nullptr;
   mLodGenerator = nullptr;
   mCodeGenerator = nullptr;
}

/// Subdivide a topology																		
///	@param dataInput - data to subdivide												
///	@param indices - indices for the primitive										
///	@param dataOutput - [out] subdivided data											
///	@param idxOutput - [out] subdivided data indices								
template<class TOPOLOGY, class PRIMITIVE>
inline void InnerSubdivide(const Trait& dataInput, const vec3u& indices, Trait& dataOutput, TAny<pcu32>& idxOutput) {
   using POINT = typename PRIMITIVE::PointType;

   // The rank of the primitive decides how many primitives come			
   // out after subdivision. A line subdivides into two lines, while		
   // a triangle subdivides into four triangles.								
   if constexpr (Same<TOPOLOGY, ALine>) {
      // Subdivide a line																
      auto primitive = PRIMITIVE(
         dataInput.Get<POINT>(indices[0]),
         dataInput.Get<POINT>(indices[1])
      );

      auto dividedPrimitive = primitive.Subdivide();
      if (!idxOutput.IsAllocated()) {
         // No index buffer available, so we have to push all lines		
         dataOutput << dividedPrimitive[0][0] << dividedPrimitive[0][1];
         dataOutput << dividedPrimitive[1][0] << dividedPrimitive[1][1];
      }
      else {
         // Index buffer available, so push only the new vertex			
         dataOutput << dividedPrimitive[0][1];

         // Push first half of the line											
         idxOutput << indices[0] << pcu32(dataOutput.GetCount() - 1u);
         // Push second half of the line											
         idxOutput << pcu32(dataOutput.GetCount() - 1u) << indices[1];
      }
   }
   else if constexpr (Same<TOPOLOGY, ATriangle>) {
      // Subdivide a triangle															
      auto primitive = PRIMITIVE(
         dataInput.Get<POINT>(indices[0]),
         dataInput.Get<POINT>(indices[1]),
         dataInput.Get<POINT>(indices[2])
      );

      auto dividedPrimitive = primitive.Subdivide();
      if (!idxOutput.IsAllocated()) {
         // No index buffer available, so we have to push all triangles	
         dataOutput << dividedPrimitive[0][0] << dividedPrimitive[0][1] << dividedPrimitive[0][2];
         dataOutput << dividedPrimitive[1][0] << dividedPrimitive[1][1] << dividedPrimitive[1][2];
         dataOutput << dividedPrimitive[2][0] << dividedPrimitive[2][1] << dividedPrimitive[2][2];
         dataOutput << dividedPrimitive[3][0] << dividedPrimitive[3][1] << dividedPrimitive[3][2];
      }
      else {
         // Index buffer available, so push only the new vertices			
         dataOutput << dividedPrimitive[0][1];
         dataOutput << dividedPrimitive[0][2];
         dataOutput << dividedPrimitive[1][2];

         // Push first triangle (bottom left triangle)						
         idxOutput << indices[0] << pcu32(dataOutput.GetCount() - 3u) << pcu32(dataOutput.GetCount() - 2u);
         // Push second triangle (top triangle)									
         idxOutput << pcu32(dataOutput.GetCount() - 3u) << indices[1] << pcu32(dataOutput.GetCount() - 1u);
         // Push third triangle (bottom right triangle)						
         idxOutput << pcu32(dataOutput.GetCount() - 2u) << pcu32(dataOutput.GetCount() - 1u) << indices[2];
         // Push fourth triangle (in the middle)								
         idxOutput << pcu32(dataOutput.GetCount() - 3u) << pcu32(dataOutput.GetCount() - 1u) << pcu32(dataOutput.GetCount() - 2u);
      }
   }
}

/// Helper																							
template<class T>
inline bool InnerExpand(const Trait& indices, const pcbyte* source, pcbyte* dest, const pcptr stride) {
   if (!indices.Is<T>())
      return false;

   auto idx = indices.As<T*>();
   for (pcptr i = 0; i < indices.GetCount(); ++i) {
      dest += pcCopyMemory(source + (*idx) * stride, dest, stride);
      ++idx;
   }

   return true;
}

/// Expand geometry based on provided indices											
/// After this call, the geometry will no longer be indexed							
/// Does nothing if there is no Traits::Index data inside content					
void CGeneratorGeometry::Expand() {
   // Get indices																			
   const auto indices = GetData<Traits::Index>();
   if (!indices || indices->IsEmpty())
      return;

   // Expand each part of the content												
   Any newData;
   for (const auto& group : GetDataList()) {
      if (group.TraitIs<Traits::Index>())
         continue;

      auto copy = Any::From(group.GetMeta());
      copy.Allocate(indices->GetCount(), false, true);
      const auto stride = copy.GetStride();
      auto dest = copy.GetBytes();
      auto source = group.GetBytes();
      if (  InnerExpand<pcu8>(*indices, source, dest, stride)
         || InnerExpand<pcu16>(*indices, source, dest, stride)
         || InnerExpand<pcu32>(*indices, source, dest, stride)
         || InnerExpand<pcu64>(*indices, source, dest, stride)) {
         newData << Trait(group.GetTraitMeta(), pcMove(copy));
      }
      else {
         throw Except::Content(pcLogSelfError
            << "Bad index format: " << indices->GetToken());
      }
   }

   // Substitute the current content with the expanded one					
   GetDataList() = pcMove(newData);
   mView.mPCount = indices->GetCount();
   mView.mPStart = 0;
   mView.mICount = 0;
   mView.mIStart = 0;
}

/// Helper																							
///	@param group - the group to normalize												
///	@param constrain - true to keep normalized values at [-0.5; 0.5]			
///	@return true on success																	
template<class T>
bool InnerNormalize(Trait& group, bool constrain) {
   TAny<T> groupDecayed{ group.Decay<T>() };
   if (groupDecayed.IsEmpty())
      return false;

   if (constrain) {
      for (auto& element : groupDecayed)
         element = element.Normalize() * T(0.5);
   }
   else {
      for (auto& element : groupDecayed)
         element = element.Normalize();
   }

   return true;
}

/// Normalize a part of the geometry data													
///	@param trait - which part to normalize												
///	@param constrain - true to keep normalized values at [-0.5; 0.5]			
void CGeneratorGeometry::Normalize(TraitID trait, bool constrain) {
   for (auto& group : GetDataList()) {
      if (!group.GetTraitMeta()->Is(trait))
         continue;

      if (  !InnerNormalize<vec3>(group, constrain)
         && !InnerNormalize<vec2>(group, constrain)
         && !InnerNormalize<vec4>(group, constrain)) {
         throw Except::Content(pcLogFuncError
            << "Trying to normalize content that is not normalizable: "
            << group.GetToken());
      }
   }
}

/// Templated tesselator for specific topology											
///	@param t - tesselation level															
///	@param geometry - the geometry to tesselate										
template<class TOPOLOGY>
void InnerTesselate(pcptr t, CGeneratorGeometry& geometry) {
   while (t > 0) {
      // Get indices, that may or may not exist									
      const auto indexBufferInput = geometry.GetData<Traits::Index>();
      if (!indexBufferInput || indexBufferInput->IsEmpty())
         throw Except::Content();

      TAny<pcu32> indexBufferOutput;

      // Get primitive count															
      pcptr count = 0;
      if constexpr (Same<TOPOLOGY, ALine>) {
         count = geometry.GetLineCount();
         if (!indexBufferInput->IsEmpty())
            indexBufferOutput.Allocate(count * 2 * 2);
      }
      else if constexpr (Same<TOPOLOGY, ATriangle>) {
         count = geometry.GetTriangleCount();
         if (!indexBufferInput->IsEmpty())
            indexBufferOutput.Allocate(count * 3 * 4);
      }
      else LANGULUS_ASSERT("Unsupported topology");

      // Subdivide each primitive...												
      auto newContent = Any::From<Trait>();
      for (const auto& group : geometry.GetDataList()) {
         if (group.TraitIs<Traits::Index>())
            continue;

         Trait newGroup;
         if (!indexBufferInput->IsEmpty())
            newGroup = group.Clone();

         for (pcptr k = 0; k < count; ++k) {
            vec3u indices;
            if constexpr (Same<TOPOLOGY, ALine>)
               indices = geometry.GetLineIndices(k);
            else if constexpr (Same<TOPOLOGY, ATriangle>)
               indices = geometry.GetTriangleIndices(k);
            else LANGULUS_ASSERT("Unsupported topology");

            if (group.InterpretsAs<rgba>()) {
               // Color																	
               if constexpr (Same<TOPOLOGY, ALine>)
                  InnerSubdivide<TOPOLOGY, TLine<rgba>>(group, indices, newGroup, indexBufferOutput);
               else if constexpr (Same<TOPOLOGY, ATriangle>)
                  InnerSubdivide<TOPOLOGY, TTriangle<rgba>>(group, indices, newGroup, indexBufferOutput);
               else LANGULUS_ASSERT("Unsupported topology");
            }
            else if (group.InterpretsAs<ANormal>()) {
               // Normalized															
               if constexpr (Same<TOPOLOGY, ALine>)
                  InnerSubdivide<TOPOLOGY, TLine<vec3>>(group, indices, newGroup, indexBufferOutput);
               else if constexpr (Same<TOPOLOGY, ATriangle>)
                  InnerSubdivide<TOPOLOGY, TTriangle<vec3>>(group, indices, newGroup, indexBufferOutput);
               else LANGULUS_ASSERT("Unsupported topology");

               // Normalize															
               InnerNormalize<vec3>(newGroup, false);
            }
            else if (group.InterpretsAs<vec3>()) {
               // 3D vector fallback (most common)								
               if constexpr (Same<TOPOLOGY, ALine>)
                  InnerSubdivide<TOPOLOGY, TLine<vec3>>(group, indices, newGroup, indexBufferOutput);
               else if constexpr (Same<TOPOLOGY, ATriangle>)
                  InnerSubdivide<TOPOLOGY, TTriangle<vec3>>(group, indices, newGroup, indexBufferOutput);
               else LANGULUS_ASSERT("Unsupported topology");
            }
            else if (group.InterpretsAs<vec2>()) {
               // 2D vector fallback												
               if constexpr (Same<TOPOLOGY, ALine>)
                  InnerSubdivide<TOPOLOGY, TLine<vec2>>(group, indices, newGroup, indexBufferOutput);
               else if constexpr (Same<TOPOLOGY, ATriangle>)
                  InnerSubdivide<TOPOLOGY, TTriangle<vec2>>(group, indices, newGroup, indexBufferOutput);
               else LANGULUS_ASSERT("Unsupported topology");
            }
            else if (group.InterpretsAs<vec4>()) {
               // 4D vector fallback												
               if constexpr (Same<TOPOLOGY, ALine>)
                  InnerSubdivide<TOPOLOGY, TLine<vec4>>(group, indices, newGroup, indexBufferOutput);
               else if constexpr (Same<TOPOLOGY, ATriangle>)
                  InnerSubdivide<TOPOLOGY, TTriangle<vec4>>(group, indices, newGroup, indexBufferOutput);
               else LANGULUS_ASSERT("Unsupported topology");
            }
            else {
               throw Except::Content(pcLogFuncError
                  << "Trying to subdivide content that is not interpretable as vectors: "
                  << group.GetToken());
            }
         }

         // Push the tesselated content											
         newContent << Trait(group.GetTraitMeta(), pcMove(newGroup));
      }

      // Push the tesselated indices												
      const auto indexCount = indexBufferOutput.GetCount();
      newContent << Trait::FromMemory<Traits::Index>(pcMove(indexBufferOutput));

      // Substitute the content with the new one								
      geometry.GetDataList() = pcMove(newContent);
      geometry.GetView().mPCount = count;
      geometry.GetView().mPStart = 0;
      geometry.GetView().mICount = indexCount;
      geometry.GetView().mIStart = 0;

      // Perform the next level of tesselation									
      --t;
   }
}

/// Standard tesselation																		
///	@param t - the level of subdivision													
void CGeneratorGeometry::Tesselate(pcptr t) {
   // We also must make sure that the correct tesselation is in the		
   // descriptor. This will force a rehash										
   mConstruct.Set(Trait::From<Traits::Tesselation>(t));
   if (t == 0 || IsTopology<APoint>())
      return;

   if (CheckTopology<ALine>()) {
      // Tesselating lines																
      InnerTesselate<ALine>(t, *this);
   }
   else if (CheckTopology<ATriangle>()) {
      // Tesselating triangles														
      InnerTesselate<ATriangle>(t, *this);
   }
   else {
      throw Except::Content(pcLogSelfError
         << "Can't tesselate unsupported topology: "
         << GetTopology()->GetToken());
   }
}

/// Get number of line primitives inside geometry. This doesn't require			
/// geometry to be actually generated. You can count lines even if topology	
/// is made of triangles. In that case this function will count all the			
/// triangle edges																				
///	@return the number of lines															
pcptr CGeneratorGeometry::GetLineCount() const {
   if (IsTopology<APoint>())
      return 0;
   else if (IsTopology<ALine>()) {
      if (mView.mICount > 1)
         return mView.mICount / 2;
      else {
         auto decayed = mView.Decay();
         if (decayed.mPCount > 1)
            return decayed.mPCount / 2;
      }
      return 0;
   }
   else if (IsTopology<ALineStrip>()) {
      if (mView.mICount > 1)
         return mView.mICount - 1;
      else if (mView.mPCount > 1)
         return mView.mPCount - 1;
      return 0;
   }
   else if (IsTopology<ALineLoop>()) {
      if (mView.mICount > 1)
         return mView.mICount;
      else if (mView.mPCount > 1)
         return mView.mPCount;
      return 0;
   }
   else if (IsTopology<ATriangle>()) {
      if (mView.mICount > 2)
         return mView.mICount / 3;
      else {
         auto decayed = mView.Decay();
         if (decayed.mPCount > 2)
            return decayed.mPCount;
      }
      return 0;
   }
   else if (IsTopology<ATriangleStrip>() || IsTopology<ATriangleFan>()) {
      if (mView.mICount > 2)
         return 1 + (mView.mICount - 2) * 2;
      else if (mView.mPCount > 2)
         return 1 + (mView.mPCount - 2) * 2;
      return 0;
   }

   throw Except::Content(pcLogSelfError
      << "Trying to count lines for unknown topology: " 
      << GetTopology()->GetToken());
}

/// Helper that indirects in case there is an index buffer							
///	@param indices - index buffer															
///	@param where - line indices															
///	@return the (eventually indirected) line indices								
inline vec2u InnerGetIndices(const Trait& indices, const vec2u& where) {
   if (indices.IsEmpty())
      return where;

   switch (indices.GetDataSwitch()) {
      case DataID::Switch<pcu8>() : {
         auto idx = indices.As<pcu8*>();
         return { idx[where[0]], idx[where[1]] };
      }
      case DataID::Switch<pcu16>() : {
         auto idx = indices.As<pcu16*>();
         return { idx[where[0]], idx[where[1]] };
      }
      case DataID::Switch<pcu32>() : {
         auto idx = indices.As<pcu32*>();
         return { idx[where[0]], idx[where[1]] };
      }
      case DataID::Switch<pcu64>() : {
         auto idx = indices.As<pcu64*>();
         return { 
            static_cast<pcu32>(idx[where[0]]), 
            static_cast<pcu32>(idx[where[1]]) 
         };
      }
   default:
      throw Except::Content(pcLogFuncError
         << "Trying to get index from incompatible index buffer: "
         << indices.GetToken());
   }
}

/// Get the point indices of a given line													
///	@param index - line index																
///	@return the point indices as a 32bit unsigned 2D vector						
vec2u CGeneratorGeometry::GetLineIndices(pcptr index) const {
   const auto indices = GetData<Traits::Index>();
   if (!indices || indices->IsEmpty())
      throw Except::Content();

   if (IsTopology<ALine>()) {
      return InnerGetIndices(*indices, 
         vec2u(index * 2, index * 2 + 1));
   }
   else if (IsTopology<ALineStrip>()) {
      return InnerGetIndices(*indices, 
         vec2u(index > 0 ? index - 1 : 0, index > 0 ? index : 1));
   }
   else if (IsTopology<ALineLoop>()) {
      if (index == GetLineCount() - 1)
         return InnerGetIndices(*indices, 
            vec2u(index, 0));
      else
         return InnerGetIndices(*indices, 
            vec2u(index > 0 ? index - 1 : 0, index > 0 ? index : 1));
   }
   else if (IsTopology<ATriangle>()) {
      switch (index % 3) {
      case 0: case 1:
         return InnerGetIndices(*indices, 
            vec2u(index, index + 1));
      case 2:
         return InnerGetIndices(*indices, 
            vec2u(index, index - 2));
      }
   }
   else if (IsTopology<ATriangleStrip>()) {
      switch (index % 3) {
      case 0:
         return InnerGetIndices(*indices, 
            vec2u(index == 0 ? 0 : index - 2, index == 0 ? 1 : index - 1));
      case 1:
         return InnerGetIndices(*indices, 
            vec2u(index - 1, index));
      case 2:
         return InnerGetIndices(*indices, 
            vec2u(index, index - 2));
      }
   }
   else if (IsTopology<ATriangleFan>()) {
      switch (index % 3) {
      case 0:
         return InnerGetIndices(*indices, 
            vec2u(0, index == 0 ? 1 : index - 1));
      case 1:
         return InnerGetIndices(*indices, 
            vec2u(index - 1, index));
      case 2:
         return InnerGetIndices(*indices, 
            vec2u(index, 0));
      }
   }

   throw Except::Content(pcLogSelfError
      << "Trying to count lines for unknown topology: " 
      << GetTopology()->GetToken());
}

/// Get a 3D line with the given index, for convenience								
///	@param index - line index																
///	@return the line primitive																
Line3 CGeneratorGeometry::GetLine(pcptr lineIndex) const {
   auto positions = GetLineTrait<Traits::Position>(lineIndex);
   switch (positions.GetDataSwitch()) {
      case DataID::Switch<Point3>():
         return Line3(positions.As<Point3*>());
      case DataID::Switch<Point2>():
         return static_cast<Line3>(Line2(positions.As<Point2*>()));
      default:
         TODO();
   }
}

/// Get a line property with the given index												
///	@param index - line index																
///	@param trait - the line property														
///	@return the property																		
Any CGeneratorGeometry::GetLineTrait(pcptr lineIndex, TMeta trait) const {
   const auto indices = GetLineIndices(lineIndex);
   const auto soughtt = GetData(trait);
   if (!soughtt || soughtt->IsEmpty())
      return {};

   Block soughtDecayed;
   if (soughtt->InterpretsAs<Triangle3>())
      soughtDecayed = Block::From(soughtt->As<Point3*>(), soughtt->GetCount() * 3);
   else if (soughtt->InterpretsAs<Triangle2>())
      soughtDecayed = Block::From(soughtt->As<Point2*>(), soughtt->GetCount() * 3);
   else
      soughtDecayed = *static_cast<const Block*>(soughtt);

   Any result;
   result.InsertBlock(soughtDecayed.GetElement(indices[0]));
   result.InsertBlock(soughtDecayed.GetElement(indices[1]));
   return result;
}

/// Get number of triangle primitives inside geometry									
/// This doesn't require geometry to be actually generated							
/// You can't count triangles if topology is not based on triangles				
///	@return the number of triangles														
pcptr CGeneratorGeometry::GetTriangleCount() const {
   if (CheckTopology<APoint>() || CheckTopology<ALine>())
      return 0;

   if (IsTopology<ATriangle>()) {
      if (mView.mICount > 2)
         return mView.mICount / 3;

      auto decayed = mView.Decay();
      if (decayed.mPCount % 3)
         throw Except::Content(pcLogFuncError << "Bad topology");
      return decayed.mPCount / 3;
   }
   else if (IsTopology<ATriangleStrip>() || IsTopology<ATriangleFan>()) {
      if (mView.mICount > 2)
         return mView.mICount - 2;

      if (mView.mPCount < 3)
         throw Except::Content(pcLogFuncError << "Bad topology");
      return mView.mPCount - 2;
   }

   throw Except::Content(pcLogSelfError
      << "Trying to count triangles for unknown topology: " 
      << GetTopology()->GetToken());
}

/// Helper that indirects in case there is an index buffer							
///	@param indices - index buffer															
///	@param where - triangle indices														
///	@return the (eventually indirected) triangle indices							
inline vec3u InnerGetIndices(const Trait* indices, const vec3u& where) {
   if (!indices || indices->IsEmpty())
      return where;
   
   switch (indices->GetDataSwitch()) {
      case DataID::Switch<pcu8>() : {
         auto idx = indices->As<pcu8*>();
         return { idx[where[0]], idx[where[1]], idx[where[2]] };
      }
      case DataID::Switch<pcu16>() : {
         auto idx = indices->As<pcu16*>();
         return { idx[where[0]], idx[where[1]], idx[where[2]] };
      }
      case DataID::Switch<pcu32>() : {
         auto idx = indices->As<pcu32*>();
         return { idx[where[0]], idx[where[1]], idx[where[2]] };
      }
      case DataID::Switch<pcu64>() : {
         auto idx = indices->As<pcu64*>();
         return {
            static_cast<pcu32>(idx[where[0]]),
            static_cast<pcu32>(idx[where[1]]),
            static_cast<pcu32>(idx[where[2]])
         };
      }
      default:
         throw Except::Content(pcLogFuncError
            << "Trying to get index from incompatible index buffer: "
            << indices->GetToken());
   }
}

/// Get the indices of a given triangle													
///	@param index - triangle index															
///	@return the indices as a 32bit unsigned 3D vector								
vec3u CGeneratorGeometry::GetTriangleIndices(pcptr index) const {
   const auto indices = GetData<Traits::Index>();

   if (IsTopology<ATriangle>()) {
      return InnerGetIndices(indices, vec3u(
         index * 3,
         index * 3 + 1,
         index * 3 + 2)
      );
   }
   else if (IsTopology<ATriangleStrip>()) {
      return InnerGetIndices(indices, vec3u(
         index == 0 ? 0 : index - 1,
         index == 0 ? 1 : index,
         index == 0 ? 2 : index + 1 )
      );
   }
   else if (IsTopology<ATriangleFan>()) {
      return InnerGetIndices(indices, vec3u(
         0,
         index == 0 ? 1 : index,
         index == 0 ? 2 : index + 1)
      );
   }

   throw Except::Content(pcLogSelfError
      << "Trying to count triangles for unknown topology: " 
      << GetTopology()->GetToken());
}

/// Get a triangle with the given index													
///	@param triangleIndex - triangle index												
///	@return the triangle primitive														
Triangle3 CGeneratorGeometry::GetTriangle(pcptr triangleIndex) const {
   auto positions = GetTriangleTrait<Traits::Position>(triangleIndex);
   switch (positions.GetDataSwitch()) {
      case DataID::Switch<Point3>():
         return Triangle3(positions.As<Point3*>());
      case DataID::Switch<Point2>():
         return static_cast<Triangle3>(Triangle2(positions.As<Point2*>()));
      default:
         TODO();
   }
}

/// Get a triangle property with the given index										
///	@param triangleIndex - triangle index												
///	@param trait - the triangle property												
///	@return the property																		
Any CGeneratorGeometry::GetTriangleTrait(pcptr triangleIndex, TMeta trait) const {
   const auto indices = GetTriangleIndices(triangleIndex);
   const auto soughtt = GetData(trait);
   if (!soughtt || soughtt->IsEmpty())
      return {};

   Block soughtDecayed;
   if (soughtt->InterpretsAs<Triangle3>())
      soughtDecayed = soughtt->Decay<Point3>();
   else if (soughtt->InterpretsAs<Triangle2>())
      soughtDecayed = soughtt->Decay<Point2>();
   else
      soughtDecayed = *static_cast<const Block*>(soughtt);

   Any result;
   result.InsertBlock(soughtDecayed.GetElement(indices[0]));
   result.InsertBlock(soughtDecayed.GetElement(indices[1]));
   result.InsertBlock(soughtDecayed.GetElement(indices[2]));
   return result;
}

/// Default tangent generator to assign as mTangentGenerator functor			
///	@param instance - the geometry to generate tangents for					
void CGeneratorGeometry::DefaultTangentGenerator(CGeneratorGeometry* instance) {
   if (!instance->CheckTopology<ATriangle>())
      return;

   // Tangents depend on various things										
   const auto triangle = instance->GetData<Traits::Position>();
   if (!triangle || triangle->IsEmpty()) {
      pcLogFuncError << "Can't generate tangents if no positions are defined for geometry";
      return;
   }

   const auto sampler = instance->GetData<Traits::Sampler>();
   if (!sampler || sampler->IsEmpty()) {
      pcLogFuncError << "Can't generate tangents if no texture coordinates are defined for geometry";
      return;

   }
   const auto normals = instance->GetData<Traits::Aim>(0);
   if (!normals || normals->IsEmpty()) {
      pcLogFuncError << "Can't generate tangents if no normals are defined for geometry";
      return;
   }

   // Interface the data															
   if (!triangle->Is<Triangle3>()) {
      pcLogFuncError << "TODO not triangles";
      return;
   }

   auto t = triangle->As<Triangle3*>();
   auto s = sampler->As<Sampler2*>();
   auto normal = normals->As<Normal*>();
   const auto IndexCount = normals->GetCount();
   const auto TriangleCount = triangle->GetCount();

   TAny<vec3> temporary;
   temporary.Allocate(IndexCount * 2);
   auto tempt1 = temporary.GetRaw();
   auto tempt2 = temporary.GetRaw() + IndexCount;

   // Generate basis vectors for each triangle								
   for (pcptr i = 0; i < TriangleCount; ++i) {
      auto& ti = t[i];
      const real x1 = ti[1][0] - ti[0][0];
      const real x2 = ti[2][0] - ti[0][0];
      const real y1 = ti[1][1] - ti[0][1];
      const real y2 = ti[2][1] - ti[0][1];
      const real z1 = ti[1][2] - ti[0][2];
      const real z2 = ti[2][2] - ti[0][2];

      auto si = s + i * 3;
      const real s1 = si[1][0] - si[0][0];
      const real s2 = si[2][0] - si[0][0];
      const real t1 = si[1][1] - si[0][1];
      const real t2 = si[2][1] - si[0][1];

      const real r = real(1) / (s1 * t2 - s2 * t1);
      const vec3 sdir(
         (t2 * x1 - t1 * x2) * r,
         (t2 * y1 - t1 * y2) * r,
         (t2 * z1 - t1 * z2) * r
      );

      const vec3 tdir(
         (s1 * x2 - s2 * x1) * r,
         (s1 * y2 - s2 * y1) * r,
         (s1 * z2 - s2 * z1) * r
      );

      tempt1[i * 3 + 0] += sdir;
      tempt1[i * 3 + 1] += sdir;
      tempt1[i * 3 + 2] += sdir;

      tempt2[i * 3 + 0] += tdir;
      tempt2[i * 3 + 1] += tdir;
      tempt2[i * 3 + 2] += tdir;
   }

   // Calculate the tangents for each vertex									
   auto content = instance->GetData<Traits::Aim>(1);
   content->Allocate(IndexCount);
   int handedness = 0;
   for (pcptr i = 0; i < IndexCount; ++i) {
      const auto& nor = normal[i];
      const auto& tan = tempt1[i];

      // Gram-Schmidt orthogonalization										
      *content << Normal(tan - nor * nor.Dot(tan));

      // Check handedness. In principle, this value (-1 or 1) can be 
      // written to tangent.w to account for it, but at the cost of	
      // more memory and computation. Better tactic is to detect		
      // early and make sure that if two triangles have different		
      // handiness, then their vertices should get duplicated			
      handedness += (nor.Cross(tan).Dot(tempt2[i]) < 0) ? -1 : 1;
   }

   if (handedness != int(IndexCount) && handedness != -int(IndexCount)) {
      pcLogFuncWarning
         << "Tangents with different handedness generated"
         << " - you might experience seams";
   }
}

/// Default bitangent generator to assign as mBitangentGenerator functor	
///	@param instance - the geometry to generate bitangents for				
void CGeneratorGeometry::DefaultBitangentGenerator(CGeneratorGeometry* instance) {
   if (!instance->CheckTopology<ATriangle>())
      return;

   // Tangents depend on various things										
   const auto normals = instance->GetData<Traits::Aim>(0);
   if (!normals || normals->IsEmpty()) {
      pcLogFuncError << "Can't generate bitangents if no normals are defined for geometry";
      return;
   }

   const auto tangents = instance->GetData<Traits::Aim>(1);
   if (!tangents || tangents->IsEmpty()) {
      pcLogFuncError << "Can't generate bitangents if no tangents are defined for geometry";
      return;
   }

   // Interface the data															
   auto n = normals->As<Normal*>();
   auto t = tangents->As<Normal*>();
   const auto IndexCount = normals->GetCount();

   // Generate bitangents															
   auto content = instance->GetData<Traits::Aim>(2);
   content->Allocate(IndexCount);
   for (pcptr i = 0; i < IndexCount; ++i)
      *content << Normal(n[i].Cross(t[i]));
}
