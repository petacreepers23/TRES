#pragma once
#include "types.hpp"
#include "memory.hpp"

namespace tres {

	//No segmented nor paginated mode allocator
	class basic_allocator {
		//TODO: with tres::string store an id of a pointer
		//so it can be shared between pointers.
		//TODO: protect from exceeding the pretended size,
		//so it doesn't override other data.
		//TOOD: fix accesible after released for security propuses.

	public:
		//handles a new pointer, can return null
		void* allocate(size_t size);
		//releases a pointer
		void deallocate(void* what);
		//handles a change of size
		void* reallocate(void* what, size_t new_size);

	private:
		struct Cell {
			void* pointer = tres::null;
			size_t size = 0;
		};
		//structure of kernel memory:
			//----: pointer
			//----: size of it
		//one for each address and some margin
		//const max_allocated_pointers = (tres::KERNEL_MEM / sizeof(Cell)) - sizeof(Cell);

		//size_t allocated_pointers = 0;

		//void* find_a_suitable_place(size_t s);
	};

};