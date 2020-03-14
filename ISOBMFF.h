#include "isobmfftype.h"
#include <iostream>
#include <fstream>
//#include <stdint.h>
#include <byteswap.h>

namespace isobmff{

#define RW_BUF_SIZE 1024

	static inline void print32char(uint32_t d) {
		char * c_ptr = (char *)&d;
		for (int i = 3; i >= 0; i--)
			std::cout << *(c_ptr + i);
		c_ptr = NULL;
	}

	static inline void print64char(uint32_t d) {
		char * c_ptr = (char *)&d;
		for (int i = 7; i >= 0; i--)
			std::cout << *(c_ptr + i);
		c_ptr = NULL;
	}

	static inline uint8_t read8(std::istream &is) {
		uint8_t buf;
		is.read((char*)&buf, sizeof(buf));
		return buf;
	}

	static inline uint16_t read16(std::istream &is) {
		uint16_t buf;
		is.read((char*)&buf, sizeof(buf));
		return __bswap_16(buf);
	}

	static inline uint32_t read32(std::istream &is) {
		uint32_t buf;
		is.read((char *)&buf, sizeof(buf));
		return __bswap_32(buf);
	}

	static inline uint64_t read64(std::istream &is) {
		uint64_t buf;
		is.read((char *)&buf, sizeof(buf));
		return __bswap_64(buf);
	}

	static inline void write8(std::ostream &os, uint8_t d) {
		os.write((char *)&d, sizeof(d));
	}
	static inline void write16(std::ostream &os, uint16_t d) {
		d = __bswap_16(d);
		os.write((char *)&d, sizeof(d));
	}
	static inline void write32(std::ostream &os, uint32_t d) {
		d = __bswap_32(d);
		os.write((char *)&d, sizeof(d));
	}
	static inline void write64(std::ostream &os, uint64_t d) {
		d = __bswap_64(d);
		os.write((char *)&d, sizeof(d));
	}

	//////////////////////////////////
	//box class
	//////////////////////////////////

	class Box{
	public:
		uint32_t size;
		uint32_t type;
		uint64_t largesize;
		uint8_t usertype[16];
		uint64_t position;
		uint32_t boxheadersize;
		//uint64_t getboxsize();
		uint64_t getboxsize(){
				if (largesize == 0)
					return size;
				else
					return largesize;
		}
		//Box();
		Box(){
			size = (uint32_t)-1;
			type = 0;
			largesize = 0;
			memset(usertype, 0, 16);
			position = 0;
			boxheadersize = 0;
		}
		//Box(uint32_t *ptr);
		//Box(std::ifstream &is);
		Box(std::ifstream &is){
				position = is.tellg();
				size = read32(is);
				type = read32(is);
				boxheadersize = 8;
				if (size == 1){
					largesize = read64(is);
					boxheadersize = 16;
				}
				else if (size == 0){
					//box extends to end of file
					size_t cur = is.tellg();
					is.seekg(0, is.end);
					size_t last = is.tellg();
					is.seekg(cur, is.beg);
					largesize = last - cur;
				}
				if (type == BTYPE_UUID){
					is.read((char *)usertype, 16);
					boxheadersize += 16;
				}
			}
		//~Box();
		virtual ~Box(){

		}
		virtual void print(){
			std::cout << "box size: " << size << "\nbox type: ";
			print32char(type);
			std::cout << std::endl << "box position: " << position << std::endl;
		}
		virtual void parse(std::ifstream &is){
			position = is.tellg();
			size = read32(is);
			type = read32(is);
			boxheadersize = 8;
			if (size == 1){
				largesize = read64(is);
				boxheadersize = 16;
			}
			else if (size == 0){
				//box extends to end of file
				size_t cur = is.tellg();
				is.seekg(0, is.end);
				size_t last = is.tellg();
				is.seekg(cur, is.beg);
				largesize = last - cur;
			}
			if (type == BTYPE_UUID){
				is.read((char *)usertype, 16);
				boxheadersize += 16;
			}
		}
		virtual void write(std::ofstream &os){
			write32(os, size);
			write32(os, type);
			if (size == 1){
				write64(os, largesize);
			}
			if (type == BTYPE_UUID){
				os.write((char *)usertype, 16);
			}
		}
	};

