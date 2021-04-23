#include "types.h"

namespace tres {

	template<class Allocator = tres::basic_allocator> class basic_string {
	
	public:
		/* CONSTRUCTOR */
		//default
		basic_string();
		//copy
		basic_string(const basic_string& str);
		//move
		basic_string(const basic_string&& str);
		//from c-basic_string
		basic_string(const char* str);
		//from buffer
		basic_string(const char* str, size_t n);
		//fill
		basic_string(size_t n, char c);

		/* DESTRUCTOR */
		//clears space from stack
		~basic_string();

		/* ELEMENT ACCESS */
		//Asignator operator
		basic_string& operator=(const basic_string& str);
		//returns a modifiable element
		char& operator[](size_t pos);
		//returns a modifiable element
		char& at(size_t pos);
		//returns tops modifiable element
		char& front();
		//returns backs modifiable element
		char& back();
		//returns a pointer to raw data
		char* data();
		//creates a new zero terminated basic_string
		const char* c_str();

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
		//inserts a char if possible (does not change allocated size)
		bool insert(char c);
		//removes and reapila los chars
		void erase(size_t from, size_t until);
		//add chars (does change allocated size)
		void append(char c);
		//add other basic_string (does change allocated size)
		void operator+=(const basic_string& str);
		//compares basic_strings
		int compare(const basic_string& str);
		//check if a subbasic_string or char is in there
		bool contains(const basic_string& str);
		bool contains(char c);
		//returns a subbasic_string
		basic_string substr(size_t from, size_t until);
		//changes allocated size (can reduce size)
		void resize(size_t s);

		/* STATIC FUNCTIONS */
		//concats basic_strings in a new one
		static basic_string operator+(const basic_string& a,const basic_string& b);
		//compares lexicographically if they are the same
		static bool  operator==(const basic_string& a,const basic_string& b);
		//compares lexicographically if they are not the same
		static bool  operator!=(const basic_string& a,const basic_string& b);
		//converts to byte type
		static byte to_byte(const basic_string& basic_string);
		//converts to int type
		static int to_int(const basic_string& basic_string);
		//converts to float type
		static float to_float(const basic_string& basic_string);
		//converts to unsigned int
		static unsigned int to_uint(const basic_string& basic_string);
		//converts from byte type
		static basic_string to_basic_string(byte b);
		//converts from int type
		static basic_string to_basic_string(int i);
		//converts from float type
		static basic_string to_basic_string(float f);
		//converts from unsigned int
		static basic_string to_basic_string(unsigned int i);

	private:
		//TODO
	}

};