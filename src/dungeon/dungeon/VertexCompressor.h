#ifndef __vertex_compressor
#define __vertex_compressor
#include "VertexObject.h"
#include "Vertices.h"
#include "common.h"
#include <list>
#include <map>

namespace engine{
	template < class T > class VertexObject;
	template < class T = engine::simple_vertex >
	class element {
	public:
		element(T *input){
			this->data = input;
			parent = _n;
		}
		element(engine::VertexObject < T > *input, _ id){
			this->data = _n;
			this->parent = input;
			this->id = id;
		}
		T* retreive() const{
			if(this->data != _n){
				return this->data;
			} else {
				return this->parent->getLastModelVertex(this->id);
			}
		}
		friend bool operator< (const element &a, const element &b){
			return T::less_than(a.retreive(), b.retreive());
		}
		friend bool operator> (const element &a, const element &b){
			return T::greater_than(a.retreive(), b.retreive());
		}
		friend bool operator== (const element &a, const element &b){
			return T::equal(a.retreive(), b.retreive());
		}
	private:
		T *data;
		engine::VertexObject < T > *parent;
		_ id;
	};
	template < class T = engine::simple_vertex >
	class VertexCompressor{
	public:
		void insert_vertex(VertexObject < T > *input, _ id){
			_ t = this->ids.size();
			this->ids[t] = id;
			this->elements[element < T > (input, id)] = t;
		}
		_ getId(T *input){
			return this->ids[this->elements[element < T > (input)]];
		}
		bool exists(T *input){
			return this->elements.count(element < T > (input)) != 0;
		}
		void clear(){
			this->elements.clear();
			this->ids.clear();
		}
	protected:
		/*
			May be in future I will use boost, but I can't figure out why this code doesn't work correctly at this moment because of lack of time.
		*/
		/*
			class element : public avl_set_base_hook< > {
			public:
				avl_set_member_hook<> member_hook_;
				element(simple_vertex *input){
				this->data = input;
				}
				element(simple_vertex *input, _ id){
				this->data = input;
				this->id = id;
				}
				friend bool operator< (const element &a, const element &b){
				return simple_vertex::less_than(a.data, b.data);
				}
				friend bool operator> (const element &a, const element &b){
				return simple_vertex::greater_than(a.data, b.data);
				}
				friend bool operator== (const element &a, const element &b){
				return simple_vertex::equal(a.data, b.data);
				}
				_ getId(){
				return this->id;
				}
			private:
				simple_vertex *data;
				_ id;
			};
			typedef avl_set < VertexCompressor::element, compare<std::greater<VertexCompressor::element>> > tree;
			std::list < VertexCompressor::element* > elements;
			tree data;
		*/

		std::map < element < T >, _ > elements;
		std::map < _ , _ > ids;
	};
}
#endif