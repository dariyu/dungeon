#include "common.h"
#include "Artist.h"
#include "Effect.h"
#include <string>
#include "d3d10_1.h"
#include "d3dx10.h"
#include <map>

namespace engine{
	void Effect::loadFromFile(std::wstring input){
		HRESULT result = S_OK;
		ID3D10Blob* errors = _n;
		result = D3DX10CreateEffectFromFile(input.c_str(), _n, _n, 
									"fx_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 
									0, this->parent->getDevice(), NULL, NULL, &this->effect, &errors, &result);
		if(FAILED(result)){
			if(errors != _n){
				MessageBoxA(0, (char*)errors->GetBufferPointer(), 0, 0);
			}
			return;
		}
	}
	D3D10_INPUT_ELEMENT_DESC Effect::create_input_layout_element(	const char *semantic_name, _ semantic_index, DXGI_FORMAT format,
												_ input_slot, _ aligned_byte_offset, D3D10_INPUT_CLASSIFICATION input_slot_class,
												_ instance_data_step_rate){
		D3D10_INPUT_ELEMENT_DESC result;
		result.SemanticName = semantic_name;
		result.SemanticIndex = semantic_index;
		result.Format = format;
		result.InputSlot = input_slot;
		result.AlignedByteOffset = aligned_byte_offset;
		result.InputSlotClass = input_slot_class;
		result.InstanceDataStepRate = instance_data_step_rate;
		return result;
	}
	_ Effect::prepareTechnique(const char *name, InputLayout layout){
		if(this->tech_hash.count(std::string(name))){
			return this->tech_hash[std::string(name)];
		}
		HRESULT checker = S_OK;
		_ last = this->techniques.size();
		_ curr = 0;
		_uint numElements = 0;
		_technique current_technique = this->effect->GetTechniqueByName(name);
		_input_layout current_layout = _n;
		D3D10_INPUT_ELEMENT_DESC elementDesc[5];
		switch(layout){
			case PostEffectLayout:
				elementDesc[curr++] = create_input_layout_element("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,  0, D3D10_INPUT_PER_VERTEX_DATA, 0);
				elementDesc[curr++] = create_input_layout_element("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,	 0, 16, D3D10_INPUT_PER_VERTEX_DATA, 0);
				break;
			case NormalLayout:
				elementDesc[curr++] = create_input_layout_element("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D10_INPUT_PER_VERTEX_DATA, 0);
				elementDesc[curr++] = create_input_layout_element("NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT,  0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0);
				break;
			case TexturedLayout:
				elementDesc[curr++] = create_input_layout_element("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D10_INPUT_PER_VERTEX_DATA, 0);
				elementDesc[curr++] = create_input_layout_element("NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT,  0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0);
				elementDesc[curr++] = create_input_layout_element("TEXTURECOORD", 0, DXGI_FORMAT_R32G32_FLOAT,  0, 24, D3D10_INPUT_PER_VERTEX_DATA, 0);
				break;
			case ColorfulLayout:
				elementDesc[curr++] = create_input_layout_element("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D10_INPUT_PER_VERTEX_DATA, 0);
				elementDesc[curr++] = create_input_layout_element("COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT,  0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0);
				elementDesc[curr++] = create_input_layout_element("NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT,  0, 28, D3D10_INPUT_PER_VERTEX_DATA, 0);
				break;
			case BillboardLayout:
				elementDesc[curr++] = create_input_layout_element("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D10_INPUT_PER_VERTEX_DATA, 0);
				elementDesc[curr++] = create_input_layout_element("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  12, D3D10_INPUT_PER_VERTEX_DATA, 0);
				elementDesc[curr++] = create_input_layout_element("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,  0, 24, D3D10_INPUT_PER_VERTEX_DATA, 0);
				elementDesc[curr++] = create_input_layout_element("PSIZE", 0, DXGI_FORMAT_R32_FLOAT,  0, 40, D3D10_INPUT_PER_VERTEX_DATA, 0);
				elementDesc[curr++] = create_input_layout_element("BLENDINDICES", 0, DXGI_FORMAT_R8_UINT,  0, 44, D3D10_INPUT_PER_VERTEX_DATA, 0);
				break;
		}
		numElements = curr;
		D3D10_PASS_DESC PassDesc;
		current_technique->GetPassByIndex(0)->GetDesc(&PassDesc);
		checker = this->parent->getDevice()->CreateInputLayout(	elementDesc, numElements, PassDesc.pIAInputSignature, 
																PassDesc.IAInputSignatureSize, &current_layout);
		if(FAILED(checker))
			return 0;
		this->techniques[last] = Technique(current_technique, current_layout);
		this->tech_hash[std::string(name)] = last;
		return last;
	}
	void Effect::activateTechniqueLayout(_ index){
		this->parent->getDevice()->IASetInputLayout(this->techniques[index].input_layout);
	}
	Effect::Effect(Artist *parent){
		this->parent = parent;
		this->effect = _n;
	}
	void Effect::release(){
		if(this->effect != _n){
			this->effect->Release();
		}
	}
	void Effect::applyTechnique(_ index){
		this->techniques[index].technique->GetPassByIndex(0)->Apply(0);
		/*
			D3D10_TECHNIQUE_DESC techDesc;
			this->techniques[index].technique->GetDesc(&techDesc);
			for(_ p = 0; p < techDesc.Passes; ++p){
				this->techniques[index].technique->GetPassByIndex(p)->Apply(0);
			}
		*/
	}
	void Effect::applyTechniquePass(_ index, _ pass){
		this->techniques[index].technique->GetPassByIndex((UINT)pass)->Apply(0);
	}
	_effect Effect::getEffect(){
		return this->effect;
	}
	_effect_variable Effect::getVariable(const char *name) {
		return this->effect->GetVariableByName(name);
	}
	Effect::Effect(){
		this->parent = _n;
		this->effect = _n;
	}
}