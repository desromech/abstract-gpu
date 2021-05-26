#include "immediate_renderer.hpp"
#include <stddef.h>
#include <math.h>
#include <memory>

#ifndef M_PI
#define M_PI 3.14159265359
#endif

namespace AgpuCommon
{

static char uberShaderSourceCode[] =
#include "uberShader.glsl"
;

bool ImmediateShaderCompilationParameters::operator==(const ImmediateShaderCompilationParameters &other) const
{
	return flatShading == other.flatShading &&
		texturingEnabled == other.texturingEnabled &&
		tangentSpaceEnabled == other.tangentSpaceEnabled &&
		skinningEnabled == other.skinningEnabled &&
		lightingEnabled == other.lightingEnabled &&
		lightingModel == other.lightingModel;
}

size_t ImmediateShaderCompilationParameters::hash() const
{
	return std::hash<bool> ()(flatShading) ^
		std::hash<bool> ()(texturingEnabled) ^
		std::hash<bool> ()(tangentSpaceEnabled) ^
		std::hash<bool> ()(skinningEnabled) ^
		std::hash<bool> ()(lightingEnabled) ^
		std::hash<uint32_t> ()(static_cast<uint32_t> (lightingModel));
}

std::string ImmediateShaderCompilationParameters::shaderOptionsString(agpu_shader_type type) const
{
	std::string options = "#version 450\n";

	switch(type)
	{
	case AGPU_VERTEX_SHADER:
		options += "#define BUILD_VERTEX_SHADER\n";
		break;
	case AGPU_FRAGMENT_SHADER:
		options += "#define BUILD_FRAGMENT_SHADER\n";
		break;
	default:
		break;
	}

	if(flatShading)
		options += "#define FLAT_SHADING\n";
    if (texturingEnabled)
    {
        options += "#define TEXTURING_ENABLED\n";
        if (tangentSpaceEnabled)
            options += "#define TANGENT_SPACE_ENABLED\n";
    }

	if(skinningEnabled)
		options += "#define SKINNING_ENABLED\n";

    if(lightingEnabled)
	{
		if(lightingModel == AGPU_IMMEDIATE_RENDERER_LIGHTING_MODEL_FLAT_COLOR)
		{
			options += "#define FLAT_COLOR_MATERIAL\n";
		}
		else
		{
			if(lightingEnabled)
				options += "#define LIGHTING_ENABLED\n";
			switch(lightingModel)
			{
			case AGPU_IMMEDIATE_RENDERER_LIGHTING_MODEL_PER_VERTEX:
				options += "#define PER_VERTEX_LIGHTING\n";
				break;
			case AGPU_IMMEDIATE_RENDERER_LIGHTING_MODEL_PER_FRAGMENT:
				options += "#define PER_FRAGMENT_LIGHTING\n";
				break;
			case AGPU_IMMEDIATE_RENDERER_LIGHTING_MODEL_METALLIC_ROUGHNESS:
				options += "#define PER_FRAGMENT_LIGHTING\n";
				options += "#define PBR_METALLIC_ROUGHNESS\n";
				break;
			default:
				break;
			}
		}
	}

	return options;
}

agpu::shader_ref ImmediateShaderLibrary::getOrCreateWithCompilationParameters(const agpu::device_ref &device, const ImmediateShaderCompilationParameters &params, agpu_shader_type type)
{
	std::unique_lock<std::mutex> l(shaderCompilationMutex);
	auto &shaderCache = (type == AGPU_VERTEX_SHADER) ? vertexShaderCache : fragmentShaderCache;
	auto it = shaderCache.find(params);
	if(it != shaderCache.end())
		return it->second;

	auto sourceCode = params.shaderOptionsString(type);
	sourceCode += uberShaderSourceCode;

	auto compiler = agpu::offline_shader_compiler_ref(device->createOfflineShaderCompiler());
	compiler->setShaderSource(AGPU_SHADER_LANGUAGE_VGLSL, type, sourceCode.c_str(), sourceCode.size());
	auto error = compiler->compileShader(AGPU_SHADER_LANGUAGE_DEVICE_SHADER, "");
	if(error)
	{
		fprintf(stderr, "Failed to compile immediate renderer shader:\n%s\n", sourceCode.c_str());

		auto logLength = compiler->getCompilationLogLength();
		std::unique_ptr<char[]> log(new char[logLength + 1]);
		compiler->getCompilationLog(logLength, log.get());
		log[logLength] = 0;
		fprintf(stderr, "Compilation error:\n%s\n", log.get());
		abort();
	}

	auto result = agpu::shader_ref(compiler->getResultAsShader());
	shaderCache.insert(std::make_pair(params, result));
	return result;
}

inline bool isSyntheticTopology(agpu_primitive_topology type)
{
	return type >= AGPU_IMMEDIATE_TRIANGLE_FAN;
}

size_t ImmediateTextureBindingSet::hash() const
{
    return std::hash<agpu::texture_ref>()(albedoTexture) ^
        std::hash<agpu::texture_ref>()(emissionTexture) ^
        std::hash<agpu::texture_ref>()(normalTexture) ^
        std::hash<agpu::texture_ref>()(occlusionTexture) ^
		std::hash<agpu::texture_ref>()(roughnessMetallicTexture);
}

bool ImmediateTextureBindingSet::operator==(const ImmediateTextureBindingSet& other) const
{
    return albedoTexture == other.albedoTexture &&
        emissionTexture == other.emissionTexture &&
        normalTexture == other.normalTexture &&
        occlusionTexture == other.occlusionTexture &&
		roughnessMetallicTexture == other.roughnessMetallicTexture;
}
bool ImmediateTextureBindingSet::operator!=(const ImmediateTextureBindingSet& other) const
{
    return !(*this == other);
}

bool TransformationState::operator==(const TransformationState &other) const
{
	return projectionMatrix == other.projectionMatrix &&
	    modelViewMatrix == other.modelViewMatrix &&
	    inverseModelViewMatrix == other.inverseModelViewMatrix &&
	    textureMatrix == other.textureMatrix;
}

size_t TransformationState::hash() const
{
	return projectionMatrix.hash() ^
		modelViewMatrix.hash() ^
		inverseModelViewMatrix.hash() ^
		textureMatrix.hash();
}

bool SkinningState::operator==(const SkinningState &other) const
{
	return memcmp(boneMatrices, other.boneMatrices, sizeof(boneMatrices)) == 0;
}

bool SkinningState::operator!=(const SkinningState &other) const
{
	return memcmp(boneMatrices, other.boneMatrices, sizeof(boneMatrices)) != 0;
}

size_t SkinningState::hash() const
{
	size_t result = 0;
	for(auto &bone : boneMatrices)
		result ^= bone.hash();
	return result;
}

ClassicLightState::ClassicLightState()
    :
    ambientColor(0.0f, 0.0f, 0.0f, 1.0f),
    diffuseColor(0.0f, 0.0f, 0.0f, 1.0f),
    specularColor(0.0f, 0.0f, 0.0f, 1.0f),

    position(0.0f, 0.0f, 1.0f, 0.0f),

    spotDirection(0.0f, 0.0f, -1.0f),
    spotCosCutoff(-1.0f),

    spotExponent(0.0f),
    constantAttenuation(1.0f),
    linearAttenuation(0.0f),
    quadraticAttenuation(0.0f)
{
}

size_t ClassicLightState::hash() const
{
    return
        ambientColor.hash() ^
        diffuseColor.hash() ^
        specularColor.hash() ^

        position.hash() ^
        spotDirection.hash() ^
        std::hash<float> ()(spotCosCutoff) ^

        std::hash<float> () (spotExponent) ^
        std::hash<float> () (constantAttenuation) ^
        std::hash<float> () (linearAttenuation) ^
        std::hash<float> () (quadraticAttenuation);
    ;
}

bool ClassicLightState::operator==(const ClassicLightState &other) const
{
    return
        ambientColor == other.ambientColor &&
        diffuseColor == other.diffuseColor &&
        specularColor == other.specularColor &&

        position == other.position &&
        spotDirection == other.spotDirection &&
        spotCosCutoff == other.spotCosCutoff &&

        spotExponent == other.spotExponent &&
        constantAttenuation == other.constantAttenuation &&
        linearAttenuation == other.linearAttenuation &&
        quadraticAttenuation == other.quadraticAttenuation;
}

PBRLightState::PBRLightState()
    :
	ambient(0.0f, 0.0f, 0.0f, 1.0f),
	intensity(0.0f, 0.0f, 0.0f, 1.0f),

    position(0.0f, 0.0f, 1.0f, 0.0f),

    spotDirection(0.0f, 0.0f, -1.0f),
    spotCosCutoff(-1.0f),

