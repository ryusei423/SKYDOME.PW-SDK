#pragma once

//��CSGOʱ����������Ϊ"esp"����ƺ���Щ�����������������ߵ�������
//Ȼ���ҽ�����������ʱ�����һ�£�Extra-Sensory Perception�����й�֪����
//�е�ᣬ�Ҵ�������ʹ���������(��realy 19c-aim v0.15���ʼ����������esp���ں��ڵĸ�������дʱ������Ϊplayervision)
#include"../../sdk/datatypes/matrix.h"



//����ÿһ֡���´�����
struct EspItem{
	Vector pos;
	Vector size;

};

class EspDrawManager
{
public:

	
	ViewMatrix_t matrix;
private:
	



};

inline EspDrawManager* g_EspDrawManager = new EspDrawManager;