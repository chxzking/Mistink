#include "heap_solution_1.h"

#include "EngineFault.h"	//���������

#if HEAP_MEMORY_CONFIG == MEMORY_MODE_1                 	//���������ж�

#define	FREE_MEMORY 				0x0						//�����ڴ�
#define	USAGE_MEMORY				0x1						//��ʹ���ڴ�
#define MINI_MEMORY_THRESHOLD 		4						//��С�ڴ����ֵ�����ڴ��С�������ֵʱ����ǰ����ڴ�鷢���ϲ�


/**
*	@brief �ڴ��ṹ��
*
*
*  ��λ attribute�ֶ�						��λ������2��
*	+---------------------+-----------+-----+
*	|  0   1   2   3   4  |  5  |  6  |  7  | 
*	+---------------------+-----+-----+-----+
*	|    �ϲ�size����     | ���� | �ϲ�| ����| 
*	| MERGE_FIELDS_SIZE   |     | EN  | flag|
*	+---------------------+-----+-----+-----+
*/
typedef struct MemoryBlock {
    unsigned int size;
    struct MemoryBlock* next;
    unsigned char attribute;
	
	unsigned char checksum;		//�ڴ���ƿ�������У���루����ȷ�ϴ��ֶ����ڴ���ƿ飩
}MemoryBlock;

#define MERGE_FIELDS_SIZE		5							//�ϲ��ڴ��λ���ȣ�(���ṹ��ͼ���������6)

#define	FREE_FIELDS_MARK		0x01						//�ڴ������ֶ�����
#define MERGE_FIELDS_MARK		0x02						//�ϲ�ʹ���ֶ�����
#define MERGE_SIZE_FIELDS_MARK	(0xff<<( 8 - MERGE_FIELDS_SIZE)) 	//�ϲ��ֶεĳ�������

#define	MERGE_FIELDS_ENABLE		0x02						//�ϲ���־ʹ��
#define MERGE_FIELDS_DISNABLE	0x00						//�ϲ���־ʧ��



/**
*	@brief	�����ϲ���־λ״̬�����塣�����־�����жϵ�ǰ�ڴ���Ƿ�ϲ���һ����С��ֵ���ڴ��
*	@param 	x-->�ڴ���attribute�ֶ�
*			status --> �ϲ���־λ����ѡ������MERGE_FIELDS_ENABLE��MERGE_FIELDS_DISNABLE
*
*	@retval	GetMergeAttrStatus(x)����ֵΪMERGE_FIELDS_ENABLE��MERGE_FIELDS_DISNABLE
*/
//��ȡ�ϲ���־λ��״̬
#define GetMergeAttrStatus(x)	((x) & MERGE_FIELDS_MARK)		
//���úϲ���־λ��״̬
#define SetMergeAttrStatus(x, status) ((status == MERGE_FIELDS_ENABLE) ? ((x) |= MERGE_FIELDS_ENABLE) : ((x) &= (~MERGE_FIELDS_ENABLE)))

/**
*	@brief	�����ڴ�ʹ��״̬������
*	@param 	x-->�ڴ���attribute�ֶ� 
*			status --> �ڴ�״̬����ѡ������FREE_MEMORY��USAGE_MEMORY
*
*	@retval	GetMemoryAttrStatus(x)����ֵΪFREE_MEMORY��USAGE_MEMORY
*/
//��ȡ�ڴ��ʹ��״̬
#define GetMemoryAttrStatus(x)	((x) & FREE_FIELDS_MARK)		
//�����ڴ��ʹ��״̬	
#define SetMemoryAttrStatus(x,status) ( (status == FREE_MEMORY)?((x) &= (~USAGE_MEMORY)):((x) |= USAGE_MEMORY) )
/**
*	@brief	�����ϲ����Ⱥ����壬С����С��ֵ���ڴ�鵱ǰ�Ƕ೤��
*	@param 	x-->�ڴ���attribute�ֶ� 
*			size --> �ڴ泤�ȣ������÷�Χ0~(2^MERGE_FIELDS_SIZE)
*
*	@retval GetMergeAttrSize(x)	����ֵΪsize��С
*/
#define GetMergeAttrSize(x)		(((x) & MERGE_SIZE_FIELDS_MARK) >> (8 - MERGE_FIELDS_SIZE))		//��ȡ�ϲ��ڴ泤��
//���úϲ��ڴ泤��
#define SetMergeAttrSize(x, size) ((x) = ((x) & (~MERGE_SIZE_FIELDS_MARK)) | (((size) << (8 - MERGE_FIELDS_SIZE)) & MERGE_SIZE_FIELDS_MARK))

