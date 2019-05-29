#pragma once
#include "pch.h"
#include "glTFModel.hpp"
#include "Util.h"
#include "UniformBuffer.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "StagingBuffer.h"
#include "CommandBufferHandler.h"
#include "MainWindow.h"
#include "Buffer.h"
#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#define MAX_NUM_JOINTS 128u
#define NOMINMAX

namespace vkglTF {
	BuffersAccessors getBufferAccessorFromDocument(Microsoft::glTF::Document document, std::string accessorId) {
		Microsoft::glTF::Accessor accessor = document.accessors.Get(accessorId);
		Microsoft::glTF::BufferView view = document.bufferViews.Get(accessor.bufferViewId);
		Microsoft::glTF::Buffer buffer = document.buffers.Get(view.bufferId);

		BuffersAccessors ret{};
		ret.accessor = accessor;
		ret.buffer = buffer;
		ret.view = view;

		return ret;
	}

	template<typename T>
	std::vector<T> readBufferData(Microsoft::glTF::Document document, BufferInfo<T> bufferInfo, std::filesystem::path path) {
		std::vector<T> stream;

		if (bufferInfo.buffersAccessors.buffer.uri.length() > 0 || bufferInfo.buffersAccessors.buffer.byteLength > 0) {
			Microsoft::glTF::Buffer buffer = bufferInfo.buffersAccessors.buffer;

			path = std::filesystem::absolute(path);

			std::shared_ptr<StreamReader> streamReader = std::make_shared<StreamReader>(path);
			Microsoft::glTF::GLTFResourceReader reader(streamReader);

			stream = reader.ReadBinaryData<T>(document, bufferInfo.buffersAccessors.accessor);
		}
		
		return stream;
	}

	template<typename T>
	BufferInfo<T> getFullBufferData(Microsoft::glTF::Document document, std::string accessorKey, std::filesystem::path path) {
		BufferInfo<T> bufferInfo{};
		std::vector<T> bufferData;

		if (accessorKey.length() > 0) {
			BuffersAccessors mainPart = getBufferAccessorFromDocument(document, accessorKey);

			bufferInfo.buffersAccessors = mainPart;
			bufferData = vkglTF::readBufferData<T>(document, bufferInfo, path);
			bufferInfo.bufferData = bufferData;
		}

		return bufferInfo;
	}

	Texture* readTextureFromglTFImage(Microsoft::glTF::Image image, Microsoft::glTF::Document document, std::string folder,
		Microsoft::glTF::Texture gltfTexture, vkglTF::TextureSampler sampler, 
		VkDevice device, VkPhysicalDeviceMemoryProperties memprops, VkQueue transferQueue) {
		Texture *texture = nullptr;
		VkDeviceSize bufferSize = VK_NULL_HANDLE;
		bool deleteBuffer = false;

		std::string imagePath = folder += image.uri;
		
		if (std::filesystem::exists(imagePath)) {
			std::filesystem::path path = imagePath;
			texture = new Texture(device, &memprops, VK_FORMAT_R8G8B8A8_UNORM, imagePath, 4, nullptr, &sampler);
			texture->beginCreatingTexture(MainWindow::getInstance().getCommandHandler()->getCommandPool(), transferQueue);
			texture->setTextureId(gltfTexture.id);
			texture->setTextureType(image.uri);
		}

		return texture;
	}

	void BuffersAccessors::operator=(BuffersAccessors accessors) {
		accessor = accessors.accessor;
		view = accessors.view;
		buffer = accessors.buffer;
	};

	template<typename T>
	BufferInfo<T>::BufferInfo<T>() {};

	template<typename T>
	BufferInfo<T>::BufferInfo<T>(BuffersAccessors buffersAccessors, std::vector<T> bufferData) {
		this->buffersAccessors = buffersAccessors;
		this->bufferData = bufferData;
	}


	BoundingBox::BoundingBox() {};
	BoundingBox::BoundingBox(glm::vec3 min, glm::vec3 max) :min(min), max(max) {};

	BoundingBox BoundingBox::getBoundingBoxData(glm::mat4 m) {
		glm::vec3 min = glm::vec3(m[3]);
		glm::vec3 max = min;
		glm::vec3 v0, v1;
		glm::vec3 right = glm::vec3(m[0]);
		glm::vec3 up = glm::vec3(m[1]);
		glm::vec3 back = glm::vec3(m[2]);

		v0 = right * this->min.x;
		v1 = right * this->max.x;
		min += glm::min(v0, v1);
		max += glm::max(v0, v1);

		v0 = up * this->min.y;
		v1 = right * this->max.y;
		min += glm::min(v0, v1);
		max += glm::max(v0, v1);

		v0 = back * this->min.z;
		v1 = back * this->max.z;
		min += glm::min(v0, v1);
		max += glm::max(v0, v1);

		return BoundingBox(min, max);
	};

