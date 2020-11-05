#include "Section.hpp"
#include "Platform.hpp"

#include <easylogging++.h>

void Section::CalculateHash() const {
	if (!blocks) {
        hash = 0;
        return;
    }


	size_t llen = hasSkyLight ? 4096 : 2048;
	size_t sz = (16*16*16/2) << (this->pow - 2);
	std::vector<unsigned char> rawData;
	rawData.resize(sz + llen);

	std::memcpy(rawData.data(), light->block, 2048);
	if(hasSkyLight)
		std::memcpy(rawData.data() + 2048, light->sky, 2048);
	std::memcpy(rawData.data() + llen, blocks, sz);
    
    const unsigned char *from = reinterpret_cast<const unsigned char *>(rawData.data());
    size_t length = rawData.size();

    std::string str(from, from + length);
    hash =  std::hash<std::string>{}(str);
}

Section::Section(Vector pos, unsigned char bitsPerBlock, std::vector<unsigned short> palette, std::vector<uint8_t> blockData, const std::vector<unsigned char> &lightData, const std::vector<unsigned char> &skyData)
	: hasSkyLight(!skyData.empty()) {
	//Align blocks
	if (bitsPerBlock < 4) {
		LOG(ERROR) << "bitsPerBlock < 4 for chunk " << pos;
		return;
	} else if (bitsPerBlock == 4)
		this->bitsPerBlock = 4, this->pow = 2;
	else if (bitsPerBlock <= 8)
		this->bitsPerBlock = 8, this->pow = 3;
	else if (bitsPerBlock <= 16)
		this->bitsPerBlock = 16, this->pow = 4;
	else {
		LOG(ERROR) << "bitsPerBlock > 16 for chunk " << pos;
		return;
	}

    this->worldPosition = pos;
	this->palette = std::move(palette);
	size_t sz = (16*16*16/2) << (this->pow - 2);
	this->blocks = reinterpret_cast<uint16_t*>(malloc(sz));
	#define p(i) ptr[7+((i)&~7)-((i)&7)]
	if (this->bitsPerBlock <= 8) {
		if (this->bitsPerBlock == bitsPerBlock) {
			//Swap and write
			const uint64_t *lptr = reinterpret_cast<uint64_t*>(blockData.data());
			uint64_t *dlptr = reinterpret_cast<uint64_t*>(blocks);
			for (size_t i=0; i<16*16*16*bitsPerBlock/(sizeof(uint64_t)*8); i++) {
				dlptr[i] = bswap_64(lptr[i]);
			}
		} else { // only when bPB>4 && bPB<8
			//Expand?
			const uint8_t *ptr = reinterpret_cast<uint8_t*>(blockData.data());
			uint8_t *dst = reinterpret_cast<uint8_t*>(blocks);
			const uint16_t mask = 0xffff >> (16 - bitsPerBlock);
			for (size_t i = 0; i < 16*16*16; i++) {
				size_t ib = i * bitsPerBlock;
				size_t iy = ib / 8;
				uint8_t offset = ib%8;
				uint16_t buf = p(iy);
				if (offset > 8 - bitsPerBlock)
					buf |= (p(iy+1) << 8);
				dst[i] = (buf >> offset) & mask;
			}
		}
	} else {
		//Align to uint16_t and swap to host endian
		const uint8_t *ptr = reinterpret_cast<uint8_t*>(blockData.data());
		const uint16_t mask = 0xffff >> (16 - bitsPerBlock);
		for (size_t i = 0; i < 16*16*16; i++) {
			size_t ib = i * bitsPerBlock;
			size_t iy = ib / 8;
			uint8_t offset = ((ib - iy*8)%16);
			unsigned int buf = p(iy);
			if (offset > 16 - bitsPerBlock)
				buf |= (p(iy+1) << 8) | (p(iy+2) << 16);
			else if (offset > 8 - bitsPerBlock)
				buf |= p(iy+1) << 8;
			blocks[i] = (buf >> (ib % 8)) & mask;
		}
	}
	#undef p

	light = reinterpret_cast<struct lightData*>(malloc(skyData.empty() ? 2048 : 4096));
	std::copy(lightData.begin(), lightData.end(), light->block);//Copy block lightning
	if (!skyData.empty())
		std::copy(skyData.begin(), skyData.end(), light->sky);

    hash = -1;
	CalculateHash();
	std::atomic_thread_fence(std::memory_order_release);
}