    spotExponent(0.0f),
	spotInnerCosCutoff(-1.0f),
    radius(1.0f),
	padding()
{
}

size_t PBRLightState::hash() const
{
    return
		ambient.hash() ^
        intensity.hash() ^

        position.hash() ^
        spotDirection.hash() ^
        std::hash<float> ()(spotCosCutoff) ^
        std::hash<float> ()(spotInnerCosCutoff) ^

        std::hash<float> () (spotExponent) ^
        std::hash<float> () (radius)
    ;
}

bool PBRLightState::operator==(const PBRLightState &other) const
{
    return
		ambient == other.ambient &&
        intensity == other.intensity &&

        position == other.position &&
        spotDirection == other.spotDirection &&
        spotCosCutoff == other.spotCosCutoff &&

        spotExponent == other.spotExponent &&
        spotInnerCosCutoff == other.spotInnerCosCutoff &&
        radius == other.radius;
}

LightState::LightState()
    : classic(ClassicLightState())
{
}

size_t LightState::hash() const
{
    return classic.hash();
}

bool LightState::operator==(const LightState &other) const
{
    return classic == other.classic;
}

LightingState::LightingState()
    : ambientLighting(0.2f, 0.2f, 0.2f, 1.0f), enabledLightMask(1), padding()
{
    lights[0].classic.diffuseColor = Vector4F(1.0f, 1.0f, 1.0f, 1.0f);
    lights[0].classic.specularColor = Vector4F(1.0f, 1.0f, 1.0f, 1.0f);
}

size_t LightingState::hash() const
{
    // TODO: Implement this.
    size_t result = ambientLighting.hash();
    for(auto & light : lights)
        result ^= light.hash();
    return result;
}

bool LightingState::operator==(const LightingState &other) const
{
    return ambientLighting == other.ambientLighting &&
        lights == other.lights;
}

bool LightingState::operator!=(const LightingState &other) const
{
    return !(*this == other);
}

ClassicMaterialState ClassicMaterialState::defaultMaterial()
{
	auto result = ClassicMaterialState();
	result.type = MaterialStateType::Classic;
    result.shininess = 0.0f;
	result.alphaCutoff = 0;
	result.padding = 0;
    result.emission = Vector4F(0.0f, 0.0f, 0.0f, 1.0f);
    result.ambient = Vector4F(0.2f, 0.2f, 0.2f, 1.0f);
    result.diffuse = Vector4F(0.8f, 0.8f, 0.8f, 1.0f);
    result.specular = Vector4F(0.0f, 0.0f, 0.0f, 1.0f);
	return result;
}

size_t ClassicMaterialState::hash() const
{
    return
        emission.hash() ^
        ambient.hash() ^
        diffuse.hash() ^
        specular.hash() ^
        std::hash<float> ()(shininess) ^
		std::hash<float> ()(alphaCutoff);
}

bool ClassicMaterialState::operator==(const ClassicMaterialState &other) const
{
    return
        emission == other.emission &&
        ambient == other.ambient &&
        diffuse == other.diffuse &&
        specular == other.specular &&
        shininess == other.shininess &&
		alphaCutoff == other.alphaCutoff;
}

bool ClassicMaterialState::operator!=(const ClassicMaterialState &other) const
{
    return !(*this == other);
}

MetallicRoughnessMaterialState MetallicRoughnessMaterialState::defaultMaterial()
{
	auto result = MetallicRoughnessMaterialState();
	result.type = MaterialStateType::MetallicRoughness;
    result.roughnessFactor = 0.0f;
	result.metallicFactor = 0.0f;
	result.occlusionFactor = 1.0f;

	result.alphaCutoff = 0.0f;
	result.padding[0] = 0;
	result.padding[1] = 0;
	result.padding[2] = 0;

    result.emission = Vector4F(0.0f, 0.0f, 0.0f, 1.0f);
    result.baseColor = Vector4F(0.8f, 0.8f, 0.8f, 1.0f);
	return result;
}

size_t MetallicRoughnessMaterialState::hash() const
{
    return
        emission.hash() ^
        baseColor.hash() ^
		std::hash<float> ()(roughnessFactor) ^
        std::hash<float> ()(metallicFactor) ^
		std::hash<float> ()(occlusionFactor) ^
		std::hash<float> ()(alphaCutoff);
}

bool MetallicRoughnessMaterialState::operator==(const MetallicRoughnessMaterialState &other) const
{
    return
        emission == other.emission &&
        baseColor == other.baseColor &&
        roughnessFactor == other.roughnessFactor &&
        metallicFactor == other.metallicFactor &&
		occlusionFactor == other.occlusionFactor &&
		alphaCutoff == other.alphaCutoff;
}

bool MetallicRoughnessMaterialState::operator!=(const MetallicRoughnessMaterialState &other) const
{
    return !(*this == other);
}

FlatColorMaterialState FlatColorMaterialState::defaultMaterial()
{
	auto result = FlatColorMaterialState();
	result.type = MaterialStateType::FlatColor;
	result.alphaCutoff = 0.0f;
	result.padding[0] = 0;
	result.padding[1] = 0;
    result.color = Vector4F(1.0f, 1.0f, 1.0f, 1.0f);
	return result;
}

size_t FlatColorMaterialState::hash() const
{
    return color.hash() ^ std::hash<float> ()(alphaCutoff);
}

bool FlatColorMaterialState::operator==(const FlatColorMaterialState &other) const
{
    return color == other.color;
}

bool FlatColorMaterialState::operator!=(const FlatColorMaterialState &other) const
{
    return !(*this == other);
}

MaterialState::MaterialState()
    : classic(ClassicMaterialState::defaultMaterial())
{}

size_t MaterialState::hash() const
{
	switch(type)
	{
	default:
	case MaterialStateType::Classic:
		return classic.hash();
	case MaterialStateType::MetallicRoughness:
		return metallicRoughness.hash();
	case MaterialStateType::FlatColor:
		return flat.hash();
	}
}

bool MaterialState::operator==(const MaterialState &other) const
{
	if(type == other.type)
		return false;

	switch(type)
	{
	default:
	case MaterialStateType::Classic:
		return classic == other.classic;
	case MaterialStateType::MetallicRoughness:
		return metallicRoughness == other.metallicRoughness;
	case MaterialStateType::FlatColor:
		return flat == other.flat;
	}
}

bool MaterialState::operator!=(const MaterialState &other) const
{
    return !(*this == other);
}

ExtraRenderingState::ExtraRenderingState()
    :
    userClipPlane(0.0f, 0.0f, 0.0f, 0.0f),

    fogMode(AGPU_IMMEDIATE_RENDERER_FOG_MODE_NONE),
    fogStartDistance(0.0f),
    fogEndDistance(1.0f),
    fogDensity(1.0),