	Primitive::Primitive(uint32_t firstIndex, uint32_t indexCount, Material material) :firstIndex(firstIndex),
			indexCount(indexCount), material(material) { };

	void Primitive::setBoundingBox(glm::vec3 min, glm::vec3 max) {
		boundingBox = BoundingBox(min, max);
	}

	Mesh::Mesh(VkDevice device, VkPhysicalDeviceMemoryProperties memprops, glm::mat4 matrix) {
		this->device = device;
		this->UniformBlock.matrix = matrix;
		uniformBuffer = new UniformBuffer(device, memprops, sizeof(glm::mat4));
	}

	Mesh::~Mesh() {
		delete uniformBuffer;
		for (Primitive *p : primitives) {
			delete p;
		}
	};

	void Mesh::setBoundingBox(glm::vec3 min, glm::vec3 max) {
		bb.min = min;
		bb.max = max;
		bb.valid = true;
	}

	glm::mat4 Node::localMatrix() {
		return glm::translate(glm::mat4(1.0f), translation) * glm::mat4(rotation) * glm::scale(glm::mat4(1.0f), scale) * matrix;
	}

	glm::mat4 Node::getMatrix() {
		glm::mat4 m = localMatrix();
		Node *p = parent;

		while (p) {
			m = p->localMatrix() * m;
			p = p->parent;
		}

		return m;
	}

	void Node::update(VkDevice device, VkPhysicalDeviceMemoryProperties memprops) {
		if (mesh) {
			glm::mat4 m = getMatrix();

			if (skin) {
				mesh->UniformBlock.matrix = m;

				glm::mat4 inverseTransform = glm::inverse(m);
				size_t numJoints = glm::min((uint32_t)skin->joints.size(), MAX_NUM_JOINTS);

				for (size_t i = 0; i < numJoints; i++) {
					Node *jointNode = skin->joints[i];
					glm::mat4 jointMatrix = jointNode->getMatrix() * skin->inverseBindMatrices[i];
					jointMatrix *= inverseTransform;
					mesh->UniformBlock.jointMatrix[i] = jointMatrix;
				}
				mesh->UniformBlock.jointCount = static_cast<float>(numJoints);
				std::shared_ptr<UniformBuffer> buffer = std::make_shared<UniformBuffer>(device, memprops, sizeof(mesh->UniformBlock));
				buffer->setData(&mesh->UniformBlock);
				buffer->copy();
			}
			else {
				std::shared_ptr<UniformBuffer> buffer = std::make_shared<UniformBuffer>(device, memprops, sizeof(glm::mat4));
				buffer->setData(&m);
				buffer->copy();
			}
		}

		for (auto child : children) {
			child->update(device, memprops);
		}
	}

	Node::~Node() {
		if (mesh) {
			mesh->~Mesh();
		}
		for (auto child : children) {
			delete child;
		}
	}

	Node* ModelInfo::findNode(Node *parent, std::string id) {
		Node *ret = nullptr;

		if (parent->id == id) {
			return parent;
		}

		for (Node* child : parent->children) {
			ret = findNode(child, id);
			if (ret) {
				break;
			}
		}

		return ret;
	}

	Node* ModelInfo::nodeFromId(std::string id) {
		Node* ret = nullptr;

		for (Node* node : nodes) {
			ret = findNode(node, id);
			if (ret) {
				break;
			}
		}

		return ret;
	}