/**
*	@brief	�����ڴ������
*	@param 	x-->�ڴ���attribute�ֶ� 
*			mergeStatus	-->		�ϲ�״̬
*			useStatus	-->		ʹ��״̬
*			size --> �ڴ��С�������÷�Χ0~(2^MERGE_FIELDS_SIZE)
*/
#define SetMemoryAttribute(x,mergeStatus,useStatus,size)	do {\
															SetMergeAttrStatus(x, mergeStatus);\
															SetMemoryAttrStatus(x,useStatus);\
															SetMergeAttrSize(x,size);\
															} while (0)

static unsigned char MEMORY_POOL[HEAP_POOL_SIZE];			//�����ڴ��

unsigned char	Init_flag = 0;								//�ڴ�س�ʼ����־

static MemoryBlock* BlockListHeader = (MemoryBlock*)MEMORY_POOL;	//�׸��ڴ�տ��ַ


//�ڴ�У��
// ���� CRC8 У���루ʹ�� CRC-8-CCITT ����ʽ 0x07�� 
unsigned char calculate_crc8(const unsigned char *data, unsigned int length); 
// ��֤�ڴ���У���� 
int verifyBlock(MemoryBlock* block);															
#define VERIFYBLOCK_FAIL				0		//��֤�ڴ�У����ʧ��
#define VERIFYBLOCK_SUCCESS				1		//��֤�ڴ�У����ɹ�														
															
															
/******			�����ڴ�Ϸ�����֤���°汾				*******/
//�ڴ��ʼ��
void memory_pool_init(void) {
	if(Init_flag == 1)	return;
	Init_flag = 1;//�����ʼ����־
    BlockListHeader->size = HEAP_POOL_SIZE - sizeof(MemoryBlock);
    BlockListHeader->next = C_NULL;
	//��ʼ���ڴ�����Ϊ�ϲ�ʧ�ܣ������ڴ��0����
	SetMemoryAttribute(BlockListHeader->attribute, MERGE_FIELDS_DISNABLE, FREE_MEMORY, 0);
	// ����У���룬�� size��next �� attribute �ֶι���
	BlockListHeader->checksum = calculate_crc8((unsigned char*)BlockListHeader, sizeof(BlockListHeader->size) + sizeof(BlockListHeader->next) + sizeof(BlockListHeader->attribute));
	return;
}
void* c_malloc(unsigned int size){
	if(size == 0)	return C_NULL;//У������
	//��ȡ��һ����������
	MemoryBlock* current = BlockListHeader;
	
    while (current != C_NULL) {
		//�����������в����ڴ�ռ��㹻
        if ((GetMemoryAttrStatus(current->attribute) == FREE_MEMORY) && (current->size >= size)) {
			//�����ڴ�鱻ʹ�ú�ʣ�µĿռ��Ƿ�С����С�ռ���ֵ
			
			//ʣ���ڴ�鲻С����С��ֵ�������������Ϊ�µ��ڴ��
			int Residual_memory_block = current->size - size - sizeof(MemoryBlock);
			if( Residual_memory_block >= MINI_MEMORY_THRESHOLD){
				//�����µĿ����ڴ��
                MemoryBlock* new_block = (MemoryBlock*)((unsigned char*)current + sizeof(MemoryBlock) + size);
                new_block->size = current->size - size - sizeof(MemoryBlock);
                new_block->next = current->next;
				SetMemoryAttribute(new_block->attribute, MERGE_FIELDS_DISNABLE, FREE_MEMORY, 0);
                current->size = size;
                current->next = new_block;
				//�رյ�ǰ�ϲ���־
				SetMergeAttrStatus(current->attribute, MERGE_FIELDS_DISNABLE);
				
			}
			//ʣ���ڴ��С����С��ֵ������Ϊ���ڴ汻ʹ��
			else{
				//�ϲ�ʹ��
				SetMergeAttrStatus(current->attribute, MERGE_FIELDS_ENABLE); 
				//����Ϊ����Ŀ�ĳ���
				SetMergeAttrSize(current->attribute, Residual_memory_block + sizeof(MemoryBlock));
			}
			//���õ�ǰ�ڴ�������
			SetMemoryAttrStatus(current->attribute,USAGE_MEMORY);
			
			//�����ڴ�������
			current->checksum = calculate_crc8((unsigned char*)current, sizeof(current->size) + sizeof(current->next) + sizeof(current->attribute));
            //���ص�ַ
			return (unsigned char*)current + sizeof(MemoryBlock);
        }
		//����ѡ����һ��
        current = current->next;
    }
    return C_NULL;  // ���㹻�ڴ�
}


