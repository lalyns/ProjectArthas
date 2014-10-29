/************************************************************************/
/*
	CLASS			: GameLayer
	Author			: �迬��
	����				: ���ӿ� ���Ǵ� ��� ������Ʈ���� �ִ� ���̾�
	���� ��������	: 2014-10-29
	���� ������		:
	���� ��������	:
	Comment			: �׳� Ŭ������ ������, ������ �޾Ƽ� �뷹�̾�� ����� �ֵ����ϵ����, �÷��̾� ��ġ ������
*/
/************************************************************************/

#pragma once
#include "cocos2d.h"
#include "Util.h"
#define MAX_ROOM_LAYER_NUM 100

BEGIN_NS_AT

class Player;
class RoomLayer;
class GameLayer : public cocos2d::Layer
{
public:
	OVERRIDE bool					init();
	OVERRIDE void					update( float dTime );

	CREATE_FUNC( GameLayer );

private:
	RoomLayer* m_RoomLayers[MAX_ROOM_LAYER_NUM];
	Player*    m_Player;
};

END_NS_AT