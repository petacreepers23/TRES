#include "basic_allocator.h"

namespace tres {

	void* Basic_allocator::allocate(size_t size) {

		if (allocated_pointers > max_allocated_pointers) {
			return tres::null;
		}


	}

	void Basic_allocator::deallocate(void* what) {

	}

	void* Basic_allocator::reallocate(void* what, size_t new_size) {

	}

};