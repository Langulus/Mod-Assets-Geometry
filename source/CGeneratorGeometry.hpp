#pragma once
#include <PCFW.hpp>

using namespace PCFW;

class MContent;

///																									
///	GEOMETRY GENERATOR																		
///																									
class CGeneratorGeometry : public AGeometry, public TProducedFrom<MContent> {
	REFLECT(CGeneratorGeometry);
public:
	CGeneratorGeometry(DMeta, MContent*);
	CGeneratorGeometry(CGeneratorGeometry&&) noexcept = default;
	CGeneratorGeometry& operator = (CGeneratorGeometry&&) noexcept = default;
	~CGeneratorGeometry();

	PC_VERB(Create);

public:
	virtual bool DefaultCreate() { return false; }

	// From AContent																		
	void Generate() override;
	void Uninitialize() override;

	// From AGeometry																		
	NOD() const AGeometry* GetLOD(const LodState&) const override;

	// Common line interface from AGeometry										
	NOD() pcptr GetLineCount() const override;
	NOD() vec2u GetLineIndices(pcptr) const override;
	NOD() Line3 GetLine(pcptr) const override;
	NOD() Any GetLineTrait(pcptr, TMeta) const override;

	template<RTTI::ReflectedTrait T>
	NOD() Any GetLineTrait(pcptr i) const {
		return GetLineTrait(i, T::Reflect());
	}

	// Common triangle interface from AGeometry									
	NOD() pcptr GetTriangleCount() const override;
	NOD() vec3u GetTriangleIndices(pcptr) const override;
	NOD() Triangle3 GetTriangle(pcptr) const override;
	NOD() Any GetTriangleTrait(pcptr, TMeta) const override;

	template<RTTI::ReflectedTrait T>
	NOD() Any GetTriangleTrait(pcptr i) const {
		return GetTriangleTrait(i, T::Reflect());
	}

	// Some tools																			
	void Tesselate(pcptr t);
	void Expand();
	void Normalize(TraitID, bool constrain = false);

	/// Default tesselator to assign as mTesselator functor							
	///	@param instance - the geometry to tesselate									
	static void DefaultTesselator(CGeneratorGeometry* instance) {
		instance->Tesselate(instance->GetTesselation());
	}

	static void DefaultTangentGenerator(CGeneratorGeometry*);
	static void DefaultBitangentGenerator(CGeneratorGeometry*);

	void AddDeclaration(const Construct&);

	/// Declare geometry data trait, and add an empty content and range			
	///	@param meta - the type of the content											
	template<RTTI::ReflectedTrait TRAIT>
	void AddDataDeclaration(DMeta meta) {
		SaveData<TRAIT>(Any::From(meta));
		AddDeclaration(meta);
	}

protected:
	/// Generator functor																		
	using Generator = TFunctor<void(CGeneratorGeometry*)>;
	/// Functor for generating level-of-detail variants of the geometry			
	using LodGenerator = TFunctor<const CGeneratorGeometry*(const CGeneratorGeometry*, const LodState&)>;
	/// Functor for signed distance field calculation									
	using SdfFunction = TFunctor<real(const CGeneratorGeometry*, const vec3&)>;

	Generator mVertexGenerator;
	Generator mNormalGenerator;
	Generator mTangentGenerator;
	Generator mBitangentGenerator;
	Generator mTextureCoordGenerator;
	Generator mInstanceGenerator;
	Generator mRotationGenerator;
	Generator mColorGenerator;
	Generator mIndexGenerator;
	Generator mTesselator;
	Generator mCodeGenerator;

	LodGenerator mLodGenerator;
	SdfFunction mSDF;
};