BlockId Section::GetBlockId(Vector pos) const {
	if (!blocks)
		return {0, 0};

	unsigned int virtualIndex = ((((pos.y * 16) + pos.z) * 16) + pos.x);
	BlockId ret;

	uint16_t value;


	mutex.lock_shared();//Prevent expanding and use acquire memory orded
	if (bitsPerBlock <= 8) {//4 or 8
		uint8_t *block = reinterpret_cast<uint8_t*>(blocks);//Make endianess independent
		unsigned int realIndex = virtualIndex >> (3 - this->pow);
		uint8_t paletted;
		if (bitsPerBlock == 4) {
			//Nibbles are swapped
			uint8_t nibbleIndex = virtualIndex & 1;
			paletted = (unsigned)(block[realIndex] >> (4 * nibbleIndex)) & 0xF;// (0xF0 >> ((3 - nibbleIndex - this->pow) * 4))));
		} else
			paletted = block[realIndex];

		if (paletted >= palette.size()) {
			LOG(ERROR) << "Out of palette: " << paletted;
			return {0, 0};
		} else
			value = palette[paletted];
	} else {//16
		value = blocks[virtualIndex];
	}
	mutex.unlock_shared();


	ret.id = value >> 4;
	ret.state = value & 0x0F;
	return ret;
}

unsigned char Section::GetBlockLight(Vector pos) const {
	if (!light)
		return 0;
	int blockNumber = pos.y * 256 + pos.z * 16 + pos.x;
	unsigned char lightValue = this->light->block[blockNumber / 2];
	return (blockNumber % 2 == 0) ? (lightValue & 0xF) : (lightValue >> 4);
}

unsigned char Section::GetBlockSkyLight(Vector pos) const {
	if (!hasSkyLight)
		return 0;
	int blockNumber = pos.y * 256 + pos.z * 16 + pos.x;
	unsigned char skyValue = this->light->sky[blockNumber / 2];
	return (blockNumber % 2 == 0) ? (skyValue & 0xF) : (skyValue >> 4);
}

void Section::SetBlockId(Vector pos, BlockId value) {
	SetBlock((pos.y * 256) + (pos.z * 16) + pos.x, (value.id << 4) | value.state);
}

//Should be called only from main thread
void Section::SetBlock(unsigned int num, uint16_t block) {
	//Don't lock mutex, we are in main thread, it's nothing to worry about. I hope.
	if (bitsPerBlock > 8) {//We use only 16 bits per block
		//Just set
		blocks[num] = block;
	} else {
		uint8_t *ptr = reinterpret_cast<uint8_t*>(blocks);
		size_t psz = palette.size();
		int index = -1;
		for (unsigned int i = 0; i < psz; i++) {//Search for block in palette
			if (palette[i] == block) {
				index = i;
				break;
			}
		}
		if (index == -1) {//Not found
			if (psz == ((uint)1<<bitsPerBlock)){
				//Compact palette
				//Expand bPB
				ExpandBPB();
				SetBlock(num, block);
				return;
			} else {
				//Add blockID to palette
				palette.push_back(block);
				index = psz;
			}
		}
		//Set by index
		if (bitsPerBlock == 4) {
			ptr[num/2] = (ptr[num/2] & (0x0F << (((num+1)&1) << 2))) | (index << ((num&1) << 2));
		} else {
			ptr[num] = index;
		}
	}
	hash = ~(size_t)0;
	CalculateHash();
}

void Section::ExpandBPB() noexcept {
	bool locked = mutex.try_lock();//We are in main thread! Try to lock ASAP!

	size_t sz = (16*16*16/2) << (this->pow - 1);
	uint16_t *expanded = reinterpret_cast<uint16_t*>(malloc(sz));
	if (this->pow == 2) {//Expand to 8 bPB
		uint8_t *old = reinterpret_cast<uint8_t*>(blocks);
		uint8_t *new_blocks = reinterpret_cast<uint8_t*>(expanded);
		for (size_t i = 0; i < 16*16*16; i++) {
			uint8_t nibble = i&1;
			new_blocks[i] = (old[i>>1] >> (nibble << 2)) & 0x0F;
		}
	} else if (this->pow == 3) {//Expand to 16 bPB
		for (size_t i = 0; i < 16*16*16; i++) {
			expanded[i] = palette[blocks[i]];
		}
	}

	//Lock and swap
	if (!locked)
		mutex.lock();
	free(blocks);
	if (this->bitsPerBlock == 8) {
		palette.clear();
		palette.shrink_to_fit();
	}
	blocks = expanded;
	this->bitsPerBlock = 1 << (++this->pow);
	mutex.unlock();
}


Vector Section::GetPosition() const {
	return worldPosition;
}

size_t Section::GetHash() const {
	if (hash == ~(size_t)0)
        CalculateHash();
    return hash;
}

Section::~Section() {
	if (light)
		free(light);
	if (blocks)
		free(blocks);
	light = nullptr;
	blocks = nullptr;
}