void* c_realloc(void* ptr,unsigned int size){
	//�������У��
	if(ptr == C_NULL)	return C_NULL;
	//��׼����ǰָ��
	MemoryBlock* block = (MemoryBlock*)((unsigned char*)ptr - sizeof(MemoryBlock));
	//��⴫���ָ���Ƿ�Ϸ�
	if(verifyBlock(block) == VERIFYBLOCK_FAIL){
		//������<�Ƿ�����>
		EngineFault_IllegalAccess_Handle();
	}
	
	//����Ƿ����Ż��ϲ��Ĳ���
	//������ںϲ���������ô����������ڴ�
	if(GetMergeAttrStatus(block->attribute) == MERGE_FIELDS_ENABLE){
		//����Ƿ�Ϊ�����ڴ��Ƿ�Ϊ�ڴ�ռ�
		if(size < (block->size - GetMergeAttrSize(block->attribute))){
			return C_NULL;//�������С�ڴ��������˳�
		}
		
		//��鵱ǰ�ڴ�������ص��ڴ���Ƿ���������
		int temp = block->size - size; 
		if(temp >= 0){
			//�������Ҫ����ֱ����չ
			SetMergeAttrSize(block->attribute, temp);
			if(temp == 0){
				//������е������ڴ�պö������꣬��ʧ�ܺϲ���־
				SetMergeAttrStatus(block->attribute, MERGE_FIELDS_DISNABLE);
			}
			//�����ڴ�������
			block->checksum = calculate_crc8((unsigned char*)block, sizeof(block->size) + sizeof(block->next) + sizeof(block->attribute));
			return ptr;//�����ڴ��ַ
		}	
	}
	else{
		//���û�кϲ������������·���ռ�С�ڵ�ǰ�ռ�����Ϊ������������˳�
		if(size < block->size)
			return C_NULL;
	}
	
	//�����һ�������Ƿ��ǿ��е�
	if(block->next != C_NULL && (GetMemoryAttrStatus(block->next->attribute) == FREE_MEMORY)){
		//�����һ�����鴦�ڿ���״̬����������֤�ռ��Ƿ���������ϲ�
		int temp = block->next->size - (size - block->size);//��ȡ��һ���ڴ�鱻ʹ�ú󻹻�ʣ����ٿռ�
		
		if(temp >= MINI_MEMORY_THRESHOLD){
			//��һ��ռ����
			//��ȡƫ����
			unsigned int offset = size - block->size;
			//��ȡ��һ�������ڴ��ַ
			MemoryBlock* next_block = block->next;
			//�����ڴ���С
			next_block->size -= offset;
			//��ȡβ���ַ
			next_block++;
			unsigned char* tail = ((unsigned char*)next_block);
			tail--;//����һ���ֽڵĵ�ַ��
			
			//�ڴ渴��
			for(unsigned int i = 0;i<sizeof(MemoryBlock);i++){
				*(tail + offset) = *tail;//���õ�ַ��ֵ���ߵ�ַƫ��offest����λ
				tail--;
			}
			unsigned char* newBlockHead = tail+offset;//��ȡ�µ��ڴ���׵�ַλ��
			//�������ڴ渴���ж����һλ������Ӧ�üӻ���
			newBlockHead++;
			
			//���µ�ǰ��nextָ��
			block->next = (MemoryBlock*)newBlockHead;
			//�����ڴ����´�С
			block->size = size;
			//������������ڴ�飬��ôҲ�������ˣ����Բ��ܴ治����ͳһ���úϲ���־λʧ��
			SetMergeAttrStatus(block->attribute, MERGE_FIELDS_DISNABLE);
			//�����ڴ�������
			block->checksum = calculate_crc8((unsigned char*)block, sizeof(block->size) + sizeof(block->next) + sizeof(block->attribute));
			//���ص�ַ
			return ptr;
		}
		//���ʣ���ڴ��С������ֵ,���ǿռ��㹻����ֱ�Ӻϲ�
		else if(temp < MINI_MEMORY_THRESHOLD && temp >= 0){
			block->next = block->next->next;//����nextָ��
			SetMergeAttrSize(block->attribute, temp + sizeof(MemoryBlock));//����ʣ����
			block->size += (block->next->size + sizeof(MemoryBlock));//���õ�ǰ�ڴ�����
			//�����ڴ�������
			block->checksum = calculate_crc8((unsigned char*)block, sizeof(block->size) + sizeof(block->next) + sizeof(block->attribute));
			//���ص�ַ
			return ptr;
		}
		//��һ���ڴ��Ҳ���㹻����Ҫ�������ڴ�,���Լ���ִ���·�����
	}

	//��һ��ռ䲻���㣬��ô���¿���һ���¿ռ䣬���������ݹ�ȥ
	unsigned char* new_block =  (unsigned char*)c_malloc(size);
	if(new_block == C_NULL)	return C_NULL;//�ռ����ʧ�ܣ����ؿյ�ַ
		
	//�ռ����ɹ������и���
	for(unsigned int i = 0;i < block->size;i++){
		new_block[i] = ((unsigned char*)ptr)[i];
	}
	//�ͷ�ԭ�ռ�
	c_free(ptr);
	//�½��ڴ治��Ҫ�����ڴ�����룬��Ϊc_malloc�ڲ��Ѿ�����
	return new_block;
}


