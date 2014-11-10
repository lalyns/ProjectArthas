/************************************************************************/
/*
CLASS			: MinimapLayer
Author			: 우재우
역할			: Minimap을 보여주고, M키를 눌렀을 때 확대 뷰까지 보여준다!
최종 수정일자	: 2014-11-05
최종 수정자		:
최종 수정사유	:
Comment			: 
*/
/************************************************************************/

#pragma once
#include "cocos2d.h"
#include "Util.h"
#include "GameSceneUILayer.h"

BEGIN_NS_AT

class MinimapLayer : public GameSceneUILayer
{
public:
	MinimapLayer();
	~MinimapLayer();

	OVERRIDE bool			init();
	OVERRIDE void			update(float dTime);

	void					setUpMap();
	//플로어가 바뀔 때, 흔들릴 때 커맨드 받아서 호출해야 할 함수
	void					showMapWin();
	void					hideMapWin();
	bool					getMapWinOn();

	CREATE_FUNC(MinimapLayer);

private:
	//Draw Map
	cocos2d::DrawNode*		m_Map;
	cocos2d::DrawNode*		m_Minimap;
	StageData				m_StageData;
	int						m_ModuleSize;

	int						m_CurrentFloor;
	std::vector<int>		m_VisitedRoom;

	struct MarginSet
	{
		bool mUp;
		bool mRight;
		bool mDown;
		bool mLeft;
	};
	MarginSet				m_MarginSet;
	void					initMarginSet();
	int						getModulePlaceData(int roomCnt, int x, int y);
	void					roomBoundaryCheck(int roomCnt, int x, int y, int maxX, int maxY);
	cocos2d::DrawNode*		makeRoomRect(int width, int height, int marginSize, MarginSet margin, cocos2d::Color4B fillColor);
	cocos2d::DrawNode*		drawMap(int margin, int drawScale);

	//Map Window
	cocos2d::Sprite*		m_MapWin;
	bool					m_MapWinOn;
	
	void					drawMapWin();
	float					fitToWin();


	//Minimap
	cocos2d::Sprite*		m_MinimapFrame;
	cocos2d::Sprite*		m_MinimapMask;

	void					drawMiniMap();
};

END_NS_AT



