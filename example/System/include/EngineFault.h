#ifndef __ENGINE_FAULT_H__
#define __ENGINE_FAULT_H__




//�ڴ�Ƿ�����
void EngineFault_IllegalAccess_Handle(void);

//��ָ�����
void EngineFault_NullPointerGuard_Handle(void);

//��̬�ڴ�Ƿ��ͷ�
void EngineFault_IllegalFree_Handle(void);

//��Դ����
void EngineFault_InadequateResources_Handle(void);
#endif
