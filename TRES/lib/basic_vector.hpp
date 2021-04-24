#pragma once
#include "types.hpp"

namespace tres {

	template<typename T, class Allocator = tres::basic_allocator> class basic_vector {
	
	public:
		/* CONSTRUCTOR */
		//default
		basic_vector();
		//copy
		basic_vector(const basic_vector& vec);
		//move
		basic_vector(const basic_vector&& vec);
		//from buffer
		basic_vector(const T* vec, size_t n);
		//fill
		basic_vector(size_t n, T elem);

		/* DEvalUCTOR */
		//clears space from stack
		~basic_vector();

		/* ELEMENT ACCESS */
		//Asignator operator
		basic_vector& operator=(const basic_vector& vec);
		//returns a modifiable element
		T& operator[](size_t pos);
		//returns a modifiable element
		T& at(size_t pos);
		//returns tops modifiable element
		T& front();
		//returns backs modifiable element
		T& back();
		//returns a pointer to raw data
		T* data();

		/* PROPERTIES */
		//check if it empty
		bool empty() const;
		//returns its size
		size_t size() const;
		//returns "allocated" capacity
		size_t capacity() const;

		/* OPERATIONS */
		//empties its contents
		void clear();
		//inserts a T if possible (does not change allocated size)
		bool insert(T elem);
		//removes and reapila los Ts
		void erase(size_t from, size_t until);
		//add Ts (does change allocated size)
		void push_back(T elem);
		//removes Ts (does not change allocated size)
		T pop_back();
		//check if a subbasic_vector or T is in there
		bool contains(const basic_vector& vec);
		bool contains(T elem);
		//changes allocated size (can reduce size)
		void resize(size_t new_size);

		/* STATIC FUNCTIONS */
		//compares lexicographically if they are the same
		static bool operator==(const basic_vector& a,const basic_vector& b);
		//compares lexicographically if they are not the same
		static bool operator!=(const basic_vector& a,const basic_vector& b);

	private:
		//TODO
	}

};