void c_free(void* ptr){
	if (ptr == C_NULL) return;//���ڴ淵��

	//�ڴ���ƿ��ȡ
    MemoryBlock* block = (MemoryBlock*)((unsigned char*)ptr - sizeof(MemoryBlock));
	
	//�����ڴ���ƿ��ͷ��Ƿ�Ϸ�
	if(verifyBlock(block) == VERIFYBLOCK_FAIL){
		//��֤ʧ��
		//������<��̬�ڴ�Ƿ��ͷ�>
		EngineFault_IllegalFree_Handle();	
	}

	//���ø��ڴ�Ϊ����״̬
	SetMemoryAttrStatus(block->attribute,FREE_MEMORY);

    // �Ե�ǰ�׸��յ�ַ�ڴ�鿪ʼ���ϲ����ڵĿ��п�
    MemoryBlock* current = BlockListHeader;
    while (current != C_NULL) {
        if ( (GetMemoryAttrStatus(current->attribute) == FREE_MEMORY ) && ( current->next != C_NULL ) && (GetMemoryAttrStatus(current->next->attribute) == FREE_MEMORY)) {
            current->size += current->next->size + sizeof(MemoryBlock);
            current->next = current->next->next;
			/*
				return Ϊ����Ӵ��룺���ǣ�������Ҫ�ڵ��̻߳�����һ�����ֻ���ܳ���һ���ڴ����ͷţ�
				û�б�Ҫ���������ڴ��ȫ��ɨ�衣
			*/
			return;
        }
        current = current->next;
    }
	
	return;
}



//�ڴ�������У��


// ���� CRC8 У���루ʹ�� CRC-8-CCITT ����ʽ 0x07�� 
unsigned char calculate_crc8(const unsigned char *data, unsigned int length) { 
	unsigned char crc = 0x00; 
	unsigned char poly = 0x07; // CRC-8-CCITT ����ʽ 
	for (unsigned int i = 0; i < length; i++) { 
		crc ^= data[i]; 
			for (unsigned char j = 0; j < 8; j++) { 
				if (crc & 0x80)
					crc = (crc << 1) ^ poly;
				else crc <<= 1;
			}
	}
	return crc; 
}

// ��֤�ڴ���У����,����ʧ�ܷ���0����֤ͨ������1
int verifyBlock(MemoryBlock* block) { 
	unsigned char expected_checksum = calculate_crc8((unsigned char*)block, sizeof(block->size) + sizeof(block->next) + sizeof(block->attribute)); 
	if(block->checksum != expected_checksum){
		return VERIFYBLOCK_FAIL;
	}
	return VERIFYBLOCK_SUCCESS;
}


#endif	//HEAP_SCHEME == HEAP_SOLUTION_1


