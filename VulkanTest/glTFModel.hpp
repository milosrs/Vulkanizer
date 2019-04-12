#pragma once
#ifndef GLTF_MODEL_HPP
#define GLTF_MODEL_HPP
#endif

#include <glm/glm.hpp>
#include <Magick++.h>
#include <memory>
#include <algorithm>
#include <vector>
#include "StreamReader.hpp"
#include "PLATFORM.h"

class UniformBuffer;
class IndexBuffer;
class CommandBufferHandler;
class Buffer;
class Texture;
template<typename T> class VertexBuffer;
template<typename T> class StagingBuffer;

#define MAX_NUM_JOINTS 128u

namespace vkglTF{
	struct Node;
	
	struct Skin {
		std::string name;
		vkglTF::Node *skeletonRoot = nullptr;
		std::vector<glm::mat4> inverseBindMatrices;
		std::vector<vkglTF::Node*> joints;
	};

	struct AnimationChannel {
		enum PathType { TRANSLATION, ROTATION, SCALE, WEIGHTS };
		PathType path;
		Node *node;
		std::string samplerId;
	};

	struct AnimationSampler {
		enum Interpolation { LINEAR, STEP, CUBICSPLINE };
		Interpolation interpolation;
		std::vector<float> inputs;
		std::vector<glm::vec4> outputs;
		std::vector<float> weights;			//Weights of morph targets
	};

	struct Animation {
		std::string name;
		std::vector<AnimationSampler> samplers;
		std::vector<AnimationChannel> channels;
		float start = std::numeric_limits<float>::max();
		float end = std::numeric_limits<float>::min();
	};

	struct TextureSampler {
		std::string id;
		VkFilter magFilter;
		VkFilter minFilter;
		VkSamplerAddressMode U;
		VkSamplerAddressMode V;
		VkSamplerAddressMode W;
	};

	struct Vertex {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 uv0;
		glm::vec2 uv1;
		glm::vec4 joint0;
		glm::vec4 weight0;
	};

	struct Material {
		enum AlphaMode { ALPHA_OPAQUE, ALPHA_MASK, ALPHA_BLEND };

		/*
			Koji model povrsine treba da se koristi
		*/
		struct PBRWorkflows {
			bool specularGlossiness = false;
			bool metallicRoughness = true;
		} PBRWorkflow;

		/*
			Teksture po PBR specifikaciji
		*/
		struct TextureCoordinateSets {
			uint8_t baseColor = 0;
			uint8_t metallicRoughness = 0;
			uint8_t specularGlossiness = 0;
			uint8_t normal = 0;
			uint8_t occlusion = 0;
			uint8_t emmisive = 0;
		}TexCoordinateSets;

		/*
			Dodatni efekti kod naseg 3D modela
		*/
		struct Extension {
			Texture* specularGlossinessTexture = nullptr;
			Texture* diffuseTexture = nullptr;

			glm::vec4 diffuseFactor = glm::vec4(1.0f);
			glm::vec4 specularFactor = glm::vec4(1.0f);
		};

		std::string materialId;
		VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
		AlphaMode alphaMode = AlphaMode::ALPHA_OPAQUE;
		float alphaCutoff = 1.0f;
		float metallicFactor = 1.0f;
		float roughnessFactor = 1.0f;
		glm::vec4 baseColorFactor = glm::vec4(1.0f);
		glm::vec4 emissiveFactor = glm::vec4(1.0f);
		Texture* baseColorTexture = nullptr;
		Texture* metallicRoughnessTexture = nullptr;
		Texture* normalTexture = nullptr;
		Texture* oclussionTexture = nullptr;
		Texture* emissiveTexture = nullptr;
	};

	struct BuffersAccessors {
		Microsoft::glTF::Accessor accessor;
		Microsoft::glTF::BufferView view;
		Microsoft::glTF::Buffer buffer;

		void operator=(BuffersAccessors accessors);
	};

	template<typename T> struct BufferInfo {
		BuffersAccessors buffersAccessors;
		std::vector<T> bufferData;

		BufferInfo<T>();

		BufferInfo<T>(BuffersAccessors buffersAccessors, std::vector<T> bufferData);

		const void operator=(const BufferInfo<T> &info) {
			buffersAccessors = info.buffersAccessors;
			bufferData = info.bufferData;
		};
	};

	struct BoundingBox {
		glm::vec3 min;
		glm::vec3 max;
		bool valid = false;

		BoundingBox getBoundingBoxData(glm::mat4 m);
		BoundingBox();
		BoundingBox(glm::vec3 min, glm::vec3 max);
	};

	struct Primitive {
		uint32_t firstIndex;
		uint32_t indexCount;
		Material material;
		BoundingBox boundingBox;

		Primitive(uint32_t firstIndex, uint32_t indexCount, Material material);
		void setBoundingBox(glm::vec3 min, glm::vec3 max);
	};

