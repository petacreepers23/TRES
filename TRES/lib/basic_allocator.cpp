#include "basic_allocator.hpp"
#include "memory.hpp"
namespace tres {

/*
Parametros: size el tamaño en bytes que se desea reservar.
return: Un puntero al inicio de la dir de memoria reservada o NULL en caso de fallo.
Resumen: Reserva memoria del tamaño indicado. 
Recorre el KERNEL_MEMORY que es una estructura de tipo Cell y en caso de que una celda no tenga memoria reservada o ya haya sido liberada.
en cuyo caso si nuestra peticion coincide con su size también nso vale.
Si termina las cells y no encuentra devuelve null.
*/
void* basic_allocator::allocate(size_t size) {
	Cell* kernel_memory = (Cell*)KERNEL_MEM;
	byte* heap_memory_pointer = (byte*)HEAP_MEM;

	for (size_t i = 0; i < KERNEL_SIZE; i += 1) {
		Cell allocation_entry = kernel_memory[i];

		if (allocation_entry.pointer == 0) {  // check el puntero

			if (allocation_entry.size == 0 || allocation_entry.size == size) {  // check size.
				allocation_entry.pointer = heap_memory_pointer;
				allocation_entry.size = size;
				kernel_memory[i] = allocation_entry;
				return allocation_entry.pointer;
			}
		}
		heap_memory_pointer += allocation_entry.size;
	}
	return nullptr;
}

void basic_allocator::deallocate(void* what) {
	Cell* kernel_memory = (Cell*)KERNEL_MEM;

	for (size_t i = 0; i < KERNEL_SIZE; i++) {
		//Cell allocation_entry = kernel_memory[i];
		if (kernel_memory[i].pointer == (byte*)what) {
			kernel_memory[i].pointer = 0;
			//kernel_memory[i] = allocation_entry;
			return;
		}
	}
}

void* basic_allocator::reallocate(void* what, size_t new_size, bool& successfull) {
	//Para reallocar, 1 necesitamos nuevo hueco donde meterlo
	void* newplace = allocate(new_size);
	if (newplace == nullptr) {
		successfull = false;
		return what;
	}

	//2 Necesitamos encontrar el tamaño de los datos de antes
	Cell* kernel_memory = (Cell*)KERNEL_MEM;
	Cell allocation_entry;
	for (size_t i = 0; i < KERNEL_SIZE; i += 1) {
		if (allocation_entry.pointer == what) {
			allocation_entry.pointer = nullptr;
			kernel_memory[i] = allocation_entry;
			break;
		}
	}

	//3 Copiamos la memoria
	for (size_t i = 0; i < allocation_entry.size; i++) {
		static_cast<byte*>(newplace)[i] = allocation_entry.pointer[i];
	}

	//4 liberamos la antigua
	deallocate(what);

	//5 salimos
	return newplace;
}



};  // namespace tres

void* operator new(tres::size_t size) {
	tres::basic_allocator a = tres::basic_allocator();
	return a.allocate(size);
}
void* operator new[](tres::size_t size) {
	tres::basic_allocator a = tres::basic_allocator();
	return a.allocate(size);
}

void operator delete (void* ptr,tres::size_t size) noexcept{
	tres::basic_allocator a = tres::basic_allocator();
	return a.deallocate(ptr);
	
}
void operator delete[] (void* ptr) noexcept{
	tres::basic_allocator a = tres::basic_allocator();
	return a.deallocate(ptr);
	
}