/****************************    	����Ϊ��ʼ�汾�Ĵ���			***********************************/
/*
	���˵����
		1��c_malloc �����������ԣ�û�з�������
		2��c_realloc ͨ���������ԣ�û�з�������
		3��c_free	û������
*/

////�ڴ��ʼ��
//void memory_pool_init(void) {
//	if(Init_flag == 1)	return;
//	Init_flag = 1;//�����ʼ����־
//    BlockListHeader->size = HEAP_POOL_SIZE - sizeof(MemoryBlock);
//    BlockListHeader->next = C_NULL;
//	//��ʼ���ڴ�����Ϊ�ϲ�ʧ�ܣ������ڴ��0����
//	SetMemoryAttribute(BlockListHeader->attribute, MERGE_FIELDS_DISNABLE, FREE_MEMORY, 0);
//	return;
//}
//void* c_malloc(unsigned int size){
//	if(size == 0)	return C_NULL;//У������
//	//��ȡ��һ����������
//	MemoryBlock* current = BlockListHeader;
//	
//    while (current != C_NULL) {
//		//�����������в����ڴ�ռ��㹻
//        if ((GetMemoryAttrStatus(current->attribute) == FREE_MEMORY) && (current->size >= size)) {
//			//�����ڴ�鱻ʹ�ú�ʣ�µĿռ��Ƿ�С����С�ռ���ֵ
//			
//			//ʣ���ڴ�鲻С����С��ֵ�������������Ϊ�µ��ڴ��
//			int Residual_memory_block = current->size - size - sizeof(MemoryBlock);
//			if( Residual_memory_block >= MINI_MEMORY_THRESHOLD){
//				//�����µĿ����ڴ��
//                MemoryBlock* new_block = (MemoryBlock*)((unsigned char*)current + sizeof(MemoryBlock) + size);
//                new_block->size = current->size - size - sizeof(MemoryBlock);
//                new_block->next = current->next;
//				SetMemoryAttribute(new_block->attribute, MERGE_FIELDS_DISNABLE, FREE_MEMORY, 0);
//                current->size = size;
//                current->next = new_block;
//				//�رյ�ǰ�ϲ���־
//				SetMergeAttrStatus(current->attribute, MERGE_FIELDS_DISNABLE);
//				
//			}
//			//ʣ���ڴ��С����С��ֵ������Ϊ���ڴ汻ʹ��
//			else{
//				//�ϲ�ʹ��
//				SetMergeAttrStatus(current->attribute, MERGE_FIELDS_ENABLE); 
//				//����Ϊ����Ŀ�ĳ���
//				SetMergeAttrSize(current->attribute, Residual_memory_block + sizeof(MemoryBlock));
//			}
//			//���õ�ǰ�ڴ�������
//			SetMemoryAttrStatus(current->attribute,USAGE_MEMORY);
//			//���ص�ַ
//            return (unsigned char*)current + sizeof(MemoryBlock);
//        }
//		//����ѡ����һ��
//        current = current->next;
//    }
//    return C_NULL;  // ���㹻�ڴ�
//}