	struct Mesh {
		struct UniformBlock {
			glm::mat4 matrix;
			glm::mat4 jointMatrix[MAX_NUM_JOINTS]{};
			float jointCount{ 0 };
		} UniformBlock;

		VkDevice device;
		std::vector<Primitive*> primitives;
		UniformBuffer *uniformBuffer = nullptr;
		BoundingBox bb;
		BoundingBox aabb;

		Mesh(VkDevice device, VkPhysicalDeviceMemoryProperties memprops, glm::mat4 matrix);
		~Mesh();
		void setBoundingBox(glm::vec3 min, glm::vec3 max);
	};

	struct Node {
		Node *parent;
		std::vector<Node*> children;
		glm::mat4 matrix;
		std::string name;
		std::string id;
		Mesh *mesh = nullptr;
		Skin *skin = nullptr;
		std::string skinId = "";
		glm::vec3 translation{};
		glm::vec3 scale{ 1.0f };
		glm::quat rotation{};
		BoundingBox bb;
		BoundingBox aabb;

		glm::mat4 localMatrix();
		glm::mat4 getMatrix();
		void update(VkDevice device, VkPhysicalDeviceMemoryProperties memprops);
		~Node();
	};

	struct ModelInfo {
		VkDevice device;
		VkPhysicalDeviceMemoryProperties memprops;
		VkQueue transferQueue;
		VkCommandPool cmdPool;
		std::filesystem::path modelPath;

		glm::mat4 aabb;
		std::vector<Node*> nodes;
		std::vector<Node*> linearNodes;
		std::vector<Skin*> skins;
		std::vector<Texture*> textures;
		std::vector<TextureSampler> textureSamplers;
		std::vector<Material> materials;
		std::vector<Animation> animations;
		std::vector<std::string> extensions;

		StagingBuffer<vkglTF::Vertex> *vertexData = nullptr;
		StagingBuffer<uint32_t> *indexData = nullptr;
		VertexBuffer<vkglTF::Vertex> *vertexBuffer = nullptr;
		IndexBuffer *indexBuffer = nullptr;

		struct Dimensions {
			glm::vec3 min = glm::vec3(FLT_MAX);
			glm::vec3 max = glm::vec3(-FLT_MAX);
		} Dimensions;

		struct Vertices {
			VkBuffer buffer = VK_NULL_HANDLE;
			VkDeviceMemory memory;
		} Vertices;

		struct Indices {
			int count;
			VkBuffer buffer = VK_NULL_HANDLE;
			VkDeviceMemory memory;
		} Indices;

		enum BufferViewTarget
		{
			UNKNOWN_BUFFER = 0,
			ARRAY_BUFFER = 34962,
			ELEMENT_ARRAY_BUFFER = 34963
		};

		Node* findNode(Node *parent, std::string id);
		Node* nodeFromId(std::string id);
		void loadNode(Node *parent, Microsoft::glTF::Node node, Microsoft::glTF::Document document, std::string nodeId,
			std::vector<uint32_t>& indexBuffer, std::vector<vkglTF::Vertex>& vertexBuffer, float globalScale);
		void loadSkins(Microsoft::glTF::Document &document);
		void loadTextures(Microsoft::glTF::Document document, std::string folderPath);
		void loadTextureSamplers(Microsoft::glTF::Document document);
		void loadMaterials(Microsoft::glTF::Document document);
		void loadAnimations(Microsoft::glTF::Document document);
		void loadFromDocument(Microsoft::glTF::Document* document, std::string modelFolder, VkDevice* device, VkPhysicalDeviceMemoryProperties* memprops,
			VkQueue* queue, VkCommandPool cmdPool, float globalScale = 1.0f);
		void drawNode(Node *node, VkCommandBuffer cmdBuffer);
		void draw(VkCommandBuffer cmdBuffer);
		void calculateBoundingBox(Node *node, Node *parent);
		void getSceneDimensions();
		void destroy();
		VkSamplerAddressMode getVkWrapMode(int32_t wrapMode);
		VkFilter getVkFilterMode(int32_t filterMode);
	};

	template<typename T>
	std::vector<T> readBufferData(Microsoft::glTF::Document document, BufferInfo<T> bufferInfo, std::filesystem::path path);

	template<typename T>
	BufferInfo<T> getFullBufferData(Microsoft::glTF::Document document, std::string accessorKey, std::filesystem::path path);

	Texture* readTextureFromglTFImage(Microsoft::glTF::Image image, Microsoft::glTF::Document document, std::string folder,
		Microsoft::glTF::Texture gltfTexture, vkglTF::TextureSampler sampler, VkDevice device, VkPhysicalDeviceMemoryProperties memprops,
		VkQueue transferQueue);

	BuffersAccessors getBufferAccessorFromDocument(Microsoft::glTF::Document document, std::string accessorId);
};