	void ModelInfo::loadNode(Node *parent, Microsoft::glTF::Node node, Microsoft::glTF::Document document, std::string nodeId,
		std::vector<uint32_t>& indexBuffer, std::vector<vkglTF::Vertex>& vertexBuffer, float globalScale) {
		Node *newNode = new Node();
		glm::vec3 translation = glm::vec3(0.0f);
		glm::mat4 rotation = glm::mat4(1.0f);
		glm::vec3 scale = glm::vec3(1.0f);

		newNode->id = nodeId;
		newNode->parent = parent;
		newNode->name = node.name;
		newNode->skinId = node.skinId;
		newNode->matrix = glm::mat4(1.0f);

		translation = glm::vec3(node.translation.x, node.translation.y, node.translation.z);
		glm::quat quaternion_rotation = glm::quat(node.rotation.w, node.rotation.x, node.rotation.y, node.rotation.z);
		rotation = glm::mat4(quaternion_rotation);
		scale = glm::vec3(node.scale.x, node.scale.y, node.scale.z);

		if (node.matrix.values.size() == 16) {
			newNode->matrix = glm::mat4(0.0f);
			int nodeMatrixIndex = 0;

			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					newNode->matrix[i][j] = node.matrix.values[nodeMatrixIndex];
				}
			}
		}

		for (std::string childId : node.children) {
			Microsoft::glTF::Node child = document.nodes.Get(childId);
			loadNode(newNode, child, document, childId, indexBuffer, vertexBuffer, globalScale);
		}

		if (node.meshId.length() > 0) {
			Microsoft::glTF::Mesh microsoftMesh = document.meshes.Get(node.meshId);
			Mesh *mesh = new Mesh(device, memprops, newNode->matrix);

			for (Microsoft::glTF::MeshPrimitive primitive : microsoftMesh.primitives) {
				if (primitive.indicesAccessorId.length() <= 0) {
					continue;
				}

				uint32_t indexStart = static_cast<uint32_t>(indexBuffer.size());
				uint32_t vertexStart = static_cast<uint32_t>(vertexBuffer.size());
				uint32_t indexCount = 0;
				glm::vec3 posMin{};
				glm::vec3 posMax{};
				bool hasSkin = false;

				//This segment reads vertices from buffer
				{
					size_t posAccessorCount = 0;
					BufferInfo<float> bufferPos;
					BufferInfo<float> bufferNormals;
					BufferInfo<float> bufferTexCoordSets0;
					BufferInfo<float> bufferTexCoordSets1;
					BufferInfo<uint16_t> bufferJoints;
					BufferInfo<uint8_t> bufferWeights;

					const auto bufposval = primitive.attributes.find("POSITION");
					const auto normval = primitive.attributes.find("NORMAL");
					const auto tex0 = primitive.attributes.find("TEXCOORD_0");
					const auto tex1 = primitive.attributes.find("TEXCOORD_1");
					const auto jointsval = primitive.attributes.find("JOINTS_0");
					const auto weightsval = primitive.attributes.find("WEIGHTS_0");
					const auto end = primitive.attributes.end();

					bufferPos = bufposval != end ? getFullBufferData<float>(document, bufposval->second, this->modelPath) : BufferInfo<float>();
					bufferNormals = normval != end ? getFullBufferData<float>(document, normval->second, this->modelPath) : BufferInfo<float>();
					bufferTexCoordSets0 = tex0 != end ? getFullBufferData<float>(document, tex0->second, this->modelPath) : BufferInfo<float>();
					bufferTexCoordSets1 = tex1 != end ? getFullBufferData<float>(document, tex1->second, this->modelPath) : BufferInfo<float>();
					bufferJoints = jointsval != end ? getFullBufferData<uint16_t>(document, jointsval->second, this->modelPath) : BufferInfo<uint16_t>();
					bufferWeights = weightsval != end ? getFullBufferData<uint8_t>(document, weightsval->second, this->modelPath) : BufferInfo<uint8_t>();

					posMin = glm::vec3(bufferPos.buffersAccessors.accessor.min[0], bufferPos.buffersAccessors.accessor.min[1], bufferPos.buffersAccessors.accessor.min[2]);
					posMax = glm::vec3(bufferPos.buffersAccessors.accessor.max[0], bufferPos.buffersAccessors.accessor.max[1], bufferPos.buffersAccessors.accessor.max[2]);

					hasSkin = bufferJoints.bufferData.size() > 0 && bufferWeights.bufferData.size() > 0;

					for (size_t v = 0; v < bufferPos.buffersAccessors.accessor.count; v++) {
						vkglTF::Vertex vert{};
						vert.position = glm::vec4(glm::make_vec3(&bufferPos.bufferData[v * 3]), 1.0f);

						if (bufferNormals.bufferData.size() > 0) {
							glm::vec3 vectToNormalize = glm::make_vec3(&bufferNormals.bufferData[v * 3]);
							vert.normal = glm::normalize(vectToNormalize);
						}
						else {
							vert.normal = glm::vec3(0.0f);
						}

						if (bufferTexCoordSets0.bufferData.size() > 0) {
							vert.uv0 = glm::make_vec2(&bufferTexCoordSets0.bufferData[v * 2]);
						}
						else {
							vert.uv0 = glm::vec3(0.0f);
						}

						if (bufferTexCoordSets1.bufferData.size() > 0) {
							vert.uv1 = glm::make_vec2(&bufferTexCoordSets1.bufferData[v * 2]);
						}
						else {
							vert.uv1 = glm::vec3(0.0f);
						}

						if (hasSkin) {
							vert.joint0 = glm::vec4(glm::make_vec4(&bufferJoints.bufferData[v * 4]));
						}
						else {
							vert.joint0 = glm::vec4(0.0f);
						}

						if (bufferWeights.bufferData.size() > 0) {
							vert.weight0 = glm::make_vec4(&bufferWeights.bufferData[v * 4]);
						}
						else {
							vert.weight0 = glm::vec4(0.0f);
						}

						vertexBuffer.push_back(vert);
					}
				}

				//This segment reads indices from buffer
				{
					Microsoft::glTF::Accessor accessor = document.accessors.Get(primitive.indicesAccessorId);
					uint32_t indexCount = static_cast<uint32_t>(accessor.count);

					switch (accessor.componentType) {
					case Microsoft::glTF::ComponentType::COMPONENT_UNSIGNED_INT: {
						BufferInfo<uint32_t> indicesBufferInfo = getFullBufferData<uint32_t>(document, primitive.indicesAccessorId, this->modelPath);

						for (uint32_t index = 0; index < indexCount; index++) {
							indexBuffer.push_back(indicesBufferInfo.bufferData[index] + vertexStart);
						}

						break;
					}
					case Microsoft::glTF::ComponentType::COMPONENT_UNSIGNED_SHORT: {
						BufferInfo<uint16_t> indicesBufferInfo = getFullBufferData<uint16_t>(document, primitive.indicesAccessorId, this->modelPath);

						for (auto index = 0; index < indexCount; index++) {
							indexBuffer.push_back(indicesBufferInfo.bufferData[index] + vertexStart);
						}

						break;
					}
					case Microsoft::glTF::ComponentType::COMPONENT_UNSIGNED_BYTE: {
						BufferInfo<uint8_t> indicesBufferInfo = getFullBufferData<uint8_t>(document, primitive.indicesAccessorId, this->modelPath);

						for (uint32_t index = 0; index < indexCount; index++) {
							indexBuffer.push_back(indicesBufferInfo.bufferData[index] + vertexStart);
						}

						break;
					}
					default: {
						std::cerr << "Component type not supported. Type is: " + accessor.componentType << std::endl;
						return;
					}
					};
				}

				Primitive *newPrimitive = nullptr;

				if (primitive.materialId.length() > 0) {
					for (Material material : materials) {
						if (material.materialId._Equal(primitive.materialId)) {
							newPrimitive = new Primitive(indexStart, indexCount, material);
							break;
						}
					}
				}
				else {
					newPrimitive = new Primitive(indexStart, indexCount, materials.back());
				}

				newPrimitive->setBoundingBox(posMin, posMax);
				mesh->primitives.push_back(newPrimitive);
			}

			//Bounding Box za Mesh izvucen iz BB primitiva
			for (Primitive *p : mesh->primitives) {
				if (p->boundingBox.valid && !mesh->bb.valid) {
					mesh->bb = p->boundingBox;
					mesh->bb.valid = true;
				}

				mesh->bb.min = glm::min(mesh->bb.min, p->boundingBox.min);
				mesh->bb.max = glm::max(mesh->bb.max, p->boundingBox.max);
			}

			newNode->mesh = mesh;
		}

		if (parent) {
			parent->children.push_back(newNode);
		}
		else {
			nodes.push_back(newNode);
		}

		linearNodes.push_back(newNode);
	}

	void ModelInfo::loadSkins(Microsoft::glTF::Document &document) {
		for (Microsoft::glTF::Skin skin : document.skins.Elements()) {
			std::unique_ptr<vkglTF::Skin> newSkin = std::make_unique<vkglTF::Skin>();
			newSkin->name = skin.name;

			if (skin.skeletonId.length() > 0) {
				newSkin->skeletonRoot = nodeFromId(skin.skeletonId);
			}

			for (std::string jointId : skin.jointIds) {
				Node *node = nodeFromId(jointId);
				if (node) {
					newSkin->joints.push_back(node);
				}
			}

			if (skin.inverseBindMatricesAccessorId.length() > 0) {
				BufferInfo<float> bufferInfo = getFullBufferData<float>(document, skin.inverseBindMatricesAccessorId, this->modelPath);
				newSkin->inverseBindMatrices.resize(bufferInfo.buffersAccessors.accessor.count);
				memcpy(newSkin->inverseBindMatrices.data(), bufferInfo.bufferData.data(), bufferInfo.buffersAccessors.accessor.count * sizeof(glm::mat4));
			}

			skins.push_back(newSkin.get());
		}
	}

	void ModelInfo::loadTextures(Microsoft::glTF::Document document, std::string folderPath) {
		for (Microsoft::glTF::Texture texture : document.textures.Elements()) {
			Microsoft::glTF::Image image = document.images[texture.imageId];
			vkglTF::TextureSampler textureSampler;

			for (vkglTF::TextureSampler sampler : textureSamplers) {
				if (sampler.id._Equal(texture.samplerId)) {
					textureSampler = sampler;
					break;
				}
			}

			textures.push_back(vkglTF::readTextureFromglTFImage(image, document, folderPath, texture, textureSampler,
				device, memprops, transferQueue));
		}
	}

	void ModelInfo::loadTextureSamplers(Microsoft::glTF::Document document) {
		for (Microsoft::glTF::Sampler sampler : document.samplers.Elements()) {
			TextureSampler texSampler{};

			texSampler.id = sampler.id;
			texSampler.magFilter = getVkFilterMode(sampler.magFilter);
			texSampler.minFilter = getVkFilterMode(sampler.minFilter);
			texSampler.U = getVkWrapMode(sampler.wrapS);
			texSampler.V = getVkWrapMode(sampler.wrapT);
			texSampler.W = VK_SAMPLER_ADDRESS_MODE_REPEAT;

			textureSamplers.push_back(texSampler);
		}
	}

	void ModelInfo::loadMaterials(Microsoft::glTF::Document document) {
		for (Microsoft::glTF::Material material : document.materials.Elements()) {
			vkglTF::Material customMaterial{};
			Microsoft::glTF::Material::PBRMetallicRoughness metallicRoughnessInfo = material.metallicRoughness;
			Microsoft::glTF::Color4 baseColorFactor = metallicRoughnessInfo.baseColorFactor;
			Microsoft::glTF::Color4 emissiveFactor = material.emissiveFactor.AsColor4();

			customMaterial.alphaCutoff = material.alphaCutoff;
			customMaterial.materialId = material.id;
			customMaterial.baseColorFactor = glm::vec4(baseColorFactor.r, baseColorFactor.g, baseColorFactor.b, baseColorFactor.a); //RGBA
			customMaterial.emissiveFactor = glm::vec4(emissiveFactor.r, emissiveFactor.g, emissiveFactor.b, emissiveFactor.a);		//RGBA
			customMaterial.metallicFactor = metallicRoughnessInfo.metallicFactor;
			customMaterial.roughnessFactor = metallicRoughnessInfo.roughnessFactor;

			switch (material.alphaMode) {
				case Microsoft::glTF::AlphaMode::ALPHA_BLEND: customMaterial.alphaMode = vkglTF::Material::AlphaMode::ALPHA_BLEND; break;
				case Microsoft::glTF::AlphaMode::ALPHA_MASK: customMaterial.alphaMode = vkglTF::Material::AlphaMode::ALPHA_MASK; break;
				case Microsoft::glTF::AlphaMode::ALPHA_OPAQUE: customMaterial.alphaMode = vkglTF::Material::AlphaMode::ALPHA_OPAQUE; break;
				case Microsoft::glTF::AlphaMode::ALPHA_UNKNOWN: customMaterial.alphaMode = vkglTF::Material::AlphaMode::ALPHA_OPAQUE; break;
				default: assert(0 && "Alpha mode unrecognized."); break;
			}

			//Read textures
			for (auto textureInfo : material.GetTextures()) {
				switch (textureInfo.second) {
				case Microsoft::glTF::TextureType::BaseColor: {
					for (Texture* tex : textures) {
						if (tex->getTextureId()._Equal(textureInfo.first) || tex->getTextureType() == TextureType::BASE_COLOR) {
							customMaterial.baseColorTexture = tex;
							customMaterial.TexCoordinateSets.baseColor = static_cast<uint8_t>(metallicRoughnessInfo.baseColorTexture.texCoord);
						}
					}
					break;
				}
				case Microsoft::glTF::TextureType::Emissive: {
					for (Texture* tex : textures) {
						if (tex->getTextureId()._Equal(textureInfo.first) || tex->getTextureType() == TextureType::EMISSIVE) {
							customMaterial.emissiveTexture = tex;
							customMaterial.TexCoordinateSets.emmisive = static_cast<uint8_t>(material.emissiveTexture.texCoord);
						}
					}
					break;
				}
				case Microsoft::glTF::TextureType::MetallicRoughness: {
					for (Texture* tex : textures) {
						if (tex->getTextureId()._Equal(textureInfo.first) || tex->getTextureType() == TextureType::METALLIC_ROUGHNESS) {
							customMaterial.metallicRoughnessTexture = tex;
							customMaterial.TexCoordinateSets.metallicRoughness = static_cast<uint8_t>(metallicRoughnessInfo.metallicRoughnessTexture.texCoord);
						}
					}
					break;
				}

				case Microsoft::glTF::TextureType::Normal: {
					for (Texture* tex : textures) {
						if (tex->getTextureId()._Equal(textureInfo.first) || tex->getTextureType() == TextureType::NORMAL) {
							customMaterial.normalTexture = tex;
							customMaterial.TexCoordinateSets.normal = static_cast<uint8_t>(material.normalTexture.texCoord);
						}
					}
					break;
				}

				case Microsoft::glTF::TextureType::Occlusion: {
					for (Texture* tex : textures) {
						if (tex->getTextureId()._Equal(textureInfo.first) || tex->getTextureType() == TextureType::OCCLUSION) {
							customMaterial.oclussionTexture = tex;
							customMaterial.TexCoordinateSets.occlusion = static_cast<uint8_t>(material.occlusionTexture.texCoord);
						}
					}
					break;
				}
				}
			}

			std::cout << "Material extensions details" << std::endl;
			for (std::pair<std::string, std::string> extensionPair : material.extensions) {
				std::cout << "< Key: " + extensionPair.first << ", Value: " + extensionPair.second;
			}

			materials.push_back(customMaterial);
		}
	}

	void ModelInfo::loadAnimations(Microsoft::glTF::Document document) {
		for (Microsoft::glTF::Animation anim : document.animations.Elements()) {
			vkglTF::Animation animation{};

			animation.name = anim.name;

			for (Microsoft::glTF::AnimationSampler samp : anim.samplers.Elements()) {
				vkglTF::AnimationSampler sampler{};

				switch (samp.interpolation) {
				case Microsoft::glTF::InterpolationType::INTERPOLATION_LINEAR: sampler.interpolation = vkglTF::AnimationSampler::Interpolation::LINEAR; break;
				case Microsoft::glTF::InterpolationType::INTERPOLATION_STEP: sampler.interpolation = vkglTF::AnimationSampler::Interpolation::STEP; break;
				case Microsoft::glTF::InterpolationType::INTERPOLATION_CUBICSPLINE: sampler.interpolation = vkglTF::AnimationSampler::Interpolation::CUBICSPLINE; break;
				case Microsoft::glTF::InterpolationType::INTERPOLATION_UNKNOWN: sampler.interpolation = vkglTF::AnimationSampler::Interpolation::LINEAR; break;
				default: assert(0 && "Cant convert this animation type.");
				}

				//Read inputs
				{
					vkglTF::BuffersAccessors inputs = getBufferAccessorFromDocument(document, samp.inputAccessorId);
					BufferInfo<float> inputBuffer;

					if (inputs.accessor.componentType == Microsoft::glTF::ComponentType::COMPONENT_FLOAT) {
						inputBuffer = getFullBufferData<float>(document, samp.inputAccessorId, this->modelPath);
						sampler.inputs = inputBuffer.bufferData;

						//Where is our animations start
						for (float input : sampler.inputs) {
							if (input < animation.start) {
								animation.start = input;
							}
							if (input > animation.end) {
								animation.end = input;
							}
						}
					}
					else {
						assert(0 && "Unsupported data type.");
					}
				}

				//Read outputs
				{
					vkglTF::BuffersAccessors outputs = getBufferAccessorFromDocument(document, samp.outputAccessorId);

					if (outputs.accessor.componentType == Microsoft::glTF::ComponentType::COMPONENT_FLOAT) {
						switch (outputs.accessor.type) {
							case Microsoft::glTF::AccessorType::TYPE_VEC3: {
								BufferInfo<glm::vec3> outputBuffer = getFullBufferData<glm::vec3>(document, samp.outputAccessorId, this->modelPath);

								for (auto vec : outputBuffer.bufferData) {
									glm::vec4 vectorToAdd = glm::vec4(vec[0], vec[1], vec[2], 1.0f);
									sampler.outputs.push_back(vectorToAdd);
								}

								break;
							}
							case Microsoft::glTF::AccessorType::TYPE_VEC4: {
								BufferInfo<glm::vec4> outputBuffer = getFullBufferData<glm::vec4>(document, samp.outputAccessorId, this->modelPath);

								for (auto vec : outputBuffer.bufferData) {
									sampler.outputs.push_back(vec);
								}

								break;
							}
							case Microsoft::glTF::AccessorType::TYPE_SCALAR :{
								BufferInfo<float> outputBuffer = getFullBufferData<float>(document, samp.outputAccessorId, this->modelPath);

								for (auto scalar : outputBuffer.bufferData) {
									sampler.weights.push_back(scalar);
								}
							}
						}
					}
					else {
						std::cout << "Unsupported type: " + outputs.accessor.componentType << std::endl;
					}
				}

				animation.samplers.push_back(sampler);
			}

			for (Microsoft::glTF::AnimationChannel chan : anim.channels.Elements()) {
				AnimationChannel channel{};

				if (chan.target.path == Microsoft::glTF::TargetPath::TARGET_ROTATION) {
					channel.path = AnimationChannel::PathType::ROTATION;
				}
				if (chan.target.path == Microsoft::glTF::TargetPath::TARGET_SCALE) {
					channel.path = AnimationChannel::PathType::SCALE;
				}
				if (chan.target.path == Microsoft::glTF::TargetPath::TARGET_TRANSLATION) {
					channel.path = AnimationChannel::PathType::TRANSLATION;
				}
				if (chan.target.path == Microsoft::glTF::TargetPath::TARGET_WEIGHTS) {
					channel.path = AnimationChannel::PathType::WEIGHTS;
				}

				channel.samplerId = chan.samplerId;
				channel.node = nodeFromId(chan.target.nodeId);

				if (!channel.node) {
					continue;
				}

				animation.channels.push_back(channel);
			}

			animations.push_back(animation);
		}
	}

	void ModelInfo::loadFromDocument(Microsoft::glTF::Document* document, std::string modelFolder,
		VkDevice* device, VkPhysicalDeviceMemoryProperties* memprops,
		VkQueue* queue, VkCommandPool cmdPool, float globalScale) {
		this->device = *device;
		this->memprops = *memprops;
		this->transferQueue = *queue;
		this->cmdPool = cmdPool;
		this->modelPath = modelFolder;

		std::vector<uint32_t> indices;
		std::vector<vkglTF::Vertex> vertices;

		if (document != nullptr) {
			loadTextureSamplers(*document);
			loadTextures(*document, modelFolder);
			loadMaterials(*document);

			Microsoft::glTF::Scene scene = document->scenes.Elements()[0];
			int documentNodesIndex = 0;

			for (std::string sceneNode : scene.nodes) {
				for (Microsoft::glTF::Node node : document->nodes.Elements()) {
					if (node.id._Equal(sceneNode)) {
						loadNode(nullptr, node, *document, node.id, indices, vertices, globalScale);
					}
				}
			}

			if (document->animations.Size() > 0) {
				loadAnimations(*document);
			}

			loadSkins(*document);

			for (auto node : linearNodes) {
				if (node->skinId.length() > 0) {
					for (vkglTF::Skin *skin : skins) {
						if (skin->name._Equal(node->skinId)) {
							node->skin = skin;
						}
					}
				}

				if (node->mesh) {
					node->update(*device, *memprops);
				}
			}

			for (std::string extension : document->extensionsUsed) {
				extensions.push_back(extension);
			}

			size_t vertexBufferSize = vertices.size() * sizeof(vkglTF::Vertex);
			size_t indexBufferSize = indices.size() * sizeof(uint32_t);
			Indices.count = static_cast<uint32_t>(indexBufferSize);

			assert((vertexBufferSize > 0 && indexBufferSize > 0) && "Vertex and Index buffers are empty.");

			vertexData = new StagingBuffer<vkglTF::Vertex>(*device, *memprops, vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
			indexData = new StagingBuffer<uint32_t>(*device, *memprops, indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
			vertexData->fillBuffer(vertices);
			indexData->fillBuffer(indices);

			vertexBuffer = new VertexBuffer<vkglTF::Vertex>(*device, *memprops, vertexBufferSize);
			indexBuffer = new IndexBuffer(*device, *memprops, indexBufferSize);

			CommandBufferHandler::copyBuffer(vertexData->getBuffer(), vertexBuffer->getBuffer(),
				vertexBufferSize, *queue, cmdPool, *device);
			CommandBufferHandler::copyBuffer(indexData->getBuffer(), indexBuffer->getBuffer(),
				indexBufferSize, *queue, cmdPool, *device);

			getSceneDimensions();
		}
		else {
			assert(0 && "Invalid document ref.");
		}
	}

	VkSamplerAddressMode ModelInfo::getVkWrapMode(int32_t wrapMode) {
		switch (wrapMode) {
		case 10497: return VK_SAMPLER_ADDRESS_MODE_REPEAT;
		case 33071: return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		case 33648: return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
		default: return VK_SAMPLER_ADDRESS_MODE_REPEAT;
		}
	}

	VkFilter ModelInfo::getVkFilterMode(int32_t filterMode)
	{
		switch (filterMode) {
		case 9728: return VK_FILTER_NEAREST;
		case 9729: return VK_FILTER_LINEAR;
		case 9984: return VK_FILTER_NEAREST;
		case 9985: return VK_FILTER_NEAREST;
		case 9986: return VK_FILTER_LINEAR;
		case 9987: return VK_FILTER_LINEAR;
		default: return VK_FILTER_LINEAR;
		}
	}

	void ModelInfo::drawNode(Node *node, VkCommandBuffer cmdBuffer) {
		if (node->mesh) {
			for (Primitive *p : node->mesh->primitives) {
				vkCmdDrawIndexed(cmdBuffer, p->indexCount, 1, p->firstIndex, 0, 0);
			}
		}

		for (Node *child : node->children) {
			drawNode(child, cmdBuffer);
		}
	}

	void ModelInfo::draw(VkCommandBuffer cmdBuffer) {
		const VkDeviceSize offsets[1] = { 0 };
		vkCmdBindVertexBuffers(cmdBuffer, 0, 1, &Vertices.buffer, offsets);
		vkCmdBindIndexBuffer(cmdBuffer, Indices.buffer, 0, VK_INDEX_TYPE_UINT32);

		for (Node *node : nodes) {
			drawNode(node, cmdBuffer);
		}
	}

	void ModelInfo::calculateBoundingBox(Node *node, Node *parent) {
		BoundingBox parentBBox = parent ? parent->bb : BoundingBox(Dimensions.min, Dimensions.max);

		if (node->mesh) {
			if (node->mesh->bb.valid) {
				node->aabb = node->mesh->bb.getBoundingBoxData(node->getMatrix());

				if (node->children.size() > 0) {
					node->bb.min = node->aabb.min;
					node->bb.max = node->aabb.max;
					node->bb.valid = true;
				}
			}
		}

		parentBBox.min = glm::min(parentBBox.min, node->bb.min);
		parentBBox.max = glm::max(parentBBox.max, node->bb.max);

		for (Node *child : node->children) {
			calculateBoundingBox(child, node);
		}
	}

	void ModelInfo::getSceneDimensions() {
		for (Node *sceneNode : linearNodes) {
			calculateBoundingBox(sceneNode, sceneNode->parent);
		}

		Dimensions.min = glm::vec3(FLT_MAX);
		Dimensions.max = glm::vec3(-FLT_MAX);

		for (Node *sceneNode : linearNodes) {
			if (sceneNode->bb.valid)
			{
				Dimensions.min = glm::min(Dimensions.min, sceneNode->bb.min);
				Dimensions.max = glm::min(Dimensions.min, sceneNode->bb.min);
			}
		}

		aabb = glm::scale(glm::mat4(1.0f), glm::vec3(Dimensions.max[0] - Dimensions.min[0], Dimensions.max[1] - Dimensions.min[1], Dimensions.max[2] - Dimensions.min[2]));
		aabb[3][0] = Dimensions.min[0];
		aabb[3][1] = Dimensions.min[1];
		aabb[3][2] = Dimensions.min[2];
	}



	void ModelInfo::destroy() {
		if (Vertices.buffer != VK_NULL_HANDLE) {
			vkDestroyBuffer(device, Vertices.buffer, nullptr);
			vkFreeMemory(device, Vertices.memory, nullptr);
		}

		if (Indices.buffer != VK_NULL_HANDLE) {
			vkDestroyBuffer(device, Indices.buffer, nullptr);
			vkFreeMemory(device, Indices.memory, nullptr);
		}

		for (Texture* tex : textures) {
			tex->~Texture();
		}

		for (Node *node : nodes) {
			node->~Node();
		}

		for (Node *node : linearNodes) {
			node->~Node();
		}

		nodes.clear();
		linearNodes.clear();
		skins.clear();
		textures.clear();
		textureSamplers.clear();
		materials.clear();
		animations.clear();
		extensions.clear();
	}

	template struct BufferInfo<float>;
	template struct BufferInfo<glm::vec3>;
	template struct BufferInfo<glm::vec4>;
	template struct BufferInfo<uint8_t>;
	template struct BufferInfo<uint16_t>;
	template struct BufferInfo<uint32_t>;
	template struct BufferInfo<size_t>;
	template struct BufferInfo<float>;
};