//void* c_realloc(void* ptr,unsigned int size){
//	//�������У��
//	if(ptr == C_NULL)	return C_NULL;
//	//��׼����ǰָ��
//	MemoryBlock* block = (MemoryBlock*)((unsigned char*)ptr - sizeof(MemoryBlock));
//	//����Ƿ����Ż��ϲ��Ĳ���
//	//������ںϲ���������ô����������ڴ�
//	if(GetMergeAttrStatus(block->attribute) == MERGE_FIELDS_ENABLE){
//		//����Ƿ�Ϊ�����ڴ��Ƿ�Ϊ�ڴ�ռ�
//		if(size < (block->size - GetMergeAttrSize(block->attribute))){
//			return C_NULL;//�������С�ڴ��������˳�
//		}
//		
//		//��鵱ǰ�ڴ�������ص��ڴ���Ƿ���������
//		int temp = block->size - size; 
//		if(temp >= 0){
//			//�������Ҫ����ֱ����չ
//			SetMergeAttrSize(block->attribute, temp);
//			if(temp == 0){
//				//������е������ڴ�պö������꣬��ʧ�ܺϲ���־
//				SetMergeAttrStatus(block->attribute, MERGE_FIELDS_DISNABLE);
//			}
//			return ptr;//�����ڴ��ַ
//		}	
//	}
//	else{
//		//���û�кϲ������������·���ռ�С�ڵ�ǰ�ռ�����Ϊ������������˳�
//		if(size < block->size)
//			return C_NULL;
//	}
//	
//	//�����һ�������Ƿ��ǿ��е�
//	if(block->next != C_NULL && (GetMemoryAttrStatus(block->next->attribute) == FREE_MEMORY)){
//		//�����һ�����鴦�ڿ���״̬����������֤�ռ��Ƿ���������ϲ�
//		int temp = block->next->size - (size - block->size);//��ȡ��һ���ڴ�鱻ʹ�ú󻹻�ʣ����ٿռ�
//		
//		if(temp >= MINI_MEMORY_THRESHOLD){
//			//��һ��ռ����
//			//��ȡƫ����
//			unsigned int offset = size - block->size;
//			//��ȡ��һ�������ڴ��ַ
//			MemoryBlock* next_block = block->next;
//			//�����ڴ���С
//			next_block->size -= offset;
//			//��ȡβ���ַ
//			next_block++;
//			unsigned char* tail = ((unsigned char*)next_block);
//			tail--;//����һ���ֽڵĵ�ַ��
//			
//			//�ڴ渴��
//			for(unsigned int i = 0;i<sizeof(MemoryBlock);i++){
//				*(tail + offset) = *tail;//���õ�ַ��ֵ���ߵ�ַƫ��offest����λ
//				tail--;
//			}
//			unsigned char* newBlockHead = tail+offset;//��ȡ�µ��ڴ���׵�ַλ��
//			//�������ڴ渴���ж����һλ������Ӧ�üӻ���
//			newBlockHead++;
//			
//			//���µ�ǰ��nextָ��
//			block->next = (MemoryBlock*)newBlockHead;
//			//�����ڴ����´�С
//			block->size = size;
//			//������������ڴ�飬��ôҲ�������ˣ����Բ��ܴ治����ͳһ���úϲ���־λʧ��
//			SetMergeAttrStatus(block->attribute, MERGE_FIELDS_DISNABLE);
//			//���ص�ַ
//			return ptr;
//		}
//		//���ʣ���ڴ��С������ֵ,���ǿռ��㹻����ֱ�Ӻϲ�
//		else if(temp < MINI_MEMORY_THRESHOLD && temp >= 0){
//			block->next = block->next->next;//����nextָ��
//			SetMergeAttrSize(block->attribute, temp + sizeof(MemoryBlock));//����ʣ����
//			block->size += (block->next->size + sizeof(MemoryBlock));//���õ�ǰ�ڴ�����
//			//���ص�ַ
//			return ptr;
//		}
//		//��һ���ڴ��Ҳ���㹻����Ҫ�������ڴ�,���Լ���ִ���·�����
//	}

//	//��һ��ռ䲻���㣬��ô���¿���һ���¿ռ䣬���������ݹ�ȥ
//	unsigned char* new_block =  (unsigned char*)c_malloc(size);
//	if(new_block == C_NULL)	return C_NULL;//�ռ����ʧ�ܣ����ؿյ�ַ
//		
//	//�ռ����ɹ������и���
//	for(unsigned int i = 0;i < block->size;i++){
//		new_block[i] = ((unsigned char*)ptr)[i];
//	}
//	//�ͷ�ԭ�ռ�
//	c_free(ptr);
//	return new_block;
//}


//void c_free(void* ptr){
//	if (ptr == C_NULL) return;//���ڴ淵��

//	//�ڴ���ƿ��ȡ
//    MemoryBlock* block = (MemoryBlock*)((unsigned char*)ptr - sizeof(MemoryBlock));
//	
//	//���ø��ڴ�Ϊ����״̬
//	SetMemoryAttrStatus(block->attribute,FREE_MEMORY);

//    // �Ե�ǰ�׸��յ�ַ�ڴ�鿪ʼ���ϲ����ڵĿ��п�
//    MemoryBlock* current = BlockListHeader;
//    while (current != C_NULL) {
//        if ( (GetMemoryAttrStatus(current->attribute) == FREE_MEMORY ) && ( current->next != C_NULL ) && (GetMemoryAttrStatus(current->next->attribute) == FREE_MEMORY)) {
//            current->size += current->next->size + sizeof(MemoryBlock);
//            current->next = current->next->next;
//        }
//        current = current->next;
//    }
//	return;
//}