    fogColor(0.0f, 0.0f, 0.0f, 0.0f)
{}

size_t ExtraRenderingState::hash() const
{
    return
        userClipPlane.hash() ^

        std::hash<uint32_t> ()(fogMode) ^
        std::hash<float> ()(fogStartDistance) ^
        std::hash<float> ()(fogEndDistance) ^
        std::hash<float> ()(fogDensity) ^

        fogColor.hash();
}

bool ExtraRenderingState::operator==(const ExtraRenderingState &other) const
{
    return
        userClipPlane == other.userClipPlane;
}

bool ExtraRenderingState::operator!=(const ExtraRenderingState &other) const
{
    return !(*this == other);
}

agpu_vertex_attrib_description ImmediateVertexAttributes[] = {
    {0, AGPU_IMMEDIATE_RENDERER_VERTEX_ATTRIBUTE_POSITION, AGPU_TEXTURE_FORMAT_R32G32B32_FLOAT, offsetof(ImmediateRendererVertex, position), 0},
    {0, AGPU_IMMEDIATE_RENDERER_VERTEX_ATTRIBUTE_COLOR, AGPU_TEXTURE_FORMAT_R32G32B32A32_FLOAT, offsetof(ImmediateRendererVertex, color), 0},
    {0, AGPU_IMMEDIATE_RENDERER_VERTEX_ATTRIBUTE_NORMAL, AGPU_TEXTURE_FORMAT_R32G32B32_FLOAT, offsetof(ImmediateRendererVertex, normal), 0},
    {0, AGPU_IMMEDIATE_RENDERER_VERTEX_ATTRIBUTE_TEXCOORD, AGPU_TEXTURE_FORMAT_R32G32_FLOAT, offsetof(ImmediateRendererVertex, texcoord), 0},
};

bool ImmediateRendererSamplerStateDescription::operator==(const ImmediateRendererSamplerStateDescription &other) const
{
	return filter == other.filter
		&& maxAnisotropy == other.maxAnisotropy
		&& addressU == other.addressU
		&& addressV == other.addressV
		&& addressW == other.addressW;
}

bool ImmediateRendererSamplerStateDescription::operator!=(const ImmediateRendererSamplerStateDescription &other) const
{
	return !(*this == other);
}

size_t ImmediateRendererSamplerStateDescription::hash() const
{
	return
		std::hash<uint32_t> ()(uint32_t(filter)) ^
		std::hash<float> ()(maxAnisotropy) ^
		std::hash<uint32_t> ()(uint32_t(addressU)) ^
		std::hash<uint32_t> ()(uint32_t(addressV)) ^
		std::hash<uint32_t> ()(uint32_t(addressW));
}

agpu::shader_resource_binding_ref &ImmediateSharedRenderingStates::getSamplerStateBindingFor(const ImmediateRendererSamplerStateDescription &description)
{
	auto canonicalizedDescription = description;
	if(canonicalizedDescription.filter != AGPU_FILTER_ANISOTROPIC)
	{
		canonicalizedDescription.maxAnisotropy = 1;
	}
	else if(canonicalizedDescription.maxAnisotropy < 2)
	{
		canonicalizedDescription.filter = AGPU_FILTER_MIN_LINEAR_MAG_LINEAR_MIPMAP_LINEAR;
		canonicalizedDescription.maxAnisotropy = 1;
	}

	std::unique_lock<std::mutex> l(samplerStatesMutex);
	auto it = samplerStates.find(canonicalizedDescription);
	if(it != samplerStates.end())
		return it->second.binding;

	ImmediateRendererSamplerState samplerState = {};

	agpu_sampler_description samplerDescription = {};
	samplerDescription.filter = canonicalizedDescription.filter;
	samplerDescription.maxanisotropy = canonicalizedDescription.maxAnisotropy;
	samplerDescription.address_u = canonicalizedDescription.addressU;
	samplerDescription.address_v = canonicalizedDescription.addressV;
	samplerDescription.address_w = canonicalizedDescription.addressW;
	samplerDescription.max_lod = 1000.0f;
	samplerState.sampler = agpu::sampler_ref(device->createSampler(&samplerDescription));
	if(!samplerState.sampler)
		return defaultSampler;

	samplerState.binding = agpu::shader_resource_binding_ref(shaderSignature->createShaderResourceBinding(0));
	if(!samplerState.binding)
		return defaultSampler;
	samplerState.binding->bindSampler(0, samplerState.sampler);
	samplerStates.insert(std::make_pair(canonicalizedDescription, samplerState));
	return samplerStates[canonicalizedDescription].binding;
}

bool StateTrackerCache::ensureImmediateRendererObjectsExists()
{
    std::unique_lock<std::mutex> l(immediateRendererObjectsMutex);

    if(immediateRendererObjectsInitialized)
        return true;

    // Create the shader signature.
    {
        auto builder = agpu::shader_signature_builder_ref(device->createShaderSignatureBuilder());
        if(!builder) return false;

		// Sampling state (Set 0)
        builder->beginBindingBank(1);
        builder->addBindingBankElement(AGPU_SHADER_BINDING_TYPE_SAMPLER, 32);

		// Lighting state (Set 1)
		builder->beginBindingBank(1000);
        builder->addBindingBankElement(AGPU_SHADER_BINDING_TYPE_UNIFORM_BUFFER, 1);

		// Extra rendering state (Set 2)
		builder->beginBindingBank(1000);
        builder->addBindingBankElement(AGPU_SHADER_BINDING_TYPE_UNIFORM_BUFFER, 1);

		// Material state (Set 3)
        builder->beginBindingBank(100000);
        builder->addBindingBankElement(AGPU_SHADER_BINDING_TYPE_UNIFORM_BUFFER, 1);

		// Transformation state (Set 4)
        builder->beginBindingBank(100000);
        builder->addBindingBankElement(AGPU_SHADER_BINDING_TYPE_UNIFORM_BUFFER, 1);

		// Skinning state (Set 5)
        builder->beginBindingBank(1000);
        builder->addBindingBankElement(AGPU_SHADER_BINDING_TYPE_UNIFORM_BUFFER, 1);

		// Textures (Set 6).
        builder->beginBindingBank(10000);
        builder->addBindingBankElement(AGPU_SHADER_BINDING_TYPE_SAMPLED_IMAGE, 1); // Albedo
		builder->addBindingBankElement(AGPU_SHADER_BINDING_TYPE_SAMPLED_IMAGE, 1); // Emission
		builder->addBindingBankElement(AGPU_SHADER_BINDING_TYPE_SAMPLED_IMAGE, 1); // Normal
		builder->addBindingBankElement(AGPU_SHADER_BINDING_TYPE_SAMPLED_IMAGE, 1); // Occlusion
		builder->addBindingBankElement(AGPU_SHADER_BINDING_TYPE_SAMPLED_IMAGE, 1); // Roughness metallic

        immediateShaderSignature = agpu::shader_signature_ref(builder->build());
        if(!immediateShaderSignature) return false;
    }

    // Create the immediate shader library.
    {
        immediateShaderLibrary.reset(new ImmediateShaderLibrary());
        if(!immediateShaderLibrary) return false;
    }

    {
        immediateSharedRenderingStates.reset(new ImmediateSharedRenderingStates());
        if(!immediateSharedRenderingStates) return false;

		ImmediateRendererSamplerStateDescription defaultSampleState = {
			AGPU_FILTER_MIN_LINEAR_MAG_LINEAR_MIPMAP_LINEAR, 1,
			AGPU_TEXTURE_ADDRESS_MODE_WRAP, AGPU_TEXTURE_ADDRESS_MODE_WRAP, AGPU_TEXTURE_ADDRESS_MODE_WRAP
		};
		immediateSharedRenderingStates->device = device;
		immediateSharedRenderingStates->shaderSignature = immediateShaderSignature;
		immediateSharedRenderingStates->defaultSampler = immediateSharedRenderingStates->getSamplerStateBindingFor(defaultSampleState);
    }

	// Create the default white texture.
	{
		agpu_texture_description desc = {};
		desc.type = AGPU_TEXTURE_2D;
		desc.format = AGPU_TEXTURE_FORMAT_R8G8B8A8_UNORM;
		desc.width = 1;
		desc.height = 1;
		desc.depth = 1;
		desc.layers = 1;
		desc.miplevels = 1;
		desc.sample_count = 1;
		desc.sample_quality = 0;
		desc.usage_modes = agpu_texture_usage_mode_mask(AGPU_TEXTURE_USAGE_SAMPLED | AGPU_TEXTURE_USAGE_UPLOADED);
		desc.main_usage_mode = AGPU_TEXTURE_USAGE_SAMPLED;
        desc.clear_value.color = { 1, 1, 1, 1 };
		auto texture = agpu::texture_ref(device->createTexture(&desc));
		if (!texture) return false;

        uint32_t color = 0xFFFFFFFF;
        texture->uploadTextureData(0, 0, 4, 4, &color);

		immediateSharedRenderingStates->defaultAlbedoTexture = texture;
		immediateSharedRenderingStates->defaultEmissionTexture = texture;
		immediateSharedRenderingStates->defaultOcclusionTexture = texture;
		immediateSharedRenderingStates->defaultRoughnessMetallicTexture = texture;
	}

	// Create the default normal texture.
	{
		agpu_texture_description desc = {};
		desc.type = AGPU_TEXTURE_2D;
		desc.format = AGPU_TEXTURE_FORMAT_B8G8R8A8_UNORM;
		desc.width = 1;
		desc.height = 1;
		desc.depth = 1;
		desc.layers = 1;
		desc.miplevels = 1;
		desc.sample_count = 1;
		desc.sample_quality = 0;
		desc.usage_modes = agpu_texture_usage_mode_mask(AGPU_TEXTURE_USAGE_SAMPLED | AGPU_TEXTURE_USAGE_UPLOADED);
		desc.main_usage_mode = AGPU_TEXTURE_USAGE_SAMPLED;
    	auto texture = agpu::texture_ref(device->createTexture(&desc));
		if (!texture) return false;

        uint32_t color = 0x808080FF;
        texture->uploadTextureData(0, 0, 4, 4, &color);
		immediateSharedRenderingStates->defaultNormalTexture = texture;
	}
    // Create the immediate vertex layout.
    {
        immediateVertexLayout = agpu::vertex_layout_ref(device->createVertexLayout());
        if(!immediateVertexLayout) return false;

        agpu_size strides = sizeof(ImmediateRendererVertex);
        auto error = immediateVertexLayout->addVertexAttributeBindings(1, &strides,
            sizeof(ImmediateVertexAttributes) / sizeof(ImmediateVertexAttributes[0]),
            ImmediateVertexAttributes);
        if(error) return false;
    }

    immediateRendererObjectsInitialized = true;
    return true;
}

ImmediateRenderer::ImmediateRenderer(const agpu::state_tracker_cache_ref &stateTrackerCache)
    : stateTrackerCache(stateTrackerCache),
	immediateShaderSignature(stateTrackerCache.as<StateTrackerCache> ()->immediateShaderSignature),
	lightingStateBuffer(immediateShaderSignature),
    extraRenderingStateBuffer(immediateShaderSignature),
    materialStateBuffer(immediateShaderSignature),
    transformationStateBuffer(immediateShaderSignature),
	skinningStateBuffer(immediateShaderSignature)
{
    vertexBufferCapacity = 0;
    indexBufferCapacity = 0;
    usedTextureBindingCount = 0;
    activeMatrixStack = nullptr;
	haveFlushedRenderingState = false;

    auto impl = stateTrackerCache.as<StateTrackerCache> ();
    device = impl->device;

    immediateShaderLibrary = impl->immediateShaderLibrary.get();
    immediateSharedRenderingStates = impl->immediateSharedRenderingStates.get();
    immediateVertexLayout = impl->immediateVertexLayout;
}

ImmediateRenderer::~ImmediateRenderer()
{
}

agpu::immediate_renderer_ref ImmediateRenderer::create(const agpu::state_tracker_cache_ref &cache)
{
    auto cacheImpl = cache.as<StateTrackerCache> ();
    if(!cacheImpl->ensureImmediateRendererObjectsExists())
        return agpu::immediate_renderer_ref();

    auto vertexBinding = agpu::vertex_binding_ref(cacheImpl->device->createVertexBinding(cacheImpl->immediateVertexLayout));
    if(!vertexBinding)
        return agpu::immediate_renderer_ref();

    auto result = agpu::makeObject<ImmediateRenderer> (cache);
    result.as<ImmediateRenderer> ()->vertexBinding = vertexBinding;

    return result;
}

agpu_error ImmediateRenderer::beginRendering(const agpu::state_tracker_ref &state_tracker)
{
    if(!state_tracker)
        return AGPU_NULL_POINTER;
    if(currentStateTracker)
        return AGPU_INVALID_OPERATION;

    currentStateTracker = state_tracker;
    activeMatrixStack = nullptr;
    activeMatrixStackDirtyFlag = nullptr;

    // Reset the matrices.
    projectionMatrixStack.clear();
    projectionMatrixStack.push_back(Matrix4F::identity());
    projectionMatrixStackDirtyFlag = true;

    modelViewMatrixStack.clear();
    modelViewMatrixStack.push_back(Matrix4F::identity());
    modelViewMatrixStackDirtyFlag = true;

    textureMatrixStack.clear();
    textureMatrixStack.push_back(Matrix4F::identity());
    textureMatrixStackDirtyFlag = true;

	// Reset the transformation state buffer.
    transformationStateBuffer.reset();

	// Reset the skinning state buffer.
	skinningStateBuffer.reset();

    // Reset the rendering state.
    currentRenderingState = ImmediateRenderingState();
	currentRenderingState.samplingStateBinding = immediateSharedRenderingStates->defaultSampler;

    // Reset the texture bindings.
    usedTextureBindingMap.clear();
    usedTextureBindingCount = 0;

    // Reset the lighting state.
    lightingStateBuffer.reset();

    // Reset the material state.
    materialStateBuffer.reset();

    // Reset the extra rendering state.
    extraRenderingStateBuffer.reset();

    // Reset the vertices.
    lastDrawnVertexIndex = 0;
    vertices.clear();
    currentVertex.color = Vector4F(1.0f, 1.0f, 1.0f, 1.0f);
    currentVertex.normal = Vector3F(0.0f, 0.0f, 1.0f);
    currentVertex.texcoord = Vector2F(0.0f, 0.0f);

    // Reset the indices.
    indices.clear();

    // Reset the immediate meshes.
    renderingImmediateMesh = false;
	haveExplicitVertexBinding = false;
	haveExplicitIndexBuffer = false;
    currentImmediateMeshBaseVertex = 0;
    currentImmediateMeshVertexCount = 0;

    return AGPU_OK;
}

agpu_error ImmediateRenderer::endRendering()
{
    if(!currentStateTracker)
        return AGPU_INVALID_OPERATION;

    flushRenderingData();
	lastFlushedRenderingState = ImmediateRenderingState();
	haveFlushedRenderingState = false;
    for(auto &command : pendingRenderingCommands)
	{
        command();
	}

	lastFlushedRenderingState = ImmediateRenderingState();
	haveFlushedRenderingState = false;
    pendingRenderingCommands.clear();
    currentStateTracker.reset();
    return AGPU_OK;
}

agpu_error ImmediateRenderer::setBlendState(agpu_int renderTargetMask, agpu_bool enabled)
{
    return delegateToStateTracker([=]{
        currentStateTracker->setBlendState(renderTargetMask, enabled);
    });
}

agpu_error ImmediateRenderer::setBlendFunction(agpu_int renderTargetMask, agpu_blending_factor sourceFactor, agpu_blending_factor destFactor, agpu_blending_operation colorOperation, agpu_blending_factor sourceAlphaFactor, agpu_blending_factor destAlphaFactor, agpu_blending_operation alphaOperation)
{
    return delegateToStateTracker([=]{
        currentStateTracker->setBlendFunction(renderTargetMask, sourceFactor, destFactor, colorOperation, sourceAlphaFactor, destAlphaFactor, alphaOperation);
    });
}

agpu_error ImmediateRenderer::setColorMask(agpu_int renderTargetMask, agpu_bool redEnabled, agpu_bool greenEnabled, agpu_bool blueEnabled, agpu_bool alphaEnabled)
{
    return delegateToStateTracker([=]{
        currentStateTracker->setColorMask(renderTargetMask, redEnabled, greenEnabled, blueEnabled, alphaEnabled);
    });
}

agpu_error ImmediateRenderer::setFrontFace(agpu_face_winding winding)
{
    return delegateToStateTracker([=]{
        currentStateTracker->setFrontFace(winding);
    });
}

agpu_error ImmediateRenderer::setCullMode(agpu_cull_mode mode)
{
    return delegateToStateTracker([=]{
        currentStateTracker->setCullMode(mode);
    });
}

agpu_error ImmediateRenderer::setDepthBias(agpu_float constant_factor, agpu_float clamp, agpu_float slope_factor)
{
    return delegateToStateTracker([=]{
        currentStateTracker->setDepthBias(constant_factor, clamp, slope_factor);
    });
}

agpu_error ImmediateRenderer::setDepthState(agpu_bool enabled, agpu_bool writeMask, agpu_compare_function function)
{
    return delegateToStateTracker([=]{
        currentStateTracker->setDepthState(enabled, writeMask, function);
    });
}

agpu_error ImmediateRenderer::setPolygonMode(agpu_polygon_mode mode)
{
    return delegateToStateTracker([=]{
        currentStateTracker->setPolygonMode(mode);
    });
}

agpu_error ImmediateRenderer::setStencilState(agpu_bool enabled, agpu_int writeMask, agpu_int readMask)
{
    return delegateToStateTracker([=]{
        currentStateTracker->setStencilState(enabled, writeMask, readMask);
    });
}

agpu_error ImmediateRenderer::setStencilFrontFace(agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction)
{
    return delegateToStateTracker([=]{
        currentStateTracker->setStencilFrontFace(stencilFailOperation, depthFailOperation, stencilDepthPassOperation, stencilFunction);
    });
}

agpu_error ImmediateRenderer::setStencilBackFace(agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction)
{
    return delegateToStateTracker([=]{
        currentStateTracker->setStencilBackFace(stencilFailOperation, depthFailOperation, stencilDepthPassOperation, stencilFunction);
    });
}

agpu_error ImmediateRenderer::setViewport(agpu_int x, agpu_int y, agpu_int w, agpu_int h)
{
    return delegateToStateTracker([=]{
        currentStateTracker->setViewport(x, y, w, h);
    });
}

agpu_error ImmediateRenderer::setScissor(agpu_int x, agpu_int y, agpu_int w, agpu_int h)
{
    return delegateToStateTracker([=]{
        currentStateTracker->setScissor(x, y, w, h);
    });
}

agpu_error ImmediateRenderer::setStencilReference(agpu_uint reference)
{
    return delegateToStateTracker([=]{
        currentStateTracker->setStencilReference(reference);
    });
}

agpu_error ImmediateRenderer::setFlatShading(agpu_bool enabled)
{
    currentRenderingState.flatShading = enabled;
    return AGPU_OK;
}

agpu_error ImmediateRenderer::setLightingEnabled(agpu_bool enabled)
{
    currentRenderingState.lightingEnabled = enabled;
    return AGPU_OK;
}


agpu_error ImmediateRenderer::setLightingModel(agpu_immediate_renderer_lighting_model model)
{
	currentRenderingState.lightingModel = model;
    return AGPU_OK;
}

agpu_error ImmediateRenderer::clearLights()
{
    auto newState = lightingStateBuffer.currentState;
    newState.lights = LightingState().lights;
    newState.enabledLightMask = 0;
    lightingStateBuffer.setState(newState);
    return AGPU_OK;
}

agpu_error ImmediateRenderer::setAmbientLighting(agpu_float r, agpu_float g, agpu_float b, agpu_float a)
{
    auto newAmbientLighting = Vector4F(r, g, b, a);
    if(lightingStateBuffer.currentState.ambientLighting != newAmbientLighting)
    {
        lightingStateBuffer.currentState.ambientLighting = newAmbientLighting;
        lightingStateBuffer.makeDirty();
    }

    return AGPU_OK;
}

agpu_error ImmediateRenderer::setLight(agpu_uint index, agpu_bool enabled, agpu_immediate_renderer_light* state)
{
    if(index >= LightingState::MaxLightCount)
        return AGPU_OK;

    uint32_t bit = 1<<index;
    if(enabled)
    {
        lightingStateBuffer.currentState.enabledLightMask |= bit;

        if(state)
        {
            auto &light = lightingStateBuffer.currentState.lights[index];
			if(hasMetallicRoughnessLighting())
			{
				light.pbr = PBRLightState();
				light.pbr.ambient = Vector4F(state->pbr.ambient, 0);
	            light.pbr.intensity = Vector4F(state->pbr.intensity, 0);
	            light.pbr.position = modelViewMatrixStack.back()*Vector4F(state->pbr.position);
	            light.pbr.spotDirection = modelViewMatrixStack.back().transformDirection3(Vector3F(state->pbr.spot_direction));
	            light.pbr.spotExponent = state->pbr.spot_exponent;
	            light.pbr.spotCosCutoff = float(cos(state->pbr.spot_cutoff*M_PI/180.0));
				light.pbr.spotInnerCosCutoff = float(cos(state->pbr.spot_inner_cutoff*M_PI/180.0));
	            light.pbr.radius = state->pbr.radius;
			}
			else
			{
				light.classic = ClassicLightState();
				light.classic.ambientColor = Vector4F(state->classic.ambient);
	            light.classic.diffuseColor = Vector4F(state->classic.diffuse);
	            light.classic.specularColor = Vector4F(state->classic.specular);
	            light.classic.position = modelViewMatrixStack.back()*Vector4F(state->classic.position);
	            light.classic.spotDirection = modelViewMatrixStack.back().transformDirection3(Vector3F(state->classic.spot_direction));
	            light.classic.spotExponent = state->classic.spot_exponent;
	            light.classic.spotCosCutoff = float(cos(state->classic.spot_cutoff*M_PI/180.0));
	            light.classic.constantAttenuation = state->classic.constant_attenuation;
	            light.classic.linearAttenuation = state->classic.linear_attenuation;
	            light.classic.quadraticAttenuation = state->classic.quadratic_attenuation;
			}
        }
    }
    else
    {
        lightingStateBuffer.currentState.enabledLightMask &= ~bit;
    }
    lightingStateBuffer.makeDirty();

    return AGPU_OK;
}

agpu_error ImmediateRenderer::setClipPlane(agpu_uint index, agpu_bool enabled, agpu_float p1, agpu_float p2, agpu_float p3, agpu_float p4)
{
    // We only support a single user clip plane, for now.
    if(index > 0)
        return AGPU_OK;

    auto newClipPlane = Vector4F(0.0f, 0.0f, 0.0f, 0.0f);
    if(enabled)
    {
        auto modelViewMatrix = modelViewMatrixStack.back();
        auto inverseModelViewMatrix = modelViewMatrix.inverted();

        newClipPlane = Vector4F(p1, p2, p3, p4)*inverseModelViewMatrix;
    }

    if(extraRenderingStateBuffer.currentState.userClipPlane != newClipPlane)
    {
        extraRenderingStateBuffer.currentState.userClipPlane = newClipPlane;
        extraRenderingStateBuffer.makeDirty();
    }

    return AGPU_OK;
}

agpu_error ImmediateRenderer::setFogMode(agpu_immediate_renderer_fog_mode mode)
{
    if(extraRenderingStateBuffer.currentState.fogMode != mode)
    {
        extraRenderingStateBuffer.currentState.fogMode = mode;
        extraRenderingStateBuffer.makeDirty();
    }

    return AGPU_OK;
}

agpu_error ImmediateRenderer::setFogColor(agpu_float r, agpu_float g, agpu_float b, agpu_float a)
{
    auto color = Vector4F(r, g, b, a);

    if(extraRenderingStateBuffer.currentState.fogColor != color)
    {
        extraRenderingStateBuffer.currentState.fogColor = color;
        extraRenderingStateBuffer.makeDirty();
    }

    return AGPU_OK;
}

agpu_error ImmediateRenderer::setFogDistances(agpu_float start, agpu_float end)
{
    if(extraRenderingStateBuffer.currentState.fogStartDistance != start ||
       extraRenderingStateBuffer.currentState.fogEndDistance != end)
    {
        extraRenderingStateBuffer.currentState.fogStartDistance = start;
        extraRenderingStateBuffer.currentState.fogEndDistance = end;
        extraRenderingStateBuffer.makeDirty();
    }
    return AGPU_OK;
}

agpu_error ImmediateRenderer::setFogDensity(agpu_float density)
{
    if(extraRenderingStateBuffer.currentState.fogDensity != density)
    {
        extraRenderingStateBuffer.currentState.fogDensity = density;
        extraRenderingStateBuffer.makeDirty();
    }
    return AGPU_OK;
}

agpu_error ImmediateRenderer::setMaterial(agpu_immediate_renderer_material* state)
{
    if(!state)
        return AGPU_NULL_POINTER;

    MaterialState newMaterialState;
	if(hasMetallicRoughnessLighting())
	{
		newMaterialState.metallicRoughness = MetallicRoughnessMaterialState::defaultMaterial();
		newMaterialState.metallicRoughness.emission = Vector4F(state->metallic_roughness.emission);
	    newMaterialState.metallicRoughness.baseColor = Vector4F(state->metallic_roughness.base_color);
	    newMaterialState.metallicRoughness.metallicFactor = state->metallic_roughness.metallic_factor;
		newMaterialState.metallicRoughness.roughnessFactor = state->metallic_roughness.roughness_factor;
		newMaterialState.metallicRoughness.occlusionFactor = state->metallic_roughness.occlusion_factor;
		newMaterialState.metallicRoughness.alphaCutoff = state->metallic_roughness.alpha_cutoff;
	}
	else if(hasFlatColorLightingMode())
	{
		newMaterialState.flat = FlatColorMaterialState::defaultMaterial();
		newMaterialState.flat.color = Vector4F(state->flat_color.color);
		newMaterialState.flat.alphaCutoff = state->flat_color.alpha_cutoff;
	}
	else
	{
		newMaterialState.classic = ClassicMaterialState::defaultMaterial();
		newMaterialState.classic.emission = Vector4F(state->classic.emission);
	    newMaterialState.classic.ambient = Vector4F(state->classic.ambient);
	    newMaterialState.classic.diffuse = Vector4F(state->classic.diffuse);
	    newMaterialState.classic.specular = Vector4F(state->classic.specular);
	    newMaterialState.classic.shininess = state->classic.shininess;
		newMaterialState.classic.alphaCutoff = state->classic.alpha_cutoff;
	}
    materialStateBuffer.setState(newMaterialState);
    return AGPU_OK;
}

agpu_error ImmediateRenderer::setTexturingEnabled(agpu_bool enabled)
{
    currentRenderingState.texturingEnabled = enabled;
    return AGPU_OK;
}

agpu_error ImmediateRenderer::setSamplingMode(agpu_filter filter, agpu_float maxAnisotropy, agpu_texture_address_mode addressU, agpu_texture_address_mode addressV, agpu_texture_address_mode addressW)
{
	ImmediateRendererSamplerStateDescription desc = {};
	desc.filter = filter;
	desc.maxAnisotropy = maxAnisotropy;
	desc.addressU = addressU;
	desc.addressV = addressV;
	desc.addressW = addressW;
	currentRenderingState.samplingStateBinding = immediateSharedRenderingStates->getSamplerStateBindingFor(desc);
	return AGPU_OK;
}

agpu_error ImmediateRenderer::setTangentSpaceEnabled(agpu_bool enabled)
{
    currentRenderingState.tangentSpaceEnabled = enabled;
    return AGPU_OK;
}

agpu_error ImmediateRenderer::bindTexture(const agpu::texture_ref &texture)
{
    // For compatibility with existent OpenGL behavior, bind as albedo and emission.
    currentRenderingState.textureBindingSet.albedoTexture = texture;
    currentRenderingState.textureBindingSet.emissionTexture = texture;
    return AGPU_OK;
}

agpu_error ImmediateRenderer::bindTextureIn(const agpu::texture_ref & texture, agpu_immediate_renderer_texture_binding binding)
{
	switch(binding)
	{
	case AGPU_IMMEDIATE_RENDERER_TEXTURE_BINDING_ALBEDO:
	    currentRenderingState.textureBindingSet.albedoTexture = texture;
		return AGPU_OK;
	case AGPU_IMMEDIATE_RENDERER_TEXTURE_BINDING_EMISSION:
	    currentRenderingState.textureBindingSet.emissionTexture = texture;
		return AGPU_OK;
	case AGPU_IMMEDIATE_RENDERER_TEXTURE_BINDING_NORMAL:
	    currentRenderingState.textureBindingSet.normalTexture = texture;
		return AGPU_OK;
	case AGPU_IMMEDIATE_RENDERER_TEXTURE_BINDING_ROUGHNESS_METALLIC_AMBIENT:
	    currentRenderingState.textureBindingSet.occlusionTexture = texture;
		currentRenderingState.textureBindingSet.roughnessMetallicTexture = texture;
		return AGPU_OK;
	case AGPU_IMMEDIATE_RENDERER_TEXTURE_BINDING_AMBIENT_OCCLUSION:
		currentRenderingState.textureBindingSet.occlusionTexture = texture;
		return AGPU_OK;
	case AGPU_IMMEDIATE_RENDERER_TEXTURE_BINDING_ROUGHNESS_METALLIC:
		currentRenderingState.textureBindingSet.roughnessMetallicTexture = texture;
		return AGPU_OK;
	default: return AGPU_UNSUPPORTED;
	}
}

agpu_error ImmediateRenderer::setSkinningEnabled(agpu_bool enabled)
{
	currentRenderingState.skinningEnabled = enabled;
    return AGPU_OK;
}

agpu_error ImmediateRenderer::setSkinBones(agpu_uint count, agpu_float* matrices, agpu_bool transpose)
{
	auto actualBoneCount = std::min(count, (agpu_uint)SkinningState::MaxNumberOfBones);
	auto newState = SkinningState();

	auto sourcePointer = matrices;
	for(agpu_uint i = 0; i < actualBoneCount; ++i, sourcePointer += 16)
	{
		auto convertedMatrix = Matrix4F(
			Vector4F(sourcePointer[0], sourcePointer[1], sourcePointer[2], sourcePointer[3]),
			Vector4F(sourcePointer[4], sourcePointer[5], sourcePointer[6], sourcePointer[7]),
			Vector4F(sourcePointer[8], sourcePointer[9], sourcePointer[10], sourcePointer[11]),
			Vector4F(sourcePointer[12], sourcePointer[13], sourcePointer[14], sourcePointer[15]));
		if(transpose)
			convertedMatrix = convertedMatrix.transposed();
		newState.boneMatrices[i] = convertedMatrix;
	}

	skinningStateBuffer.setState(newState);
	return AGPU_OK;
}

agpu::shader_resource_binding_ref ImmediateRenderer::getValidTextureBindingFor(const ImmediateTextureBindingSet &bindingSet)
{
	auto sanitizedBindingSet = bindingSet;
    if(!sanitizedBindingSet.albedoTexture)
		sanitizedBindingSet.albedoTexture = immediateSharedRenderingStates->defaultAlbedoTexture;
	if(!sanitizedBindingSet.emissionTexture)
		sanitizedBindingSet.emissionTexture = immediateSharedRenderingStates->defaultEmissionTexture;
	if(!sanitizedBindingSet.normalTexture)
		sanitizedBindingSet.normalTexture = immediateSharedRenderingStates->defaultNormalTexture;
	if(!sanitizedBindingSet.occlusionTexture)
		sanitizedBindingSet.occlusionTexture = immediateSharedRenderingStates->defaultOcclusionTexture;
	if(!sanitizedBindingSet.roughnessMetallicTexture)
		sanitizedBindingSet.roughnessMetallicTexture = immediateSharedRenderingStates->defaultRoughnessMetallicTexture;

    // Do we have an existing binding.
    auto it = usedTextureBindingMap.find(sanitizedBindingSet);
    if(it != usedTextureBindingMap.end())
        return it->second;

    // Get or create an available texture binding.
    agpu::shader_resource_binding_ref textureBinding;
    if(usedTextureBindingCount < allocatedTextureBindings.size())
    {
        textureBinding = allocatedTextureBindings[usedTextureBindingCount++];
    }
    else
    {
        textureBinding = agpu::shader_resource_binding_ref(stateTrackerCache.as<StateTrackerCache> ()->immediateShaderSignature->createShaderResourceBinding(6));
        allocatedTextureBindings.push_back(textureBinding);
        ++usedTextureBindingCount;
    }

    // Bind the texture on the binding point.
    textureBinding->bindSampledTextureView(0, agpu::texture_view_ref(sanitizedBindingSet.albedoTexture->getOrCreateFullView()));
	textureBinding->bindSampledTextureView(1, agpu::texture_view_ref(sanitizedBindingSet.emissionTexture->getOrCreateFullView()));
	textureBinding->bindSampledTextureView(2, agpu::texture_view_ref(sanitizedBindingSet.normalTexture->getOrCreateFullView()));
	textureBinding->bindSampledTextureView(3, agpu::texture_view_ref(sanitizedBindingSet.occlusionTexture->getOrCreateFullView()));
	textureBinding->bindSampledTextureView(4, agpu::texture_view_ref(sanitizedBindingSet.roughnessMetallicTexture->getOrCreateFullView()));
    usedTextureBindingMap[sanitizedBindingSet] = textureBinding;

    return textureBinding;
}

agpu_error ImmediateRenderer::projectionMatrixMode()
{
    activeMatrixStack = &projectionMatrixStack;
    activeMatrixStackDirtyFlag = &projectionMatrixStackDirtyFlag;
    return AGPU_OK;
}

agpu_error ImmediateRenderer::modelViewMatrixMode()
{
    activeMatrixStack = &modelViewMatrixStack;
    activeMatrixStackDirtyFlag = &modelViewMatrixStackDirtyFlag;
    return AGPU_OK;
}

agpu_error ImmediateRenderer::textureMatrixMode()
{
    activeMatrixStack = &textureMatrixStack;
    activeMatrixStackDirtyFlag = &textureMatrixStackDirtyFlag;
    return AGPU_OK;
}

agpu_error ImmediateRenderer::loadIdentity()
{
    if(!activeMatrixStack)
        return AGPU_INVALID_OPERATION;

    activeMatrixStack->back() = Matrix4F::identity();
    invalidateMatrix();
    return AGPU_OK;
}

agpu_error ImmediateRenderer::pushMatrix()
{
    if(!activeMatrixStack)
        return AGPU_INVALID_OPERATION;

    activeMatrixStack->push_back(activeMatrixStack->back());
    return AGPU_OK;
}

agpu_error ImmediateRenderer::popMatrix()
{
    if(!activeMatrixStack || activeMatrixStack->size() <= 1)
        return AGPU_INVALID_OPERATION;

    activeMatrixStack->pop_back();
    invalidateMatrix();
    return AGPU_OK;
}

agpu_error ImmediateRenderer::loadMatrix(agpu_float* elements)
{
    activeMatrixStack->back() = Matrix4F(
        Vector4F(elements[0], elements[1], elements[2], elements[3]),
        Vector4F(elements[4], elements[5], elements[6], elements[7]),
        Vector4F(elements[8], elements[9], elements[10], elements[11]),
        Vector4F(elements[12], elements[13], elements[14], elements[15])
    );

    return AGPU_OK;
}

agpu_error ImmediateRenderer::loadTransposeMatrix(agpu_float* elements)
{
    activeMatrixStack->back() = Matrix4F(
        Vector4F(elements[0], elements[4], elements[8], elements[12]),
        Vector4F(elements[1], elements[5], elements[9], elements[13]),
        Vector4F(elements[2], elements[6], elements[10], elements[14]),
        Vector4F(elements[3], elements[7], elements[11], elements[15])
    );

    return AGPU_OK;
}

agpu_error ImmediateRenderer::multiplyMatrix(agpu_float* elements)
{
    applyMatrix(Matrix4F(
        Vector4F(elements[0], elements[1], elements[2], elements[3]),
        Vector4F(elements[4], elements[5], elements[6], elements[7]),
        Vector4F(elements[8], elements[9], elements[10], elements[11]),
        Vector4F(elements[12], elements[13], elements[14], elements[15])
    ));
    return AGPU_OK;
}

agpu_error ImmediateRenderer::multiplyTransposeMatrix(agpu_float* elements)
{
    applyMatrix(Matrix4F(
        Vector4F(elements[0], elements[4], elements[8], elements[12]),
        Vector4F(elements[1], elements[5], elements[9], elements[13]),
        Vector4F(elements[2], elements[6], elements[10], elements[14]),
        Vector4F(elements[3], elements[7], elements[11], elements[15])
    ));
    return AGPU_OK;
}

agpu_error ImmediateRenderer::ortho(agpu_float left, agpu_float right, agpu_float bottom, agpu_float top, agpu_float near, agpu_float far)
{
    if(!activeMatrixStack)
        return AGPU_INVALID_OPERATION;

    auto tx = -(right + left)/(right - left);
    auto ty = -(top + bottom)/(top - bottom);
    auto tz = -near/(far - near);

    auto matrix = Matrix4F(
        Vector4F(2.0f/(right - left), 0.0f, 0.0f, 0.0f),
        Vector4F(0.0f, 2.0f/(top - bottom), 0.0f, 0.0f),
        Vector4F(0.0f, 0.0f, -1.0f/(far - near), 0.0f),
        Vector4F(tx, ty, tz, 1.0f)
    );

    // Flip the Y axis
    if(device->hasTopLeftNdcOrigin() != device->hasBottomLeftTextureCoordinates())
    {
        matrix.c2.y = -matrix.c2.y;
        matrix.c4.y = -matrix.c4.y;
    }

    applyMatrix(matrix);
    return AGPU_OK;
}

agpu_error ImmediateRenderer::frustum(agpu_float left, agpu_float right, agpu_float bottom, agpu_float top, agpu_float near, agpu_float far)
{
    if(!activeMatrixStack)
        return AGPU_INVALID_OPERATION;

    auto matrix = Matrix4F(
        Vector4F(2.0f*near / (right - left), 0.0f, 0.0f, 0.0f),
        Vector4F(0.0f, 2.0f*near / (top - bottom), 0.0f, 0.0f),
        Vector4F(0.0f, 0.0f, -far/(far - near), -1.0f),
        Vector4F((right + left) / (right - left), (top + bottom) / (top - bottom),  -near * far / (far - near), 0.0f)
    );

    // Flip the Y axis
    if (device->hasTopLeftNdcOrigin() != device->hasBottomLeftTextureCoordinates())
    {
        matrix.c2.y = -matrix.c2.y;
        matrix.c3.y = -matrix.c3.y;
    }

    applyMatrix(matrix);
    return AGPU_OK;
}

agpu_error ImmediateRenderer::perspective(agpu_float fovy, agpu_float aspect, agpu_float near, agpu_float far)
{
    auto radians = fovy*float(M_PI/180.0f*0.5f);
    auto top = near * float(tan(radians));
    auto right = top * aspect;
    return frustum(-right, right, -top, top, near, far);
}

agpu_error ImmediateRenderer::rotate(agpu_float angle, agpu_float vx, agpu_float vy, agpu_float vz)
{
    if(!activeMatrixStack)
        return AGPU_INVALID_OPERATION;

    auto radians = angle*M_PI/180.0f;
    auto c = float(cos(radians));
    auto s = float(sin(radians));

    auto l = float(sqrt(vx*vx + vy*vy + vz*vz));
    auto x = vx / l;
    auto y = vy / l;
    auto z = vz / l;

    // Formula from: https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glRotate.xml
    applyMatrix(Matrix4F(
        Vector4F(x*x*(1 - c) +   c, y*x*(1 - c) + z*s, z*x*(1 - c) - y*s, 0.0f),
        Vector4F(x*y*(1 - c) - z*s, y*y*(1 - c) +   c, z*y*(1 - c) + x*s, 0.0f),
        Vector4F(x*z*(1 - c) + y*s, y*z*(1 - c) - x*s, z*z*(1 - c) +   c, 0.0f),
        Vector4F(0.0f, 0.0f, 0.0f, 1.0f)
    ));
    return AGPU_OK;
}

agpu_error ImmediateRenderer::translate(agpu_float x, agpu_float y, agpu_float z)
{
    if(!activeMatrixStack)
        return AGPU_INVALID_OPERATION;

    applyMatrix(Matrix4F(
        Vector4F(1.0f, 0.0f, 0.0f, 0.0f),
        Vector4F(0.0f, 1.0f, 0.0f, 0.0f),
        Vector4F(0.0f, 0.0f, 1.0f, 0.0f),
        Vector4F(x, y, z, 1.0f)
    ));
    return AGPU_OK;
}

agpu_error ImmediateRenderer::scale(agpu_float x, agpu_float y, agpu_float z)
{
    if(!activeMatrixStack)
        return AGPU_INVALID_OPERATION;

    applyMatrix(Matrix4F(
        Vector4F(x, 0.0f, 0.0f, 0.0f),
        Vector4F(0.0f, y, 0.0f, 0.0f),
        Vector4F(0.0f, 0.0f, z, 0.0f),
        Vector4F(0.0f, 0.0f, 0.0f, 1.0f)
    ));
    return AGPU_OK;
}

agpu_error ImmediateRenderer::beginPrimitives(agpu_primitive_topology type)
{
    if(!currentStateTracker)
        return AGPU_INVALID_OPERATION;

    auto error = validateRenderingStates();
    if(error) return error;

    currentRenderingState.activePrimitiveTopology = type;
    lastDrawnVertexIndex = vertices.size();

    auto stateToRender = currentRenderingState;
	stateToRender.tangentSpaceEnabled = false;
    pendingRenderingCommands.push_back([=]{
        flushRenderingState(stateToRender);
    });


    return AGPU_OK;
}


agpu_error ImmediateRenderer::validateRenderingStates()
{
    auto error = validateTransformationState();
    if(error) return error;

	error = validateSkinningState();
    if(error) return error;

    error = validateLightingState();
    if(error) return error;

    error = validateMaterialState();
    if(error) return error;

    error = validateExtraRenderingState();
    if(error) return error;

    return AGPU_OK;
}

agpu_error ImmediateRenderer::endPrimitives()
{
    if(!currentStateTracker)
        return AGPU_INVALID_OPERATION;

    auto vertexCount = vertices.size() - lastDrawnVertexIndex;
    if(vertexCount > 0)
    {
        auto vertexStart = lastDrawnVertexIndex;
		// In the case of synthetic primitive topologies, we need to generate the indices manually.
		auto synthetic = isSyntheticTopology(currentRenderingState.activePrimitiveTopology);
		if (synthetic)
		{
			auto firstIndex = indices.size();
			auto baseVertex = lastDrawnVertexIndex;
			switch (currentRenderingState.activePrimitiveTopology)
			{
			case AGPU_IMMEDIATE_POLYGON:
			case AGPU_IMMEDIATE_TRIANGLE_FAN:
				if (vertexCount >= 3)
				{
					for (size_t i = 2; i < vertexCount; ++i)
					{
						indices.push_back(0);
						indices.push_back(i-1);
						indices.push_back(i);
					}
				}
				break;
			case AGPU_IMMEDIATE_QUADS:
				if (vertexCount >= 4)
				{
					size_t quadCount = vertexCount / 4;
					auto quadBaseIndex = 0;
					for (size_t i = 0; i < quadCount; ++i)
					{
						auto qi0 = quadBaseIndex;
						auto qi1 = quadBaseIndex + 1;
						auto qi2 = quadBaseIndex + 2;
						auto qi3 = quadBaseIndex + 3;

						indices.push_back(qi0);
						indices.push_back(qi1);
						indices.push_back(qi2);

						indices.push_back(qi2);
						indices.push_back(qi3);
						indices.push_back(qi0);

						quadBaseIndex += 4;
					}
				}
				break;
			default:
				break;
			}
			auto indexCount = indices.size() - firstIndex;
			if (indexCount > 0)
			{
				pendingRenderingCommands.push_back([=] {
					auto error = flushImmediateVertexRenderingState();
					if (!error)
					{
						currentStateTracker->useIndexBuffer(indexBuffer);
						currentStateTracker->drawElements(indexCount, 1, firstIndex, baseVertex, 0);
					}
				});
			}
		}
		else
		{
			pendingRenderingCommands.push_back([=] {
				auto error = flushImmediateVertexRenderingState();
				if (!error)
				{
					currentStateTracker->drawArrays(vertexCount, 1, vertexStart, 0);
				}
			});

		}

    }

    return AGPU_OK;
}

agpu_error ImmediateRenderer::color(agpu_float r, agpu_float g, agpu_float b, agpu_float a)
{
    if(!currentStateTracker)
        return AGPU_INVALID_OPERATION;

    currentVertex.color = Vector4F(r, g, b, a);
    return AGPU_OK;
}

agpu_error ImmediateRenderer::texcoord(agpu_float x, agpu_float y)
{
    if(!currentStateTracker)
        return AGPU_INVALID_OPERATION;

    currentVertex.texcoord = Vector2F(x, y);
    return AGPU_OK;
}

agpu_error ImmediateRenderer::normal(agpu_float x, agpu_float y, agpu_float z)
{
    if(!currentStateTracker)
        return AGPU_INVALID_OPERATION;

    currentVertex.normal = Vector3F(x, y, z);
    return AGPU_OK;
}

agpu_error ImmediateRenderer::vertex(agpu_float x, agpu_float y, agpu_float z)
{
    if(!currentStateTracker)
        return AGPU_INVALID_OPERATION;

    currentVertex.position = Vector3F(x, y, z);
    vertices.push_back(currentVertex);
    return AGPU_OK;
}

void ImmediateRenderer::applyMatrix(const Matrix4F &matrix)
{
    activeMatrixStack->back() *= matrix;
    invalidateMatrix();
}

void ImmediateRenderer::invalidateMatrix()
{
    *activeMatrixStackDirtyFlag = true;
}

agpu_error ImmediateRenderer::validateLightingState()
{
    if(lightingStateBuffer.isDirty())
		currentRenderingState.lightingStateBinding = lightingStateBuffer.validateCurrentState();

    return AGPU_OK;
}

agpu_error ImmediateRenderer::validateMaterialState()
{
    if(materialStateBuffer.isDirty())
        currentRenderingState.materialStateBinding = materialStateBuffer.validateCurrentState();

    return AGPU_OK;
}

agpu_error ImmediateRenderer::validateSkinningState()
{
	if(skinningStateBuffer.isDirty())
        currentRenderingState.skinningStateBinding = skinningStateBuffer.validateCurrentState();

    return AGPU_OK;
}

agpu_error ImmediateRenderer::validateTransformationState()
{
    if(projectionMatrixStackDirtyFlag)
    {
		transformationStateBuffer.currentState.projectionMatrix = projectionMatrixStack.back();
		transformationStateBuffer.makeDirty();
    }

    if(modelViewMatrixStackDirtyFlag)
    {
		transformationStateBuffer.currentState.modelViewMatrix = modelViewMatrixStack.back();
		transformationStateBuffer.makeDirty();
    }

    if(textureMatrixStackDirtyFlag)
    {
		transformationStateBuffer.currentState.textureMatrix = textureMatrixStack.back();
		transformationStateBuffer.makeDirty();
    }

	if(transformationStateBuffer.isDirty())
		currentRenderingState.transformationStateBinding = transformationStateBuffer.validateCurrentState();

    return AGPU_OK;
}

agpu_error ImmediateRenderer::validateExtraRenderingState()
{
    if(extraRenderingStateBuffer.isDirty())
		currentRenderingState.extraRenderingStateBinding = extraRenderingStateBuffer.validateCurrentState();

    return AGPU_OK;
}


agpu_error ImmediateRenderer::flushImmediateVertexRenderingState()
{
    currentStateTracker->setVertexLayout(immediateVertexLayout);
    currentStateTracker->useVertexBinding(vertexBinding);
    return AGPU_OK;
}

agpu_error ImmediateRenderer::flushShadersForRenderingState(const ImmediateRenderingState &state)
{
	if(!haveFlushedRenderingState)
	{
		currentStateTracker->setShaderSignature(immediateShaderSignature);
	}
	else
	{
		// Do we need to flush this?
		if(state.flatShading == lastFlushedRenderingState.flatShading &&
			state.texturingEnabled == lastFlushedRenderingState.texturingEnabled &&
			state.lightingEnabled == lastFlushedRenderingState.lightingEnabled &&
			state.lightingModel == lastFlushedRenderingState.lightingModel &&
			state.skinningEnabled == lastFlushedRenderingState.skinningEnabled &&
			state.tangentSpaceEnabled == lastFlushedRenderingState.tangentSpaceEnabled)
			return AGPU_OK;
	}

	ImmediateShaderCompilationParameters parameters;
	parameters.flatShading = state.flatShading;
	parameters.texturingEnabled = state.texturingEnabled;
    parameters.tangentSpaceEnabled = state.tangentSpaceEnabled;
    parameters.skinningEnabled = state.skinningEnabled;
	parameters.lightingEnabled = state.lightingEnabled;
	parameters.lightingModel = state.lightingModel;
	currentStateTracker->setVertexStage(immediateShaderLibrary->getOrCreateWithCompilationParameters(device, parameters, AGPU_VERTEX_SHADER), "main");
	currentStateTracker->setFragmentStage(immediateShaderLibrary->getOrCreateWithCompilationParameters(device, parameters, AGPU_FRAGMENT_SHADER), "main");

	return AGPU_OK;
}

agpu_error ImmediateRenderer::flushRenderingState(const ImmediateRenderingState &state)
{
	flushShadersForRenderingState(state);

	if(!haveFlushedRenderingState || state.activePrimitiveTopology != lastFlushedRenderingState.activePrimitiveTopology)
    	currentStateTracker->setPrimitiveType(isSyntheticTopology(state.activePrimitiveTopology) ? AGPU_TRIANGLES : state.activePrimitiveTopology);

	if(state.samplingStateBinding && (!haveFlushedRenderingState || state.samplingStateBinding != lastFlushedRenderingState.samplingStateBinding))
	{
		currentStateTracker->useShaderResources(state.samplingStateBinding);
	}

	if(state.lightingStateBinding && (!haveFlushedRenderingState || state.lightingStateBinding != lastFlushedRenderingState.lightingStateBinding))
	{
		currentStateTracker->useShaderResources(state.lightingStateBinding);
	}

	if(state.extraRenderingStateBinding && (!haveFlushedRenderingState || state.extraRenderingStateBinding != lastFlushedRenderingState.extraRenderingStateBinding))
	{
		currentStateTracker->useShaderResources(state.extraRenderingStateBinding);
	}

	if(state.materialStateBinding && (!haveFlushedRenderingState || state.materialStateBinding != lastFlushedRenderingState.materialStateBinding))
	{
		currentStateTracker->useShaderResources(state.materialStateBinding);
	}

	if(state.transformationStateBinding && (!haveFlushedRenderingState || state.transformationStateBinding != lastFlushedRenderingState.transformationStateBinding))
	{
		currentStateTracker->useShaderResources(state.transformationStateBinding);
	}

    if(state.texturingEnabled && (!haveFlushedRenderingState || state.textureBindingSet != lastFlushedRenderingState.textureBindingSet))
	{
        currentStateTracker->useShaderResources(getValidTextureBindingFor(state.textureBindingSet));
	}

	if(state.skinningStateBinding && (!haveFlushedRenderingState || state.skinningStateBinding != lastFlushedRenderingState.skinningStateBinding))
	{
		currentStateTracker->useShaderResources(state.skinningStateBinding);
	}

	lastFlushedRenderingState = state;
	haveFlushedRenderingState = true;

    return AGPU_OK;
}

agpu_error ImmediateRenderer::flushRenderingData()
{
    static constexpr size_t GpuBufferDataThreshold = 1024*256;

    // Upload the vertices.
    // TODO: Use an immediate state buffer for the vertices and the indices.
    agpu_error error = AGPU_OK;
    if(!vertexBuffer || vertexBufferCapacity < vertices.size())
    {
        vertexBufferCapacity = nextPowerOfTwo(vertices.size());
        if(vertexBufferCapacity < 32)
            vertexBufferCapacity = 32;

        auto requiredSize = vertexBufferCapacity*sizeof(ImmediateRendererVertex);
        agpu_buffer_description bufferDescription = {};
        bufferDescription.size = requiredSize;
        bufferDescription.heap_type = requiredSize >= GpuBufferDataThreshold
            ? AGPU_MEMORY_HEAP_TYPE_DEVICE_LOCAL : AGPU_MEMORY_HEAP_TYPE_HOST_TO_DEVICE;
        bufferDescription.usage_modes = bufferDescription.main_usage_mode = AGPU_ARRAY_BUFFER;
        bufferDescription.mapping_flags = AGPU_MAP_DYNAMIC_STORAGE_BIT;
        bufferDescription.stride = sizeof(ImmediateRendererVertex);

        vertexBuffer = agpu::buffer_ref(device->createBuffer(&bufferDescription, nullptr));
        if(!vertexBuffer)
            return AGPU_OUT_OF_MEMORY;

        vertexBinding->bindVertexBuffers(1, &vertexBuffer);
    }

    if(!vertices.empty())
    {
        error = vertexBuffer->uploadBufferData(0, vertices.size()*sizeof(ImmediateRendererVertex), &vertices[0]);
        if(error)
            return error;
    }

    // Upload the indices.
    if(!indices.empty() && (!indexBuffer || indexBufferCapacity < indices.size()))
    {
        indexBufferCapacity = nextPowerOfTwo(indices.size());
        if(indexBufferCapacity < 32)
            indexBufferCapacity = 32;

        auto requiredSize = indexBufferCapacity*sizeof(uint32_t);
        agpu_buffer_description bufferDescription = {};
        bufferDescription.size = requiredSize;
        bufferDescription.heap_type = requiredSize >= GpuBufferDataThreshold
            ? AGPU_MEMORY_HEAP_TYPE_DEVICE_LOCAL : AGPU_MEMORY_HEAP_TYPE_HOST_TO_DEVICE;
        bufferDescription.usage_modes = bufferDescription.main_usage_mode = AGPU_ELEMENT_ARRAY_BUFFER;
        bufferDescription.mapping_flags = AGPU_MAP_DYNAMIC_STORAGE_BIT;
        bufferDescription.stride = sizeof(uint32_t);

        indexBuffer = agpu::buffer_ref(device->createBuffer(&bufferDescription, nullptr));
        if(!indexBuffer)
            return AGPU_OUT_OF_MEMORY;
    }

    if(!indices.empty())
    {
        error = indexBuffer->uploadBufferData(0, indices.size()*sizeof(uint32_t), &indices[0]);
        if(error)
            return error;
    }

    // Upload the immediate state buffers.
    error = transformationStateBuffer.uploadData(device);
    if(error) return error;

    error = lightingStateBuffer.uploadData(device);
    if(error) return error;

    error = materialStateBuffer.uploadData(device);
    if(error) return error;

    error = extraRenderingStateBuffer.uploadData(device);
    if(error) return error;

    error = skinningStateBuffer.uploadData(device);
    if (error) return error;

    return AGPU_OK;
}

agpu_error ImmediateRenderer::beginMeshWithVertices(agpu_size vertexCount, agpu_size stride, agpu_size elementCount, agpu_pointer positionsPointer)
{
    if(renderingImmediateMesh)
        return AGPU_INVALID_OPERATION;

    renderingImmediateMesh = true;
	haveExplicitVertexBinding = false;
	haveExplicitIndexBuffer = false;
    currentImmediateMeshBaseVertex = vertices.size();
    currentImmediateMeshVertexCount = vertexCount;
    vertices.reserve(vertexCount);

    auto positionsBytes = reinterpret_cast<const uint8_t*> (positionsPointer);
    for(size_t i = 0; i < vertexCount; ++i)
    {
        auto vertexPositions = reinterpret_cast<const float*> (positionsBytes);
        vertices.push_back(this->currentVertex);

        auto &destPositions = vertices.back().position;
        destPositions = Vector3F(vertexPositions[0]);
        if(elementCount > 1)
        {
            destPositions.y = vertexPositions[1];
            if(elementCount > 2)
                destPositions.z = vertexPositions[2];
        }
        positionsBytes += stride;
    }

    pendingRenderingCommands.push_back([=]{
        auto error = flushImmediateVertexRenderingState();
        if(!error)
        {
            currentStateTracker->useIndexBuffer(indexBuffer);
        }
    });

    return AGPU_OK;
}

agpu_error ImmediateRenderer::beginMeshWithVertexBinding(const agpu::vertex_layout_ref & layout, const agpu::vertex_binding_ref & vertices)
{
	if(renderingImmediateMesh)
		return AGPU_INVALID_OPERATION;

	renderingImmediateMesh = true;
	haveExplicitVertexBinding = true;
	haveExplicitIndexBuffer = false;
	currentImmediateMeshBaseVertex = 0;
    currentImmediateMeshVertexCount = 0;

	pendingRenderingCommands.push_back([=]{
		currentStateTracker->setVertexLayout(layout);
		currentStateTracker->useVertexBinding(vertices);
        currentStateTracker->useIndexBuffer(indexBuffer);
    });

	return AGPU_OK;
}

agpu_error ImmediateRenderer::useIndexBuffer(const agpu::buffer_ref & index_buffer)
{
	if(!index_buffer)
        return AGPU_NULL_POINTER;

	agpu_buffer_description description;
	index_buffer->getDescription(&description);
	return useIndexBufferAt(index_buffer, 0, description.stride);
}

agpu_error ImmediateRenderer::useIndexBufferAt(const agpu::buffer_ref & index_buffer, agpu_size offset, agpu_size index_size)
{
	if(!index_buffer)
        return AGPU_NULL_POINTER;
	if(!renderingImmediateMesh)
        return AGPU_INVALID_OPERATION;

	haveExplicitIndexBuffer = true;
	pendingRenderingCommands.push_back([=]{
        currentStateTracker->useIndexBufferAt(index_buffer, offset, index_size);
    });

	return AGPU_OK;
}

agpu_error ImmediateRenderer::setCurrentMeshColors(agpu_size stride, agpu_size elementCount, agpu_pointer colors)
{
    if(!renderingImmediateMesh || haveExplicitVertexBinding)
        return AGPU_INVALID_OPERATION;

    auto colorBytes = reinterpret_cast<const uint8_t*> (colors);
    for(size_t i = 0; i < currentImmediateMeshVertexCount; ++i)
    {
        auto colorValues = reinterpret_cast<const float*> (colorBytes);
        auto &destColor = vertices[currentImmediateMeshBaseVertex + i].color;
        destColor = Vector4F(colorValues[0]);
        if(elementCount > 1)
        {
            destColor.y = colorValues[1];
            if(elementCount > 2)
            {
                destColor.z = colorValues[2];
                if(elementCount > 3)
                    destColor.w = colorValues[3];
            }
        }

        colorBytes += stride;
    }

    return AGPU_OK;
}

agpu_error ImmediateRenderer::setCurrentMeshNormals(agpu_size stride, agpu_size elementCount, agpu_pointer normals)
{
    if(!renderingImmediateMesh || haveExplicitVertexBinding)
        return AGPU_INVALID_OPERATION;

    auto normalBytes = reinterpret_cast<const uint8_t*> (normals);
    for(size_t i = 0; i < currentImmediateMeshVertexCount; ++i)
    {
        auto normalValues = reinterpret_cast<const float*> (normalBytes);
        auto &destNormal = vertices[currentImmediateMeshBaseVertex + i].normal;
        destNormal = Vector3F(normalValues[0]);
        if(elementCount > 1)
        {
            destNormal.y = normalValues[1];
            if(elementCount > 2)
                destNormal.z = normalValues[2];
        }

        normalBytes += stride;
    }

    return AGPU_OK;
}

agpu_error ImmediateRenderer::setCurrentMeshTexCoords(agpu_size stride, agpu_size elementCount, agpu_pointer texcoords)
{
    if(!renderingImmediateMesh || haveExplicitVertexBinding)
        return AGPU_INVALID_OPERATION;

    auto texcoordBytes = reinterpret_cast<const uint8_t*> (texcoords);
    for(size_t i = 0; i < currentImmediateMeshVertexCount; ++i)
    {
        auto texcoordValues = reinterpret_cast<const float*> (texcoordBytes);
        auto &destTexcoord = vertices[currentImmediateMeshBaseVertex + i].texcoord;
        destTexcoord = Vector2F(texcoordValues[0]);
        if(elementCount > 1)
            destTexcoord.y = texcoordValues[1];

        texcoordBytes += stride;
    }

    return AGPU_OK;
}

agpu_error ImmediateRenderer::drawElementsWithIndices(agpu_primitive_topology mode, agpu_pointer indicesPointer, agpu_uint index_count, agpu_uint instance_count, agpu_uint first_index, agpu_int base_vertex, agpu_uint base_instance)
{
    if(!renderingImmediateMesh || haveExplicitIndexBuffer)
        return AGPU_INVALID_OPERATION;

    auto error = validateRenderingStates();
    if(error) return error;

    size_t baseIndex = indices.size();
    auto indicesValues = reinterpret_cast<uint32_t*> (indicesPointer);
    auto actualBaseVertex = currentImmediateMeshBaseVertex + base_vertex;
    auto stateToRender = currentRenderingState;
    switch(mode)
    {
    // Directly supported modes. Just copy the data.
    case AGPU_POINTS:
	case AGPU_LINES:
	case AGPU_LINES_ADJACENCY:
	case AGPU_LINE_STRIP:
	case AGPU_LINE_STRIP_ADJACENCY:
	case AGPU_TRIANGLES:
	case AGPU_TRIANGLES_ADJACENCY:
	case AGPU_TRIANGLE_STRIP:
	case AGPU_TRIANGLE_STRIP_ADJACENCY:
	case AGPU_PATCHES:
        {
            indices.insert(indices.end(), indicesValues, indicesValues + index_count);
            stateToRender.activePrimitiveTopology = mode;
            pendingRenderingCommands.push_back([=]{
                auto error = flushRenderingState(stateToRender);
                if(!error)
				{
                    currentStateTracker->drawElements(index_count, instance_count, baseIndex + first_index, actualBaseVertex, base_instance);
				}
            });
        }
        break;
    case AGPU_IMMEDIATE_POLYGON:
    case AGPU_IMMEDIATE_TRIANGLE_FAN:
        if(index_count >= 3)
        {
            indicesValues += first_index;
            auto i0 = indicesValues[0];
            auto convertedIndexCount = 0;
            for(size_t i = 2; i < index_count; ++i)
            {
                indices.push_back(i0);
                indices.push_back(indicesValues[i - 1]);
                indices.push_back(indicesValues[i]);
                convertedIndexCount += 3;
            }

            stateToRender.activePrimitiveTopology = AGPU_TRIANGLES;
            pendingRenderingCommands.push_back([=]{
                auto error = flushRenderingState(stateToRender);
                if(!error)
				{
                    currentStateTracker->drawElements(convertedIndexCount, instance_count, baseIndex, actualBaseVertex, base_instance);
				}
            });
        }
        return AGPU_OK;
    case AGPU_IMMEDIATE_QUADS:
        if(index_count >= 4)
        {
            indicesValues += first_index;
            size_t quadCount = index_count / 4;
            auto convertedIndexCount = 0;
            for(size_t i = 0; i < quadCount; ++i)
            {
                auto qi0 = indicesValues[0];
                auto qi1 = indicesValues[1];
                auto qi2 = indicesValues[2];
                auto qi3 = indicesValues[3];

                indices.push_back(qi0);
                indices.push_back(qi1);
                indices.push_back(qi2);

                indices.push_back(qi2);
                indices.push_back(qi3);
                indices.push_back(qi0);

                indicesValues += 4;
                convertedIndexCount += 6;
            }


            stateToRender.activePrimitiveTopology = AGPU_TRIANGLES;
            pendingRenderingCommands.push_back([=]{
                auto error = flushRenderingState(stateToRender);
                if(!error)
				{
                    currentStateTracker->drawElements(convertedIndexCount, instance_count, baseIndex, actualBaseVertex, base_instance);
				}
            });
        }
        return AGPU_OK;
    default:
        return AGPU_OK;
    }

    return AGPU_OK;
}

agpu_error ImmediateRenderer::setPrimitiveType(agpu_primitive_topology type)
{
	currentRenderingState.activePrimitiveTopology = type;
	return AGPU_OK;
}

agpu_error ImmediateRenderer::drawArrays(agpu_uint vertex_count, agpu_uint instance_count, agpu_uint first_vertex, agpu_uint base_instance)
{
	if(!renderingImmediateMesh)
		return AGPU_INVALID_OPERATION;

	auto error = validateRenderingStates();
    if(error) return error;

	auto stateToRender = currentRenderingState;
	pendingRenderingCommands.push_back([=]{
		auto error = flushRenderingState(stateToRender);
		if(!error)
		{
			currentStateTracker->drawArrays(vertex_count, instance_count, first_vertex, base_instance);
		}
	});

    return AGPU_OK;
}

agpu_error ImmediateRenderer::drawElements(agpu_uint index_count, agpu_uint instance_count, agpu_uint first_index, agpu_int base_vertex, agpu_uint base_instance)
{
	if(!renderingImmediateMesh || !haveExplicitIndexBuffer)
		return AGPU_INVALID_OPERATION;

	auto error = validateRenderingStates();
    if(error) return error;

	auto stateToRender = currentRenderingState;
	pendingRenderingCommands.push_back([=]{
		auto error = flushRenderingState(stateToRender);
		if(!error)
		{
			currentStateTracker->drawElements(index_count, instance_count, first_index, base_vertex, base_instance);
		}
	});

    return AGPU_OK;
}

agpu_error ImmediateRenderer::endMesh()
{
    if(!renderingImmediateMesh)
        return AGPU_INVALID_OPERATION;

    renderingImmediateMesh = false;
	haveExplicitVertexBinding = false;
	haveExplicitIndexBuffer = false;
    return AGPU_OK;
}

} // End of namespace AgpuCommon
