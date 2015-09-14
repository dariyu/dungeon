#ifndef __effect
#define __effect
#include "Artist.h"
#include <string>

namespace engine {
	class Artist;
	class Effect {
	public:
		enum InputLayout{
			PostEffectLayout,
			NormalLayout,
			TexturedLayout,
			ColorfulLayout,
			BillboardLayout
		};
		Effect(Artist *parent);
		virtual void loadFromFile(std::wstring input);
		void release();
		_effect getEffect();
		_ prepareTechnique(const char *name, InputLayout layout);
		void applyTechnique(_ index);
		void applyTechniquePass(_ index, _ pass);
		void activateTechniqueLayout(_ index);
		/*
			What GetVariableByName method call cost? Is it cheap or expensive?
		*/
		_effect_variable getVariable(const char *name);
	protected:
		struct Technique{
			_technique technique;
			_input_layout input_layout;
			Technique() : technique(_n), input_layout(_n) {}
			Technique(_technique technique, _input_layout input_layout) : technique(technique), input_layout(input_layout) {}
		};
		Effect();
		Artist *parent;
		std::map < _, Technique > techniques;
		std::map < std::string, _ > tech_hash;
		_effect effect;
		D3D10_INPUT_ELEMENT_DESC create_input_layout_element(	const char *semantic_name, _ semantic_index, DXGI_FORMAT format,
																_ input_slot, _ aligned_byte_offset, D3D10_INPUT_CLASSIFICATION input_slot_class,
																_ instance_data_step_rate);
	};
}
#endif