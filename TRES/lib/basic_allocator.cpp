#include "basic_allocator.hpp"

namespace tres {

void* basic_allocator::allocate(size_t size) {
	if (allocated_pointers > max_allocated_pointers) {
		return tres::null;
	}
	size_t size_cell = sizeof(Cell);
	Cell* kernel_memory = (Cell*)KERNEL_MEM;
	for (size_t i = 0; i < KERNEL_SIZE; i += 1) {
		Cell allocation_entry = kernel_memory[i];

		if (allocation_entry.pointer == 0) {      // check el puntero
			
			if(allocation_entry.size==0 || allocation_entry.size==size){ // check size.
				// encontrado. poner aquí.

				break;
			}  

		} else if(allocation_entry.size==size) {
			/* code */
		}
		


	}
	return tres::null;
}

void basic_allocator::deallocate(void* what) {
}

void* basic_allocator::reallocate(void* what, size_t new_size) {
}

};  // namespace tres