	/*Box::Box(){
		size = (uint32_t)-1;
		type = 0;
		largesize = 0;
		memset(usertype, 0, 16);
		position = 0;
		boxheadersize = 0;
	}*/

	/*Box::Box(std::ifstream &is){
		position = is.tellg();
		size = read32(is);
		type = read32(is);
		boxheadersize = 8;
		if (size == 1){
			largesize = read64(is);
			boxheadersize = 16;
		}
		else if (size == 0){
			//box extends to end of file
			size_t cur = is.tellg();
			is.seekg(0, is.end);
			size_t last = is.tellg();
			is.seekg(cur, is.beg);
			largesize = last - cur;
		}
		if (type == BTYPE_UUID){
			is.read((char *)usertype, 16);
			boxheadersize += 16;
		}
	}*/

	/*Box::~Box(){

	}*/

	/*uint64_t Box::getboxsize(){
		if (largesize == 0)
			return size;
		else
			return largesize;
	}*/

	class FullBox :public Box{
	public:
		uint8_t version;
		uint8_t flags[3];
		//FullBox();
		//FullBox(uint8_t *ptr);
		FullBox() :Box(){
				version = 0;
				memset(flags, 0, 3);
		}
		//FullBox(std::ifstream &is);
		FullBox(std::ifstream &is) : Box(is){
				is.read((char *)&version, sizeof(uint8_t));
				is.read((char *)flags, sizeof(uint8_t) * 3);
				this->boxheadersize += 4;
		}

		virtual void print(){
			Box::print();
			std::cout << "version: " << (int)version << "\nflags: ";
			for (int i = 0; i < 3; i++)
				std::cout << (int)flags[i];
			std::cout << std::endl;
		}
		virtual void parse(std::ifstream &is){
			Box::parse(is);
			is.read((char *)&version, sizeof(uint8_t));
			is.read((char *)flags, sizeof(uint8_t) * 3);
			this->boxheadersize += 4;
		}
		virtual void write(std::ofstream &os){
			Box::write(os);
			write8(os, version);
			os.write((char *)flags, 3);
		}
	};

	/*FullBox::FullBox() :Box(){
		version = 0;
		memset(flags, 0, 3);
	}*/

	/*FullBox::FullBox(std::ifstream &is) : Box(is){
		is.read((char *)&version, sizeof(uint8_t));
		is.read((char *)flags, sizeof(uint8_t) * 3);
		this->boxheadersize += 4;
	}
*/
	/*Box * getBox(std::ifstream &is, uint32_t type, uint64_t pos = 0, uint64_t size = 0){

		Box * box_ptr = new Box();

		if (size == 0){
			is.seekg(0, is.end);
			size = is.tellg();
		}

		is.seekg(pos, is.beg);

		while (true){
			box_ptr->parse(is);

			if (box_ptr->type == type){
				Box * fullbox_ptr;
				if (isFullBox(type)){
					is.seekg(box_ptr->position, is.beg);
					delete box_ptr;
					fullbox_ptr = new FullBox();
					fullbox_ptr->parse(is);
					box_ptr = fullbox_ptr;
					fullbox_ptr = NULL;
				}

				is.seekg(pos, is.beg);
				return box_ptr;
			}
			else if (!check_vaild_type(box_ptr->type) || size <= box_ptr->size + box_ptr->position){
				is.seekg(pos, is.beg);
				return NULL;
			}
			else{
				is.seekg(box_ptr->size + box_ptr->position, is.beg);
			}
		}
	}*/
}
