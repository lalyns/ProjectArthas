/************************************************************************/
/*
CLASS			: LoadingSceneUILayer
Author			: �����
����			: LoadingScene�� �ʿ��� UILayer
���� ��������	: 2014-10-31
���� ������		: �����
���� ��������	:
Comment			:
*/
/************************************************************************/

#pragma once
#include "cocos2d.h"
#include "GameManager.h"
#include "Util.h"
#include "UILayer.h"

BEGIN_NS_AT

class LoadingSceneUILayer : public UILayer
{
public:
	OVERRIDE bool			init();
	OVERRIDE void			update(float dTime);

	CREATE_FUNC(LoadingSceneUILayer);

private:

};

END_NS_AT